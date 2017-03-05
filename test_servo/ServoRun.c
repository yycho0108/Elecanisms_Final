#include <p24FJ128GB206.h>
#include <stdint.h>
#include "config.h"
#include "common.h"
// #include "usb.h"
#include "pin.h"
#include "ui.h"
#include "spi.h"
#include "timer.h"
#include "oc.h"

#include "uart.h"
#include <stdio.h>

#define SERVO_1_OFFSET 20.0
#define SERVO_2_OFFSET 9.5

// SERVO_OFFSET defines offset from "horizontal"

// HS805BB Datasheet : https://www.servocity.com/hs-805bb-servo 
// MAX TRAVEL : 199.5 deg.
// PWM Range : 556-2420 us
// THUS 660-2340 maps to approx. -90 - 90 deg.
 
uint16_t calc_servo_pos(float deg){
	deg = (deg > 90)?90:deg; // cap
	/*Takes servo position between -90 and 90
	  Returns 16 bit fixed point fraction between 0 and 1*/
	return ((deg + 90)*65535)/180;
}

uint8_t string[40];

int16_t main(void) {
	init_clock();
	init_uart();

	init_ui();
	init_pin();
	init_oc();
	init_spi();
	init_timer();

	_PIN *servo_1 = &D[0];
	_PIN *servo_2 = &D[1];
	_PIN *pot_read_1 = &A[0];
	_PIN *pot_read_2 = &A[1];
	oc_servo(&oc1, servo_1, &timer1, 20e-3, 660e-6, 2340e-6, calc_servo_pos(0));
	oc_servo(&oc2, servo_2, &timer2, 20e-3, 660e-6, 2340e-6, calc_servo_pos(0));


    led_on(&led1);
    timer_setPeriod(&timer3, 0.5);
    timer_start(&timer3);

	while(1){
		float pot1 = pin_read(pot_read_1)/65535. - 0.5;
		float pot2 = pin_read(pot_read_2)/65535. - 0.5;
		pot1 *= 180; //map to degrees
		pot2 *= 180;
		
		pin_write(servo_1, calc_servo_pos(pot1 + SERVO_1_OFFSET)); //account for offset
		pin_write(servo_2, calc_servo_pos(pot2 + SERVO_2_OFFSET));
		
		if(timer_flag(&timer3)){
			timer_lower(&timer3);
            led_toggle(&led1);
			printf("pot1 : %f, pot2 : %f\n", pot1, pot2);
		}
	}
}
