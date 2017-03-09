#include <p24FJ128GB206.h>
#include <stdint.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "usb.h"
#include "pin.h"
#include "spi.h"

#define TOGGLE_LED1         1
#define TOGGLE_LED2         2
#define READ_SW1            3
#define MPU_WRITE_REG       4
#define MPU_READ_REG        5
#define MPU_WRITE_REGS      6
#define MPU_READ_REGS       7
#define TOGGLE_LED3         8
#define READ_SW2            9
#define READ_SW3            10

_PIN *SCK, *MISO, *MOSI, *CSN, *INT;

void mpu_writeReg(uint8_t address, uint8_t value) {
    if (address<=0x7E) {
        pin_clear(CSN);
        spi_transfer(&spi1, address);
        spi_transfer(&spi1, value);
        pin_set(CSN);
    }
}

uint8_t mpu_readReg(uint8_t address) {
    uint8_t value;

    if (address<=0x7E) {
        pin_clear(CSN);
        spi_transfer(&spi1, 0x80|address);
        value = spi_transfer(&spi1, 0);
        pin_set(CSN);
        return value;
    } else
        return 0xFF;
}

void mpu_writeRegs(uint8_t address, uint8_t *buffer, uint8_t n) {
    uint8_t i;

    if (address+n<=0x7E) {
        pin_clear(CSN);
        spi_transfer(&spi1, address);
        for (i = 0; i<n; i++)
            spi_transfer(&spi1, buffer[i]);
        pin_set(CSN);
    }
}

void mpu_readRegs(uint8_t address, uint8_t *buffer, uint8_t n) {
    uint8_t i;

    if (address+n<=0x7E) {
        led_on(&led1);
        pin_clear(CSN);
        spi_transfer(&spi1, 0x80|address);
        for (i = 0; i<n; i++)
            buffer[i] = spi_transfer(&spi1, 0);
        pin_set(CSN);
    } else {
      led_on(&led2)
        for (i = 0; i<n; i++)
            buffer[i] = 0xFF;
    }
}

//void ClassRequests(void) {
//    switch (USB_setup.bRequest) {
//        default:
//            USB_error_flags |= 0x01;                    // set Request Error Flag
//    }
//}

void VendorRequests(void) {
    WORD32 address;

    switch (USB_setup.bRequest) {
        case TOGGLE_LED1:
            led_toggle(&led1);
            BD[EP0IN].bytecount = 0;         // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case TOGGLE_LED2:
            led_toggle(&led2);
            BD[EP0IN].bytecount = 0;         // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case READ_SW1:
            BD[EP0IN].address[0] = (uint8_t)sw_read(&sw1);
            BD[EP0IN].bytecount = 1;         // set EP0 IN byte count to 1
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case MPU_WRITE_REG:
            mpu_writeReg(USB_setup.wValue.b[0], USB_setup.wIndex.b[0]);
            BD[EP0IN].bytecount = 0;         // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case MPU_READ_REG:
            BD[EP0IN].address[0] = mpu_readReg(USB_setup.wValue.b[0]);
            BD[EP0IN].bytecount = 1;         // set EP0 IN byte count to 1
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case MPU_WRITE_REGS:
            USB_request.setup.bmRequestType = USB_setup.bmRequestType;
            USB_request.setup.bRequest = USB_setup.bRequest;
            USB_request.setup.wValue.w = USB_setup.wValue.w;
            USB_request.setup.wIndex.w = USB_setup.wIndex.w;
            USB_request.setup.wLength.w = USB_setup.wLength.w;
            break;
        case MPU_READ_REGS:
            mpu_readRegs(USB_setup.wValue.b[0], BD[EP0IN].address, USB_setup.wLength.b[0]);
            BD[EP0IN].bytecount = USB_setup.wLength.b[0];
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case TOGGLE_LED3:
            led_toggle(&led3);
            BD[EP0IN].bytecount = 0;         // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case READ_SW2:
            BD[EP0IN].address[0] = (uint8_t)sw_read(&sw2);
            BD[EP0IN].bytecount = 1;         // set EP0 IN byte count to 1
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        case READ_SW3:
            BD[EP0IN].address[0] = (uint8_t)sw_read(&sw3);
            BD[EP0IN].bytecount = 1;         // set EP0 IN byte count to 1
            BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
            break;
        default:
            USB_error_flags |= 0x01;    // set Request Error Flag
    }
}

void VendorRequestsIn(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

void VendorRequestsOut(void) {
    WORD32 address;

    switch (USB_request.setup.bRequest) {
        case MPU_WRITE_REGS:
            mpu_writeRegs(USB_request.setup.wValue.b[0], BD[EP0OUT].address, USB_request.setup.wLength.b[0]);
            break;
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

int16_t main(void) {
    init_clock();
    init_ui();
    init_pin();
    init_spi();

    SCK = &D[11];
    MISO = &D[13];
    MOSI = &D[12];
    INT = &D[9];
    CSN = &D[10];

    pin_digitalOut(CSN);
    pin_set(CSN);

    spi_open(&spi1, MISO, MOSI, SCK, 1e6, 0);

    InitUSB();                              // initialize the USB registers and serial interface engine
    while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
        ServiceUSB();                       // ...service USB requests
    }
    while (1) {
        ServiceUSB();                       // service any pending USB requests
    }
}
