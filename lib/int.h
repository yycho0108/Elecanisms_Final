/*
** Copyright (c) 2016, Evan Dorsky
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met: 
** 
**     1. Redistributions of source code must retain the above copyright 
**        notice, this list of conditions and the following disclaimer. 
**     2. Redistributions in binary form must reproduce the above copyright 
**        notice, this list of conditions and the following disclaimer in the 
**        documentation and/or other materials provided with the distribution. 
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
** POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef _INT_H_
#define _INT_H_

#include <stdint.h>
#include "pin.h"

#define INT_RISING 1
#define INT_FALLING 0 // backwards

void init_int(void);

typedef struct _INT {
    uint16_t *IFSn;
    uint16_t *IECn;
    WORD *RPINRn;
    uint8_t rpinshift;
    uint8_t flagbit;
    uint8_t intconbit;
    void (*isr)(struct _INT *self);
    _PIN *pin;
} _INT;

extern _INT int1, int2, int3, int4;

void int_init(_INT *self, uint16_t *IFSn, uint16_t *IECn, WORD *RPINRn, uint8_t rpinshift, uint8_t flagbit, uint8_t intconbit);
void int_lower(_INT *self);
void int_enableInterrupt(_INT *self);
void int_disableInterrupt(_INT *self);
void int_attach(_INT *self, _PIN *pin, uint8_t edge, void (*callback)(_INT *self));

#endif