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
#include <stdio.h>

#define cap(mn,x,mx) ((mx)<(x))?(mx):((mn)>(x))?(mn):(x)

#define X_SERVO_PIN &D[0]
#define Y_SERVO_PIN &D[1]

#define COIN_PIN &D[2]

// servos for paddle
#define L_SERVO_PIN &D[3]
#define R_SERVO_PIN &D[4]

// center rotary wall
#define C_SERVO_PIN &D[5]

// Electromagnet control Pins
#define ELECTRO_PIN &D[6]

// Read Pins

#define START_PIN &D[7]
#define BALL_START_PIN &D[8]
#define L_READ_PIN &D[9]
#define R_READ_PIN &D[10]
#define END_PIN &D[11]
#define ELECTRO_READ_PIN &D[12]

#define C_READ_PIN &A[0]


#define LIMIT_PIN 0

enum {WAIT_COIN, SETUP_BOARD, WAIT_PLAYERS, RUN, END};

#define true 1
#define false 0

typedef unsigned char bool;
uint8_t state = 0;

volatile bool electromagnet_on = false;
volatile bool electromagnet_cooldown = false;


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
volatile bool player2ready = false;
volatile bool ballinplace = false;
volatile bool startgame_flag = false;

void coin_inserted(){
	//printf("COIN INSERTED!!\n");
	coin = true;
	led_toggle(&led3);
}

void end_reached(){
	endlimit = true;
	led_toggle(&led3);
}

void player_ready(){
	player2ready = true;
	led_toggle(&led1);
}

void ball_ready(){
	ballinplace = true;
	led_toggle(&led2);
}

//State machine helper functions
void do_balance(void){
	// TODO : account for offset
	pin_write(X_SERVO_PIN, calc_servo_pos(0));
	pin_write(Y_SERVO_PIN, calc_servo_pos(0));
}

void center_ob(void){
	float c_pot = pin_read(C_READ_PIN)/65535. - 0.5;
	c_pot *= 180;
	pin_write(C_SERVO_PIN, calc_servo_pos(c_pot));
}

void flipper_ob(void){
	pin_write(R_SERVO_PIN, calc_servo_pos(pin_read(L_READ_PIN)*90));
	pin_write(L_SERVO_PIN, calc_servo_pos((1 - pin_read(R_READ_PIN))*90));
}

void electromag_ob(void){
	// if(timer_flag(&timer4)){ // check every 1 second
	// 	timer_lower(&timer4);
	// 	if(electromagnet_on){
	// 		electromagnet_on = false;
	// 	}
	// 	else if(ELECTRO_READ_PIN){
	// 		//Turn off electromagnet
	// 		electromagnet_on = true;
	// 	}
	// 	pin_write(ELECTRO_PIN,electromagnet_on);
	// 	led_write(&led1,electromagnet_on);
	// 	}
	// }
}

void do_obstacles(void){
	// Handle the obstacles for P2
	flipper_ob();
	center_ob();
	electromag_ob();
}




void do_wii(void){
	pin_write(X_SERVO_PIN, calc_servo_pos(s_x));
	pin_write(Y_SERVO_PIN, calc_servo_pos(s_y)); //account for offset
}

void print_lcd(char *stuff_to_display){
}


//State machine main functions
void waitforcoin(void){
	if(timer_flag(&timer3)){ // check every .5 seconds, as setup initially
		timer_lower(&timer3);
		led_toggle(&led1);
	}
	if(coin){
		state = SETUP_BOARD;
		return;
	}
}

void setup(void){
	// ServiceUSB();
	// Get angle from accelerometer
	// If further than some threshold:
	//    Move servos, keep track of PWM for it.
	// Else If closer than threshold
	//    shut down servos, 
	//    set up servo with new 0.
	do_balance();
	startgame_flag = false;
	state = WAIT_PLAYERS;
}



void wait_players(void){
	startgame_flag = true;
	if(timer_flag(&timer3)){ // check every .5 seconds, as setup initially
		timer_lower(&timer3);
		led_toggle(&led2);
	}

	// Check for ball in start
	if (!ballinplace){
		startgame_flag = false;
		print_lcd("Place ball at start");
	}
	// Check for player 2 ready
	else if (!player2ready){
		print_lcd("Player 2:  Press START button when ready.");
		startgame_flag = false;
	}

	if (startgame_flag){
		state = RUN;
	}
}

void run(void){
	do_obstacles(); // Handle player 2 stuff
	do_wii(); // Handle player 1 balancing
	//TODO : also account for time limit
	if (endlimit == true){ //Actually going to be pin_get for limit switch pin or something.
		state = END;
	}
}

void end(void){
	coin = false;
}

int16_t main(void) {
	init_clock();
	init_ui();
	init_pin();
	init_int();
	init_oc();
	init_spi();
	init_timer();
	init_uart();


	// _PIN *lin_acc_a = LIN_ACC_PINA;
	// _PIN *lin_acc_b = LIN_ACC_PINB;

	//_PIN *pot_read_1 = &A[0];
	//_PIN *pot_read_2 = &A[1];

	oc_servo(&oc2, X_SERVO_PIN, &timer1, 20e-3, 660e-6, 2300e-6, calc_servo_pos(0));
	oc_servo(&oc1, Y_SERVO_PIN, &timer1, 20e-3, 660e-6, 2340e-6, calc_servo_pos(0));
	oc_servo(&oc3, C_SERVO_PIN, &timer1, 20e-3, 660e-6, 2340e-6, calc_servo_pos(0));
	oc_servo(&oc4, L_SERVO_PIN, &timer1, 20e-3, 660e-6, 2340e-6, calc_servo_pos(0));
	oc_servo(&oc5, R_SERVO_PIN, &timer1, 20e-3, 660e-6, 2340e-6, calc_servo_pos(0));

	pin_digitalIn(COIN_PIN);
	int_attach(&int1, COIN_PIN, INT_FALLING, &coin_inserted);
	pin_digitalIn(END_PIN);
	int_attach(&int2, END_PIN, INT_FALLING, &end_reached);
	pin_digitalIn(START_PIN);
	int_attach(&int3, START_PIN, INT_FALLING, &player_ready);
	pin_digitalIn(BALL_START_PIN);
	int_attach(&int4, BALL_START_PIN, INT_FALLING, &ball_ready);

	pin_analogIn(C_READ_PIN);

	led_on(&led1);
	// led_on(&led2);


	timer_setPeriod(&timer3, 0.5);
	timer_start(&timer3);

	timer_setPeriod(&timer4, 1);
	timer_start(&timer4);

	registerUSBEvents();

	InitUSB();
	while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
		ServiceUSB();
	}

	bool game_on = true;
	state = SETUP_BOARD;
	// int counter = 0;
	while(game_on){
		ServiceUSB();
		//printf("[%d : %d]\n", state, pin_read(L_READ_PIN));
		switch(state){
			case WAIT_COIN:
				waitforcoin();
				break;
			case SETUP_BOARD:
				setup();
				break;
			case WAIT_PLAYERS:
				wait_players();
				break;
			case RUN:
				run();
				break;
			case END:
				end();
				break;
			default:
				break;

		}
	}
}
