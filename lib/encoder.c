#include "encoder.h"

// assumes using SPI 1
Encoder enc = {&spi1, &D[1],&D[0],&D[2],&D[3],(int)2e6};

WORD enc_readReg(Encoder* enc, WORD address) {
    WORD cmd, result;
    cmd.w = 0x4000|address.w; //set 2nd MSB to 1 for a read
    cmd.w |= parity(cmd.w)<<15; //calculate even parity for

    pin_clear(enc->NCS);
    spi_transfer(&spi1, cmd.b[1]);
    spi_transfer(&spi1, cmd.b[0]);
    pin_set(enc->NCS);

    pin_clear(enc->NCS);
    result.b[1] = spi_transfer(&spi1, 0);
    result.b[0] = spi_transfer(&spi1, 0);
    pin_set(enc->NCS);

    return result;
}

WORD enc_getAngle(Encoder* enc){
	WORD addr;
	addr.w = ENC_ANG_REG;
	WORD res =  enc_readReg(enc, addr);
	res.w &= ENC_MASK;
	return res;
}

void init_enc(){
    pin_digitalOut(enc.NCS);
    pin_set(enc.NCS);
	spi_open(enc.spi,enc.MISO,enc.MOSI,enc.SCK,enc.freq, 1);
}
