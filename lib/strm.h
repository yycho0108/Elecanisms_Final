#ifndef _STRM_H_
#define _STRM_H_

#include <stdint.h>

void strm_CamelCase(char* str, uint8_t a, uint8_t b);
void strm_Caesar(char* str, uint8_t freq, uint8_t shift);
void strm_Brnrd(char* str, uint8_t a, uint8_t b);
void strm_Leet(char* str,uint8_t a, uint8_t b);
void strm_Pig(char* str,uint8_t a, uint8_t b);
void strm_Missing(char* str, uint8_t freq, uint8_t a);
void strm_Reverse(char* str,uint8_t a, uint8_t b);
void strm_Scramble(char* str,uint8_t a, uint8_t b);
void strm_Nada(char* str,uint8_t a, uint8_t b);
void strm_genPush(char* command, char* name, uint8_t number);
void strm_genSet(char* command, char* name, uint8_t value);
void strm_genAct(char* command, char* name, uint8_t number, uint8_t action);
uint8_t strm_Spaces(char* string, uint8_t * space_loc);
uint8_t strm_Length(char* string);

#endif