#include "os/os_timer.h"
/*
#include "os/os.h"

#include "avr/io.h"
#include "avr/interrupt.h"

void os_timer_init() {
    // CTC Mode
    TCCR0A |= (1 << WGM01);

    // 1024 prescaler
    TCCR0B |= (1 << CS02) | (1 << CS00);

    // enable interrupt on OCR0A compare match
    TIMSK0 |= (1 << OCIE0A);


}

// timer0 interrupt on match with OCR0A
ISR(TIMER0_COMPA_vect) {
    
}
*/