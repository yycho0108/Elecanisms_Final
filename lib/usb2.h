#ifndef __USB2_H__
#define __USB2_H__

#include "usb.h"

#define MAX_USB_HANDLER 32

typedef void (*usbHandler)(void);
// function accepts void, returns void

typedef struct _USBEvent{
	usbHandler func;
	uint16_t flag;
} USBEvent;

extern USBEvent usb_events[MAX_USB_HANDLER];
extern uint16_t cur_usb_idx;

void registerUSBEvent(usbHandler f, uint16_t flag);

#endif
