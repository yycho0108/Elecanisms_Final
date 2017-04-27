#include <p24FJ128GB206.h>
#include <stdint.h>
#include "config.h"
#include "common.h"
#include "usb.h"
#include "usb2.h"
#include "pin.h"
#include "ui.h"
#include "spi.h"
#include "timer.h"
#include "oc.h"
#include "int.h"
#include "uart.h"
#include "i2c.h"
#include "lcd.h"

#include <stdio.h>

#define cap(mn,x,mx) ((mx)<(x))?(mx):((mn)>(x))?(mn):(x)

// ======== PIN SETUP =========
#define W_X_SERVO_PIN &D[0]
#define W_Y_SERVO_PIN &D[1]

#define R_COIN_PIN &D[2]

// servos for paddle
#define W_L_SERVO_PIN &D[3]
#define W_R_SERVO_PIN &D[4]

// center rotary wall
#define W_C_SERVO_PIN &D[5]

// Electromagnet control Pins
#define W_ELECTRO_PIN &D[6]

//Ball Release
#define W_B_SERVO_PIN &D[7]

// LCD Pins
#define LCD_SDA_PIN &D[8]
#define LCD_SCL_PIN &D[9]

//Read if player 2 is ready
#define R_START_PIN &D[10]

// #define BALL_R_START_PIN &D[10]
#define R_L_SERVO_PIN &D[11]
#define R_R_SERVO_PIN &D[12]
#define R_END_PIN &D[13]

// center servo potentiometer
#define R_C_SERVO_PIN &A[0]
// read electromagnet, treat it "like" a digital pin
#define R_ELECTRO_PIN &A[1]

// ======== PIN SETUP =========

enum {IDLE, WAIT_COIN, SETUP_BOARD, WAIT_PLAYERS, RUN, END};

#define true 1
#define false 0
typedef unsigned char bool;


uint8_t state = 0;

/* ELECTROMAGNET */
#define EM_COOLDOWN_PER 8 // cooldown 4 sec.
#define EM_ON_PER 8 // on for 4 sec.
enum {EM_READY, EM_ON, EM_COOLDOWN};
volatile unsigned char electromagnet_state = EM_READY;
uint8_t electromag_counter=0;

// SERVO_OFFSET defines offset from "horizontal"
// HS805BB Datasheet : https://www.servocity.com/hs-805bb-servo 
// MAX TRAVEL : 199.5 deg.
// PWM Range : 556-2420 us
// THUS 660-2340 maps to approx. -90 - 90 deg.

uint16_t calc_servo_pos(float deg){
	deg = cap(-90,deg,90);
	// deg = deg;
	return (deg + 90) / 180 * 0xFFFF;
	/*Takes servo position between -90 and 90
	  Returns 16 bit fixed point fraction between 0 and 1*/
}

uint8_t string[40];

// USB-Related
#define WRITE_X 1
#define WRITE_Y 2
#define TOGGLE_LED 3

volatile float s_x=0, s_y=0;

void write_x(){
	s_x = ((float)(USB_setup.wIndex.w)-0x7FFF)/0x7FFF; // -1 ~ 1
	s_x *= 90; // -15 ~ 15

	BD[EP0IN].bytecount = 0;         // set EP0 IN byte count to 1
	BD[EP0IN].status = 0xC8;  
}

void write_y(){
	s_y = ((float)(USB_setup.wIndex.w)-0x7FFF)/0x7FFF;
	s_y *= 90; // -15 ~ 15

	BD[EP0IN].bytecount = 0;         // set EP0 IN byte count to 1
	BD[EP0IN].status = 0xC8;  
}

void toggle_led(){
	led_toggle(&led2);
	BD[EP0IN].bytecount = 0;         // set EP0 IN byte count to 1
	BD[EP0IN].status = 0xC8;  
}

void registerUSBEvents(){
	registerUSBEvent(write_x, WRITE_X);
	registerUSBEvent(write_y, WRITE_Y);
	registerUSBEvent(toggle_led, TOGGLE_LED);
}


//INTERRUPT FUNCTIONS
volatile bool coin = false;
volatile bool endlimit = false;
volatile bool timelimit = false;
volatile bool player2ready = false;
volatile bool ballinplace = false;
volatile bool startgame_flag = false;

int remaining_time = 0;

void coin_inserted(){
	//Hacky Soln.?
	//static bool first = true;
	//if(first){
	//	first = false;
	//	return;
	//}
	if(!coin){
		printf("sfx:coin\n");
		printf("COIN INSERTED : %d\n", coin);
		coin = true;
		led_toggle(&led3);
	}
}

