#include <p24FJ128GB206.h>
#include <stdint.h>
#include "config.h"
#include "common.h"
#include "pin.h"
#include "spi.h"
#include "mpu.h"

// #define TOGGLE_LED1         1
// #define TOGGLE_LED2         2
// #define READ_SW1            3
// #define MPU_WRITE_REG       4
// #define MPU_READ_REG        5
// #define MPU_WRITE_REGS      6
// #define MPU_READ_REGS       7
// #define TOGGLE_LED3         8
// #define READ_SW2            9
// #define READ_SW3            10


_PIN *SCK, *MISO, *MOSI, *CSN, *INT;

float accel_mults[4] = {1./16384., 1./8192., 1./4096., 1./2048.};
float accel_mult = accel_mults[1];
float gyro_mults[4] = {1./131., 1./65.5, 1./32.8, 1./16.4};
float gyro_mult = gyro_mults[3];
#define mag_x_mult 0.15
#define mag_y_mult 0.15
#define mag_z_mult 0.15

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
        led_toggle(&led2);
        pin_clear(CSN); // select
        spi_transfer(&spi1, 0x80|address);
        for (i = 0; i<n; i++)
            buffer[i] = spi_transfer(&spi1, 0);
            //buffer[i] = i+1;
        pin_set(CSN);
    } else {
        led_toggle(&led3);
        for (i = 0; i<n; i++){
            buffer[i] = 0xFF;
          }
    }
}


// void VendorRequests(void) {
//     WORD32 address;
// 
//     switch (USB_setup.bRequest) {
//         case TOGGLE_LED1:
//             led_toggle(&led1);
//             BD[EP0IN].bytecount = 0;         // set EP0 IN byte count to 0
//             BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
//             break;
//         case TOGGLE_LED2:
//             led_toggle(&led2);
//             BD[EP0IN].bytecount = 0;         // set EP0 IN byte count to 0
//             BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
//             break;
//         case READ_SW1:
//             BD[EP0IN].address[0] = (uint8_t)sw_read(&sw1);
//             BD[EP0IN].bytecount = 1;         // set EP0 IN byte count to 1
//             BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
//             break;
//         case MPU_WRITE_REG:
//             mpu_writeReg(USB_setup.wValue.b[0], USB_setup.wIndex.b[0]);
//             BD[EP0IN].bytecount = 0;         // set EP0 IN byte count to 0
//             BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
//             break;
//         case MPU_READ_REG:
//             BD[EP0IN].address[0] = mpu_readReg(USB_setup.wValue.b[0]);
//             BD[EP0IN].bytecount = 1;         // set EP0 IN byte count to 1
//             BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
//             break;
//         case MPU_WRITE_REGS:
//             USB_request.setup.bmRequestType = USB_setup.bmRequestType;
//             USB_request.setup.bRequest = USB_setup.bRequest;
//             USB_request.setup.wValue.w = USB_setup.wValue.w;
//             USB_request.setup.wIndex.w = USB_setup.wIndex.w;
//             USB_request.setup.wLength.w = USB_setup.wLength.w;
//             break;
//         case MPU_READ_REGS:
//             mpu_readRegs(USB_setup.wValue.b[0], BD[EP0IN].address, USB_setup.wLength.b[0]);
//             BD[EP0IN].bytecount = USB_setup.wLength.b[0];
//             BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
//             break;
//         case TOGGLE_LED3:
//             led_toggle(&led3);
//             BD[EP0IN].bytecount = 0;         // set EP0 IN byte count to 0
//             BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
//             break;
//         case READ_SW2:
//             BD[EP0IN].address[0] = (uint8_t)sw_read(&sw2);
//             BD[EP0IN].bytecount = 1;         // set EP0 IN byte count to 1
//             BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
//             break;
//         case READ_SW3:
//             BD[EP0IN].address[0] = (uint8_t)sw_read(&sw3);
//             BD[EP0IN].bytecount = 1;         // set EP0 IN byte count to 1
//             BD[EP0IN].status = 0xC8;         // send packet as DATA1, set UOWN bit
//             break;
//         default:
//             USB_error_flags |= 0x01;    // set Request Error Flag
//     }
// }
// 
// void VendorRequestsIn(void) {
//     switch (USB_request.setup.bRequest) {
//         default:
//             USB_error_flags |= 0x01;                    // set Request Error Flag
//     }
// }
// 
// void VendorRequestsOut(void) {
//     WORD32 address;
// 
//     switch (USB_request.setup.bRequest) {
//         case MPU_WRITE_REGS:
//             mpu_writeRegs(USB_request.setup.wValue.b[0], BD[EP0OUT].address, USB_request.setup.wLength.b[0]);
//             break;
//         default:
//             USB_error_flags |= 0x01;                    // set Request Error Flag
//     }
// }

