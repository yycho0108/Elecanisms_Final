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

#define Y_SERVO_PIN &D[0]
#define X_SERVO_PIN &D[1]

#define COIN_PIN &D[2]

// servos for paddle
#define L_SERVO_PIN &D[3]
#define R_SERVO_PIN &D[4]

// center rotary wall
#define C_SERVO_PIN &D[5]

#define END_PIN &D[6]
#define START_PIN &D[7]

#define LIMIT_PIN 0

#define WAIT_COIN 1
#define SETUP 2
#define RUN 3
#define WAIT_PLAYERS 4
#define END 5

#define true 1
#define false 0

typedef unsigned char bool;
uint8_t state = 0;

// SERVO_OFFSET defines offset from "horizontal"

// HS805BB Datasheet : https://www.servocity.com/hs-805bb-servo 
// MAX TRAVEL : 199.5 deg.
// PWM Range : 556-2420 us
// THUS 660-2340 maps to approx. -90 - 90 deg.

uint16_t calc_servo_pos(float deg){
	deg = cap(-90,deg,90);
	deg = deg * 0.9;
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


void coin_inserted(){
	//printf("COIN INSERTED!!\n");
	coin = true;
	led_toggle(&led3);
}

void end_reached(){
	endlimit = true;
	led_toggle(&led2);
}

void player_ready(){
	player2ready = true;
	led_toggle(&led1);
}

//State machine helper functions
void do_balance(void){
	// TODO : account for offset
	ServiceUSB();
	pin_write(X_SERVO_PIN, calc_servo_pos(0));
	pin_write(Y_SERVO_PIN, calc_servo_pos(0));
}

void do_obstacles(void){
	// Handle the obstacles for P2
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
		state = SETUP;
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
	state = WAIT_PLAYERS;
}

void wait_players(void){
	if(timer_flag(&timer3)){ // check every .5 seconds, as setup initially
		timer_lower(&timer3);
		led_toggle(&led2);
		}
	if (player2ready){
		state = RUN;
		return;
	}
}

void run(void){
	do_obstacles();
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


	//_PIN *pot_read_1 = &A[0];
	//_PIN *pot_read_2 = &A[1];

	oc_servo(&oc2, X_SERVO_PIN, &timer2, 20e-3, 660e-6, 2340e-6, calc_servo_pos(0));
	oc_servo(&oc1, Y_SERVO_PIN, &timer1, 20e-3, 660e-6, 2340e-6, calc_servo_pos(0));

	pin_digitalIn(COIN_PIN);
	int_attach(&int1, COIN_PIN, INT_FALLING, &coin_inserted);
	pin_digitalIn(END_PIN);
	int_attach(&int2, END_PIN, INT_FALLING, &end_reached);
	pin_digitalIn(START_PIN);
	int_attach(&int3, START_PIN, INT_FALLING, &player_ready);

	led_on(&led1);
	// led_on(&led2);

	timer_setPeriod(&timer3, 0.5);
	timer_start(&timer3);

	registerUSBEvents();
	
	InitUSB();
	while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
		ServiceUSB();
	}

	bool game_on = true;
  state = WAIT_COIN;
	//int counter = 0;
	while(game_on){
		//printf("[%d : %d]\n", state, ++counter);
		if(state == WAIT_COIN){
			waitforcoin();
		}
		else if (state == SETUP){
			setup();
		}
		else if (state == WAIT_PLAYERS){
			wait_players();
		}
		else if (state == RUN){
			run();
		}
		else if (state == END){
			end();
			// game_on = false;
			// exit main loop
		}
	}
}