void end_reached(){
	endlimit = true;
	led_toggle(&led3);
}

void player_ready(){
	player2ready = true;
	// led_toggle(&led1);
}

void ball_ready(){
	ballinplace = true;
	led_toggle(&led2);
}

//State machine helper functions
void do_balance(void){
	// TODO : account for offset
	pin_write(W_X_SERVO_PIN, calc_servo_pos(0));
	pin_write(W_Y_SERVO_PIN, calc_servo_pos(0));
}

void center_ob(void){
	float c_pot = pin_read(R_C_SERVO_PIN)/65535. - 0.5;
	c_pot *= 180;
	//printf("%f\n", c_pot);
	pin_write(W_C_SERVO_PIN, calc_servo_pos(c_pot));
}

void flipper_ob(void){
	unsigned char f_l = pin_read(R_L_SERVO_PIN);
	unsigned char f_r = pin_read(R_R_SERVO_PIN);

	if(f_l || f_r){
		printf("sfx:flipper\n");
	}

	pin_write(W_R_SERVO_PIN, calc_servo_pos(f_l*90));
	pin_write(W_L_SERVO_PIN, calc_servo_pos((1 - f_r)*90));
}

void electromag_ob(void){
	unsigned char e = pin_read(R_ELECTRO_PIN);
	bool em_pressed = pin_read(R_ELECTRO_PIN) > 32768;

	led_write(&led1,em_pressed);

	if(timer_flag(&timer4)){
		timer_lower(&timer4);
		++electromag_counter;
	}

	switch(electromagnet_state){
		case EM_READY:
			if(em_pressed){
				printf("sfx:electromagnet\n");
				pin_write(W_ELECTRO_PIN, 1);
				electromagnet_state = EM_ON;
				timer_stop(&timer4);
				timer_setPeriod(&timer4, 0.5);
				timer_start(&timer4);
				electromag_counter = 0; // reset counter
			}
			break;
		case EM_ON:
			if(electromag_counter > EM_ON_PER){
				pin_write(W_ELECTRO_PIN, 0);
				electromagnet_state = EM_COOLDOWN;
				timer_stop(&timer4);
				timer_start(&timer4);
				electromag_counter = 0; // reset counter
			}
			break;
		case EM_COOLDOWN:
			if(electromag_counter > EM_COOLDOWN_PER){
				electromagnet_state = EM_READY;
			}
			break;
	}

}

void do_obstacles(void){
	// Handle the obstacles for P2
	flipper_ob();
	center_ob();
	electromag_ob();
}

void do_wii(void){
	pin_write(W_X_SERVO_PIN, calc_servo_pos(s_x+10));
	pin_write(W_Y_SERVO_PIN, calc_servo_pos(s_y+2)); //account for offset
}

void print_lcd(char *stuff_to_display){
	lcd_print(&lcd[0], stuff_to_display);
}


//State machine main functions

void null_func(void){}

typedef void (*f_ctor)(void);
typedef char (*f_exec)(void);
typedef void (*f_dtor)(void);

typedef struct {
	f_ctor ctor;
	f_exec exec;
	f_dtor dtor;
} State;

int delay_cnt;
void idle_ctor(void){
	delay_cnt = 0;
	print_lcd((char*)"Staring Up...");
}

char idle(void){
	if(timer_flag(&timer3)){
		timer_lower(&timer3);
		led_toggle(&led3);
		++delay_cnt;
	}

	return (delay_cnt > 4)? IDLE: WAIT_COIN; // delay 2 sec.
}

void coin_ctor(void){
	print_lcd((char*)"Please Insert Coin");
	coin = false;
	//printf("COIN_CTOR: %d\n", coin);
}

char waitforcoin(void){
	//printf("WAITING ... : %d\n", coin);
	if(timer_flag(&timer3)){
		// Just Indicate Status 
		timer_lower(&timer3);
		led_toggle(&led1);
	}
	return coin? SETUP_BOARD : WAIT_COIN;
}


char setup(void){
	// Get angle from accelerometer
	// If further than some threshold:
	//    Move servos, keep track of PWM for it.
	// Else If closer than threshold
	//    shut down servos, 
	//    set up servo with new 0.
	do_balance();
	startgame_flag = false;
	return WAIT_PLAYERS;
}

void wait_player_ctor(void){
	print_lcd((char*)"Player 2:  Place ball at start and press START button when ready.");

	player2ready = false;
	ballinplace = false;
}