void mpu_init(void){

        init_pin();
        init_spi();

        SCK = &D[11];
        MISO = &D[13];
        MOSI = &D[12];
        //INT = &D[9];
        CSN = &D[10];
        pin_digitalOut(CSN);
        pin_digitalOut(MOSI);
        pin_digitalIn(MISO);
        pin_digitalOut(SCK);

        pin_set(CSN);

        spi_open(&spi1, MISO, MOSI, SCK, 1e6, 0);
        // Reset the MPU-9250.
        mpu_writeReg(MPU_PWR_MGMT_1, 0x80);
        // Use DLPF, set gyro bandwidth to 184 Hz and temp bandwidth to 188 Hz.
        mpu_writeReg(MPU_CONFIG, 0x01);
        // Set gyro range to +/-2000 dps.
        mpu_writeReg(MPU_GYRO_CONFIG, 0x18);
        // Set accel range to +/-4 g.
        mpu_writeReg(MPU_ACCEL_CONFIG, 0x08);
        // Set accel data rate, enable accel LPF, set bandwith to 184 Hz.
        mpu_writeReg(MPU_ACCEL_CONFIG2, 0x09);
        // Configure INT pin to latch and clear on any read
        mpu_writeReg(MPU_INT_PIN_CFG, 0x30);
        // Set I2C master mode, reset I2C slave module, and put serial interface
        // in SPI mode.
        mpu_writeReg(MPU_USER_CTRL, 0x30);
        // Confiugre I2C slave interface for a 400-kHz clock.
        mpu_writeReg(MPU_I2C_MST_CTRL, 0x0D);
        // Set I2C Slave 0 address to AK8963's I2C address.
        mpu_writeReg(MPU_I2C_SLV0_ADDR, MAG_I2C_ADDR);

        // Reset the AK8963.
        mpu_writeReg(MPU_I2C_SLV0_REG, MAG_CNTL2);
        mpu_writeReg(MPU_I2C_SLV0_DO, 0x01);
        mpu_writeReg(MPU_I2C_SLV0_CTRL, 0x81);
        time.sleep(mag_delay);

        // Put AK8963 in fuse ROM access mode.
        mpu_writeReg(MPU_I2C_SLV0_REG, MAG_CNTL1);
        mpu_writeReg(MPU_I2C_SLV0_DO, 0x0F);
        mpu_writeReg(MPU_I2C_SLV0_CTRL, 0x81);
        time.sleep(mag_delay);

        // Read ASA values from AK8963.
        mpu_writeReg(MPU_I2C_SLV0_ADDR, MAG_I2C_ADDR|0x80);
        mpu_writeReg(MPU_I2C_SLV0_REG, MAG_ASAX);
        mpu_writeReg(MPU_I2C_SLV0_CTRL, 0x83);
        time.sleep(mag_delay);
        values = mpu_readRegs(MPU_EXT_SENS_DATA_00, 3);
        mag_x_mult = 0.15*(float(values[0] - 128)/256. + 1.);
        mag_y_mult = 0.15*(float(values[1] - 128)/256. + 1.);
        mag_z_mult = 0.15*(float(values[2] - 128)/256. + 1.);

        // Put AK8963 into power-down mode.
        mpu_writeReg(MPU_I2C_SLV0_ADDR, MAG_I2C_ADDR);
        mpu_writeReg(MPU_I2C_SLV0_REG, MAG_CNTL1);
        mpu_writeReg(MPU_I2C_SLV0_DO, 0x00);
        mpu_writeReg(MPU_I2C_SLV0_CTRL, 0x81);
        time.sleep(mag_delay); //

        // Configure AK8963 for continuous 16-bit measurement mode at 100 Hz.
        mpu_writeReg(MPU_I2C_SLV0_REG, MAG_CNTL1);
        mpu_writeReg(MPU_I2C_SLV0_DO, 0x16);
        mpu_writeReg(MPU_I2C_SLV0_CTRL, 0x81);
        time.sleep(mag_delay);

        mpu_set_accel_scale(2.);
        mpu_set_gyro_scale(250.);
}


void mpu_set_accel_scale(uint8_t scale){
      if(scale<=2.){
          // Set accel range to +/-2 g.
          mpu_writeReg(MPU_ACCEL_CONFIG, 0x00);
          accel_mult = accel_mults[0];
      }
      else if (scale<=4.){
          // Set accel range to +/-4 g.
          mpu_writeReg(MPU_ACCEL_CONFIG, 0x08);
          accel_mult = accel_mults[1];
      }
      else if (scale<=8.){
          // Set accel range to +/-8 g.
          mpu_writeReg(MPU_ACCEL_CONFIG, 0x10);
          accel_mult = accel_mults[2];
      }
      else{
          // Set accel range to +/-16 g.
          mpu_writeReg(MPU_ACCEL_CONFIG, 0x18);
          accel_mult = accel_mults[3];
      }
}

