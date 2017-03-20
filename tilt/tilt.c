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

#define SERVO_1_OFFSET 20.0
#define SERVO_2_OFFSET 9.5
#define cap(mn,x,mx) ((mx)<(x))?(mx):((mn)>(x))?(mn):(x)

#define SERVO_Y_PIN &D[0]
#define SERVO_X_PIN &D[1]
#define COIN_PIN &D[2]

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

void coin_inserted(){
	printf("COIN INSERTED!!\n");
	led_toggle(&led3);
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

	_PIN *servo_y = SERVO_Y_PIN;
	_PIN *servo_x = SERVO_X_PIN;
	_PIN *coin_pin = COIN_PIN;

	//_PIN *pot_read_1 = &A[0];
	//_PIN *pot_read_2 = &A[1];

	oc_servo(&oc2, servo_x, &timer2, 20e-3, 660e-6, 2340e-6, calc_servo_pos(0));
	oc_servo(&oc1, servo_y, &timer1, 20e-3, 660e-6, 2340e-6, calc_servo_pos(0));

    pin_digitalIn(coin_pin);
    int_attach(&int1, coin_pin, INT_FALLING, &coin_inserted);

    led_on(&led1);
	led_on(&led2);

    timer_setPeriod(&timer3, 0.5);
    timer_start(&timer3);

	registerUSBEvents();

	InitUSB();
	while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
		ServiceUSB();
	}

	while(1){
		ServiceUSB();

		//s_x = pin_read(pot_read_1)/65535. - 0.5;
		//s_y = pin_read(pot_read_2)/65535. - 0.5;
		//s_x *= 180; //map to degrees
		//s_y *= 180;
		
		pin_write(servo_x, calc_servo_pos(s_x));
		pin_write(servo_y, calc_servo_pos(s_y)); //account for offset

		//}else{
		//	printf("[ERROR]! : %s\n", string);
		//}
		
		if(timer_flag(&timer3)){
			timer_lower(&timer3);
            led_toggle(&led1);

			printf("s_x : %f, s_y : %f\n", s_x, s_y);

			//printf("s_x : %u, s_y : %u\n", calc_servo_pos(s_x), calc_servo_pos(s_y));
			//printf("coin : %d", pin_read(coin_pin));
		}
	}
}