char wait_players(void){
	startgame_flag = true;
	if(timer_flag(&timer3)){ // check every .5 seconds, as setup initially
		timer_lower(&timer3);
		led_toggle(&led2);
	}

	// Check for ball in start
	// if (!ballinplace){
	// 	startgame_flag = false;
	// 	print_lcd((char*)"Place ball at start");
	// }
	// Check for player 2 ready
	if (!player2ready){
		startgame_flag = false;
		print_lcd((char*)"Player 2:  Place ball at start and press START button when ready.");
	}
	return startgame_flag?RUN:WAIT_PLAYERS;
}

void run_ctor(void){
	remaining_time = 240;
	timelimit = false;
	endlimit = false;
	pin_write(W_B_SERVO_PIN, calc_servo_pos(90));
}

char run(void){
	do_obstacles(); // Handle player 2 stuff
	do_wii(); // Handle player 1 balancing
	//TODO : also account for time limit
	char s[32] = "Reach The Goal";
	if(timer_flag(&timer3)){
		timer_lower(&timer3);
		sprintf(s,"Reach The Goal : %d", remaining_time/2);
		print_lcd(s);
		if(--remaining_time <= 0){ // = 30 sec.
			timelimit = true;
		}
	}
	return (endlimit || timelimit)? END : RUN;
}

char end(void){
	coin = false;
	char s[32] = {};
	printf("sfx:end\n");

	sprintf(s, "Game Over : Player %d Wins!",endlimit?1:2);
	print_lcd(s);

	pin_write(W_B_SERVO_PIN, calc_servo_pos(0));
	//return END;
	return IDLE;
}

State s_idle = {idle_ctor,idle,null_func};
State s_wait_coin= {coin_ctor,waitforcoin,null_func};
State s_setup= {null_func,setup,null_func};
State s_wait_players= {wait_player_ctor,wait_players,null_func};
State s_run={run_ctor,run,null_func};
State s_end={null_func,end,null_func};
State* states[] = {&s_idle, &s_wait_coin,&s_setup,&s_wait_players,&s_run,&s_end};

int16_t main(void) {
	init_clock();
	init_ui();
	init_pin();
	init_int();
	init_oc();
	init_spi();
	init_timer();
	timer_initDelayMicro(&timer5);
	init_lcd();
	init_uart();

	int cnt = 0;

	oc_servo(&oc1, W_Y_SERVO_PIN, &timer1, 20e-3, 660e-6, 2340e-6, calc_servo_pos(0));
	oc_servo(&oc2, W_X_SERVO_PIN, &timer1, 20e-3, 660e-6, 2300e-6, calc_servo_pos(0));
	oc_servo(&oc3, W_C_SERVO_PIN, &timer1, 20e-3, 660e-6, 2340e-6, calc_servo_pos(0));
	oc_servo(&oc4, W_L_SERVO_PIN, &timer1, 20e-3, 660e-6, 2340e-6, calc_servo_pos(0));
	oc_servo(&oc5, W_R_SERVO_PIN, &timer1, 20e-3, 660e-6, 2340e-6, calc_servo_pos(0));
	//oc_servo(&oc6, W_B_SERVO_PIN, &timer1, 20e-3, 660e-6, 2340e-6, calc_servo_pos(0));

	pin_digitalIn(R_COIN_PIN);
	int_attach(&int1, R_COIN_PIN, INT_FALLING, &coin_inserted);
	pin_digitalIn(R_END_PIN);
	int_attach(&int2, R_END_PIN, INT_FALLING, &end_reached);
	pin_digitalIn(R_START_PIN);
	int_attach(&int3, R_START_PIN, INT_FALLING, &player_ready);

	pin_digitalOut(W_ELECTRO_PIN);

	// pin_digitalIn(BALL_R_START_PIN);
	// int_attach(&int4, BALL_R_START_PIN, INT_FALLING, &ball_ready);

	pin_analogIn(R_ELECTRO_PIN);

	pin_analogIn(R_C_SERVO_PIN);

	led_on(&led1);
	timer_setPeriod(&timer3, 0.5);
	timer_start(&timer3);

	timer_setPeriod(&timer4, 0.5);

	registerUSBEvents();

	InitUSB();
	while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
		ServiceUSB();
	}

	bool game_on = true;
	//state = SETUP_BOARD;
	unsigned char state_id = WAIT_PLAYERS;
	states[state_id]->ctor();

	while(game_on){
		ServiceUSB();
		// State* current_state = states[state_id];
		// unsigned char next_state_id = current_state->exec();
		unsigned char next_state_id = states[state_id]->exec();
		//printf("State : %d -> %d\n", state_id, next_state_id);
		if(next_state_id != state_id){ // == transition
			states[state_id]->dtor();
			state_id = next_state_id;
			states[state_id]->ctor();
		}
	}
}
