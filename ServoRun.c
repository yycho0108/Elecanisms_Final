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

uint16_t calc_servo_pos(int ideal){
  /*Takes servo position between -90 and 90
  Returns 16 bit fixed point fraction between 0 and 1*/
  int adj_ideal = ideal + 90;
  return (ideal*25535)/180;
}

int16_t main(void) {
  init_clock();
  init_ui();
  init_pin();
  init_oc();
  init_spi();
  init_timer();

  _PIN *servo_1 = &D[0];
  _PIN *servo_2 = &D[1];
  _PIN *pot_read_1 = &A[0];
  _PIN *pot_read_2 = &A[1];

  oc_servo(&oc1, servo_1, &timer1, 20e-3, 7e-4,23e-4, calc_servo_pos(0));
  oc_servo(&oc2, servo_2, &timer2, 20e-3, 7e-4,23e-4, calc_servo_pos(0));
while(1){
  uint16_t pot1 = pin_read(pot_read_1);
  pin_write(servo_1, pot1);
  uint16_t pot2 = pin_read(pot_read_2);
  pin_write(servo_2, pot2);
  // pin_write(servo_1, calc_servo_pos(45));
}
}
