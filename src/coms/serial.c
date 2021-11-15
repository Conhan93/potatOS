#include "coms/serial.h"
#include "avr/io.h"




void USART_Init(uint64_t baud)  {

    uint64_t MYUBRR = ((16000000UL/(16UL*baud))-1);
    // Set baud rate
    UBRR0H = (uint8_t)(MYUBRR >> 8);
    UBRR0L = (uint8_t) MYUBRR;
    
    // Enable receiver and transmitter
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    /*
    // Set frame format: 8data
    UCSR0C |= (1<<UCSZ00) | (1 << UCSZ01);
    // No Parity
    UCSR0C &= ~(1 << UPM00);
    UCSR0C &= ~(1 << UPM01);
    // 1 stop bit
    UCSR0C &= ~(1 << USBS0);
    */

   UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}


static void USART_Transmit(uint8_t data) {

    // Wait for empty transmit buffer
    while (!(UCSR0A & (1<<UDRE0)))  ;
    // Put data into buffer, sends the data
    UDR0 = data;
}
static uint8_t USART_Available(void) {
    return UCSR0A & (1<<RXC0);
}
/* Get and return received data from buffer */
static uint8_t USART_Receive(void) {
    return UDR0;
}
uint8_t serial_available(void) {
    return USART_Available();
}
// write string onto serial
void serial_puts(char* string, char* delim) {
    uint8_t end_line = delim == NULL ? '\0' : (*delim);

    while(*string != end_line)
        USART_Transmit(*string++);
}
void serial_putc(char c) {
    USART_Transmit(c);
}
uint8_t serial_getc() {
    return USART_Receive();
}
// read string from serial into buf, returns length of string
uint32_t serial_gets(char* buf, uint32_t b_size) {
    char *temp = buf;
    char input;

    *temp = '\000';
    
    while ((input = serial_getc()) != '\n') {
        *temp = input;
        serial_putc(input);

        if ((temp - buf) < b_size) 
            temp++;
        
        // close string
        *temp = '\000';
    }

    return temp - buf;
}