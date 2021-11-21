#ifndef OS_H_INCLUDED
#define OS_H_INCLUDED

#include "stdint.h"

typedef void (*task_function)();

typedef enum {
    RUNNING,
    READY,
    BLOCKED,
    KILL
} task_state_t;

typedef struct {
    volatile uint8_t has_delay;
    volatile uint32_t delay;
    volatile uint32_t time_started;
} task_delay_t;

typedef struct {
    volatile uint8_t* top_of_stack; // points to thread stack context
    volatile task_state_t task_state;
    volatile task_delay_t delay;
} TCB_t;

void create_task(uint16_t size, task_function task);
void scheduler_start();

void os_timer_init(uint64_t _switch_interval);
void task_yield( ); //__attribute__ ( ( hot, flatten, naked ) );
void task_delay(uint32_t _ticks); //__attribute__ ( ( hot, flatten, naked ) );
void task_kill();

#define TASK_DELAY(x) task_delay(x) ; \
    task_yield();

#endif