#include <p24FJ128GB206.h>
#include <stdint.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "pin.h"
#include "i2c.h"
#include "timer.h"
#include "lcd.h"
#include "strm.h"
#include "uart.h"

// I2C Reg (MSB) P7 P6 P5 P4 P3 P2 P1 P0
// Driver pin    D7 D6 D5 D4 ?  E  RW RS

int16_t main(void) {
    init_clock();
    init_ui();
    init_pin();
    init_i2c();
    init_timer();
    timer_initDelayMicro(&timer5);
	led_off(&led2);

    init_lcd();

    printf("____________\r\n");
    char string1[17]="Spark Scrambler";
    char* strptr1=string1;
    char string2[17]="Suction Scissor";
    char* strptr2=string2;

    strm_Scramble(strptr1,2,10);
    strm_Scramble(strptr2,2,10);

    lcd_print2(&lcd[0],strptr1,strptr2);
	led_on(&led2);

    //lcd_print(&lcd[0],"Hello World!");
    //lcd_print1(&lcd[0],"Hello");

	while(1) {
		//
	}
}
