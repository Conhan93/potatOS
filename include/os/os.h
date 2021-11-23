#ifndef OS_H_INCLUDED
#define OS_H_INCLUDED
#include "os/os_config.h"

#include "stdint.h"

typedef void (*task_function)();

typedef enum {
    RUNNING,
    READY,
    BLOCKED,
    KILL
} task_state_t;

typedef uint32_t task_delay_t;
typedef uint8_t task_priority_t;

typedef struct {
    volatile uint8_t* top_of_stack; // points to thread stack context
    volatile task_state_t task_state;
    volatile task_delay_t delay;
    #if SCHEDULER_BEHAVIOR == PRIORITY_SCHEDULING
        volatile task_priority_t priority;
    #endif
} TCB_t;

#if SCHEDULER_BEHAVIOR == ROUND_ROBIN
void create_task(uint16_t size, task_function task);
#elif SCHEDULER_BEHAVIOR == PRIORITY_SCHEDULING
void create_task(uint16_t size, task_function task, task_priority_t prio);
#endif

void scheduler_start();

//void os_timer_init(uint64_t _switch_interval);
void task_yield();
void task_delay(uint32_t _ticks); 
void task_kill();


#endif