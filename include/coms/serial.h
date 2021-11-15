#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

#include <stdint.h>
#include <stdio.h>

void USART_Init(uint64_t);

void serial_puts(char*, char*);
void serial_putc(char);
uint8_t serial_getc();
uint32_t serial_gets(char* buf, uint32_t b_size);
uint8_t serial_available(void);

#endif