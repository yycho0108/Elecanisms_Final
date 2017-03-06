#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "ui.h"
#include "usb.h"
#include "pin.h"
#include "spi.h"
#include "timer.h"

#define ENC_MAG_REG  0x3FFE
#define ENC_ANG_REG  0x3FFF
#define ENC_MASK     0x3FFF

typedef struct _Encoder{
	_SPI *spi;
	_PIN *MISO, *MOSI,*SCK, *NCS;
	int freq; // 2e6 is a reasonable value maybe
} Encoder;

extern void init_enc();

extern WORD enc_readReg(Encoder* enc, WORD addr);
extern WORD enc_getAngle(Encoder* enc);

extern Encoder enc;

#endif
