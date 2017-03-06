#include "usb2.h"

USBEvent usb_events[MAX_USB_HANDLER] = {}; // all explicitly initialized to 0
uint16_t cur_usb_idx = 0;

void registerUSBEvent(usbHandler f, uint16_t flag){
	usb_events[cur_usb_idx].func = f;
	usb_events[cur_usb_idx].flag = flag;
	++cur_usb_idx;
}

void VendorRequests(void){
	int i;
	for(i=0; i<cur_usb_idx; ++i){
		if(USB_setup.bRequest == usb_events[i].flag){
			usb_events[i].func();
			return;
		}	
	}
	// request was never handled
	USB_error_flags |= 0x01;
}

void VendorRequestsIn(void) {
	switch (USB_request.setup.bRequest) {
		default:
			USB_error_flags |= 0x01;                    // set Request Error Flag
	}
}

void VendorRequestsOut(void) {
	//    WORD32 address;
	//
	//    switch (USB_request.setup.bRequest) {
	//        case ENC_WRITE_REGS:
	//            enc_writeRegs(USB_request.setup.wValue.b[0], BD[EP0OUT].address, USB_request.setup.wLength.b[0]);
	//            break;
	//        default:
	//            USB_error_flags |= 0x01;                    // set Request Error Flag
	//    }
}


