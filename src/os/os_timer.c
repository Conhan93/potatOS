#include "os/os_timer.h"
#include "os/os_config.h"
#include "os/os.h"
#include "os/task_list.h"

#include "avr/interrupt.h"

static volatile uint64_t switch_interval = 0;
static volatile uint64_t millis_counter = 1;

void os_timer_init(uint64_t _switch_interval) {
    // interval to tick at.
    switch_interval = _switch_interval;

    // CTC Mode
    TIMER_CTC_REG |= (1 << WGM01);

    // 64 prescaler
    TIMER_PRESCALER_REG |= (1 << CS01) | (1 << CS00);

    // enable interrupt on output compare match
    TIMER_INTERRUPT_REG |= (1 << TIMER_INTERRUPT_ENABLE);

    // 1000 Hz on 64 prescaler
    TIMER_OUTPUT_COMPA = 249;
}

/**
 * @brief decrements task delay values and change
 * task states from BLOCKED to READY when they reach 0.
 * 
 */
inline static void timer_decrement_delay_ticks() {
    TCB_t** tasks = get_tasks();
    uint8_t nr_tasks = get_nr_tasks();
    TCB_t** end = tasks + nr_tasks;

    while(tasks != end)
        if((*tasks)->delay)
            (*(tasks++))->delay--;
        else
            (*(tasks++))->task_state = READY;

}
/**
 * @brief updates the timer counter and checks
 * if a context switch is to take place
 */
static void timer_tick() {
    millis_counter++;
    timer_decrement_delay_ticks();
    if(!(millis_counter % switch_interval)) 
        TRIGGER_CONTEXT_CHANGE;
        
}
uint64_t os_get_timer_count() {return millis_counter;}

#include "shell.h"

// timer0 interrupt on match with OCR0A
ISR(TIMER_INTERRUPT_VECTOR) {
    
    millis_counter++;
    timer_decrement_delay_ticks();
    if(!(millis_counter % switch_interval)) {
        //shell_println("t");
        TRIGGER_CONTEXT_CHANGE;
    }
        

}