uint8_t mpu_get_accel_scale(void){
    uint8_t scale = mpu_readReg(MPU_ACCEL_CONFIG);
    scale = scale>>3;
    accel_mult = accel_mults[scale];
    return 2.*(1<<scale);
}

// void mpu_set_gyro_scale(uint16_t scale){
//     if scale<=250.{
//         // Set gyro range to +/-250 dps.
//         mpu_writeReg(MPU_GYRO_CONFIG, 0x00);
//         gyro_mult = gyro_mults[0];
//     }
//     else if scale<=500.{
//         // Set gyro range to +/-500 dps.
//         mpu_writeReg(MPU_GYRO_CONFIG, 0x08);
//         gyro_mult = gyro_mults[1];
//       }
//     else if scale<=1000.{
//         // Set gyro range to +/-1000 dps.
//         mpu_writeReg(MPU_GYRO_CONFIG, 0x10);
//         gyro_mult = gyro_mults[2];
//       }
//     else{
//         // Set gyro range to +/-2000 dps.
//         mpu_writeReg(MPU_GYRO_CONFIG, 0x18);
//         gyro_mult = gyro_mults[3];
//       }
// }
// 
// uint16_t mpu_get_gyro_scale(void){}
//     uint8_t scale = mpu_readReg(MPU_GYRO_CONFIG);
//     scale = scale>>3;
//     gyro_mult = gyro_mults[scale];
//     return 250.*(1<<scale);
//   }

accel_t mpu_read_accel(void){
    accel_t accels;
    uint8_t values[6] = mpu_readRegs(MPU_ACCEL_XOUT_H, 6);
    uint16_t x = 256*values[0] + values[1];
    uint16_t y = 256*values[2] + values[3];
    uint16_t z = 256*values[4] + values[5];


    float accelx = (int16_t)x*accel_mult;
    float accely = (int16_t)y*accel_mult;
    float accelz = (int16_t)z*accel_mult;

    accels.x =  (int16_t)accelx;
    accels.y =  (int16_t)accely;
    accels.z =  (int16_t)accelz;
    return accels;
}
// 
// float mpu_read_gyro(void){
//     values = mpu_readRegs(MPU_GYRO_XOUT_H, 6)
// 
//     x = 256*values[0] + values[1];
//     y = 256*values[2] + values[3];
//     z = 256*values[4] + values[5];
// 
//     x = x - 65536 if x>32767 else x;
//     y = y - 65536 if y>32767 else y;
//     z = z - 65536 if z>32767 else z;
// 
//     return [float(x)*gyro_mult, float(y)*gyro_mult, float(z)*gyro_mult];
// }
// 
// uint8_t mpu_read_temp(void){
//     values = mpu_readRegs(MPU_TEMP_OUT_H, 2);
//     value = 256*values[0] + values[1];
//     value = value - 65536 if value>32767 else value;
//     return value;
// }
// 
// uint8_t mpu_mag_whoami(void){
//     mpu_writeReg(MPU_I2C_SLV0_ADDR, MAG_I2C_ADDR|0x80);
//     mpu_writeReg(MPU_I2C_SLV0_REG, MAG_WIA);
//     mpu_writeReg(MPU_I2C_SLV0_CTRL, 0x81);
//     time.sleep(mag_delay);
//     return mpu_readReg(MPU_EXT_SENS_DATA_00);
// }
// 
// float mpu_read_mag(void){
//     mpu_writeReg(MPU_I2C_SLV0_ADDR, MAG_I2C_ADDR|0x80);
//     mpu_writeReg(MPU_I2C_SLV0_REG, MAG_HXL);
//     mpu_writeReg(MPU_I2C_SLV0_CTRL, 0x87);
//     time.sleep(mag_delay);
//     values = mpu_readRegs(MPU_EXT_SENS_DATA_00, 6);
// 
//     x = values[0] + 256*values[1];
//     y = values[2] + 256*values[3];
//     z = values[4] + 256*values[5];
// 
//     x = x - 65536 if x>32767 else x;
//     y = y - 65536 if y>32767 else y;
//     z = z - 65536 if z>32767 else z;
// 
//     return [float(x)*mag_x_mult, float(y)*mag_y_mult, float(z)*mag_z_mult];
// }
// int16_t main(void) {
// 
//     InitUSB();                              // initialize the USB registers and serial interface engine
//     while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
//         ServiceUSB();                       // ...service USB requests
//     }
//     while (1) {
//         ServiceUSB();                       // service any pending USB requests
//     }
// }
