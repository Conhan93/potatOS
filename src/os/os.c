#include "os/os.h"
#include "os/task_list.h"
#include "os/os_config.h"

#include "stdlib.h"

#include "avr/io.h"
#include "avr/interrupt.h"
#include "util/atomic.h"


// current thread control block
TCB_t volatile *current_tcb;


///// OS

#define SAVE_CONTEXT()                                                              \
        __asm__ __volatile__ (  "push   r0                             \n\t"   \
                                "in     r0, __SREG__                   \n\t"   \
                                "cli                                            \n\t"   \
                                "push   r0                             \n\t"   \
                                "push   r1                            \n\t"   \
                                "clr    r1                            \n\t"   \
                                "push   r2                                      \n\t"   \
                                "push   r3                                      \n\t"   \
                                "push   r4                                      \n\t"   \
                                "push   r5                                      \n\t"   \
                                "push   r6                                      \n\t"   \
                                "push   r7                                      \n\t"   \
                                "push   r8                                      \n\t"   \
                                "push   r9                                      \n\t"   \
                                "push   r10                                     \n\t"   \
                                "push   r11                                     \n\t"   \
                                "push   r12                                     \n\t"   \
                                "push   r13                                     \n\t"   \
                                "push   r14                                     \n\t"   \
                                "push   r15                                     \n\t"   \
                                "push   r16                                     \n\t"   \
                                "push   r17                                     \n\t"   \
                                "push   r18                                     \n\t"   \
                                "push   r19                                     \n\t"   \
                                "push   r20                                     \n\t"   \
                                "push   r21                                     \n\t"   \
                                "push   r22                                     \n\t"   \
                                "push   r23                                     \n\t"   \
                                "push   r24                                     \n\t"   \
                                "push   r25                                     \n\t"   \
                                "push   r26                                     \n\t"   \
                                "push   r27                                     \n\t"   \
                                "push   r28                                     \n\t"   \
                                "push   r29                                     \n\t"   \
                                "push   r30                                     \n\t"   \
                                "push   r31                                     \n\t"   \
                                "lds    r26, current_tcb                       \n\t"   \
                                "lds    r27, current_tcb + 1                   \n\t"   \
                                "in     r0, __SP_L__                   \n\t"   \
                                "st     x+, r0                         \n\t"   \
                                "in     r0, __SP_H__                   \n\t"   \
                                "st     x+, r0                         \n\t"   \
                             );

#define RESTORE_CONTEXT()                                                           \
        __asm__ __volatile__ (  "lds    r26, current_tcb                       \n\t"   \
                                "lds    r27, current_tcb + 1                   \n\t"   \
                                "ld     r28, x+                                 \n\t"   \
                                "out    __SP_L__, r28                           \n\t"   \
                                "ld     r29, x+                                 \n\t"   \
                                "out    __SP_H__, r29                           \n\t"   \
                                "pop    r31                                     \n\t"   \
                                "pop    r30                                     \n\t"   \
                                "pop    r29                                     \n\t"   \
                                "pop    r28                                     \n\t"   \
                                "pop    r27                                     \n\t"   \
                                "pop    r26                                     \n\t"   \
                                "pop    r25                                     \n\t"   \
                                "pop    r24                                     \n\t"   \
                                "pop    r23                                     \n\t"   \
                                "pop    r22                                     \n\t"   \
                                "pop    r21                                     \n\t"   \
                                "pop    r20                                     \n\t"   \
                                "pop    r19                                     \n\t"   \
                                "pop    r18                                     \n\t"   \
                                "pop    r17                                     \n\t"   \
                                "pop    r16                                     \n\t"   \
                                "pop    r15                                     \n\t"   \
                                "pop    r14                                     \n\t"   \
                                "pop    r13                                     \n\t"   \
                                "pop    r12                                     \n\t"   \
                                "pop    r11                                     \n\t"   \
                                "pop    r10                                     \n\t"   \
                                "pop    r9                                      \n\t"   \
                                "pop    r8                                      \n\t"   \
                                "pop    r7                                      \n\t"   \
                                "pop    r6                                      \n\t"   \
                                "pop    r5                                      \n\t"   \
                                "pop    r4                                      \n\t"   \
                                "pop    r3                                      \n\t"   \
                                "pop    r2                                      \n\t"   \
                                "pop    r1                            \n\t"   \
                                "pop    r0                             \n\t"   \
                                "out    __SREG__, r0                   \n\t"   \
                                "pop    r0                             \n\t"   \
                             );


void task_housekeeping();



static uint8_t* init_task_stack(uint8_t* stack_top, task_function func) {
    uint16_t func_addr = (uint16_t)func;

    // place function code on stack
    *stack_top-- = (uint8_t) (func_addr & 0x00ff);
    func_addr >>= 8;
    *stack_top-- = (uint8_t) (func_addr & 0x00ff);

    // R0
    *stack_top-- = (uint8_t)0x00;
    *stack_top-- = (uint8_t)0x80;

    // set R1 to 0
    *stack_top =  (uint8_t)0x00;

    // skip the rest of the registers
    for(uint8_t i = 0 ; i < 31 ; i++)
        stack_top--;


    return stack_top;

}

#if SCHEDULER_BEHAVIOR == PRIORITY_SCHEDULING
/**
 * @brief Create a task object
 * 
 * @param size memory in bytes allocated for task stack
 * @param task function to execute task
 * @param prio the priority of the task
 */
void create_task(uint16_t size, task_function task, task_priority_t prio) {
    
    // allocate task stack with tcb at bottom
    uint8_t* task_stack_adress = malloc(size);
    TCB_t* new_tcb = malloc(sizeof(TCB_t));

    // initiate task stack and couple with tcb
    new_tcb->top_of_stack = init_task_stack(task_stack_adress + size, task);

    // start task in ready state
    new_tcb->task_state = READY;

    // assign priority
    new_tcb->priority = prio;

    // add task to list
    add_task(new_tcb);

}
#elif SCHEDULER_BEHAVIOR == ROUND_ROBIN
/**
 * @brief Create a task object
 * 
 * @param size memory in bytes allocated for task stack
 * @param task function to execute task
 */
void create_task(uint16_t size, task_function task) {
    
    // allocate task stack with tcb at bottom
    uint8_t* task_stack_adress = malloc(size);
    TCB_t* new_tcb = malloc(sizeof(TCB_t));

    // initiate task stack and couple with tcb
    new_tcb->top_of_stack = init_task_stack(task_stack_adress + size, task);

    // start task in ready state
    new_tcb->task_state = READY;

    // add task to list
    add_task(new_tcb);

}
#endif
static void enable_context_switch() {
    // enable software interrupt for context switch
    DDRD |= (1 << EXTERNAL_INTERRUPT_PIN);
    PORTD &= ~(1 << EXTERNAL_INTERRUPT_PIN);

    // trigger interrupt on rising edge
    EICRA |= EXTERNAL_ICS_BITS;

    // enable interrupt
    EIMSK |= EXTERNAL_INTERRUPT_ENABLE;
}
// starts executing the scheduler
void scheduler_start() {
    // start housekeeping task
    #if SCHEDULER_BEHAVIOR == PRIORITY_SCHEDULING
    create_task(100,task_housekeeping, 255);
    #elif SCHEDULER_BEHAVIOR == ROUND_ROBIN
    create_task(100,task_housekeeping);
    #endif

    // set current tcb to first task
    next_task();

    enable_context_switch();
    // restore context of first thread, current tcb should be pointing at it already
    RESTORE_CONTEXT();

    // generate return call, to switch to first thread execution
    __asm__ __volatile__ ( "ret" );
}

static void switch_task() {
    next_task();
}




/**
 * @brief Sets a task delay and then blocks the task
 *  from executing for the duration of the delay.
 * 
 * Delay checked and cleared by housekeeping task.
 * 
 * @param _ticks the amount of timer ticks to sleep.
 */
void task_delay(uint32_t _ticks) {

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        current_tcb->delay = _ticks;
        current_tcb->task_state = BLOCKED;
    }

    TRIGGER_CONTEXT_CHANGE;
}
/**
 * @brief Sets the state of the current task to KILL
 * ,marking it for deletion by housekeeping task.
 * then yields to the next task.
 * 
 */
void task_kill() {
    current_tcb->task_state = KILL;
    TRIGGER_CONTEXT_CHANGE;
}

//////////////////// Housekeeping task

void kill_tasks() {
    TCB_t** tasks = get_tasks();
    uint8_t nr_tasks = get_nr_tasks();

    for(uint8_t index = 0 ; index < nr_tasks ; index++) {
        if(tasks[index]->task_state == KILL ) {
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                remove_task(tasks[index]);
            }
        }
    }
}

#if IDLE_SLEEP
#include "avr/sleep.h"
#endif

void task_housekeeping() {
    while(1) {
        // check task states and terminate any threads set to KILL state
        kill_tasks();    

        #if IDLE_SLEEP // sleep between ticks until context switch

        sleep_enable();

        #else         // context switch so housekeeping task won't hog the CPU if not using timer interrupts

        TRIGGER_CONTEXT_CHANGE;

        #endif
    }
}


///////////// Context switching
/** 
 * @brief called from inside a task
 *  yields the cpu to the next task by
 *  triggering a context switch
 */
void task_yield( void )
{
    TRIGGER_CONTEXT_CHANGE;
}



/**
 * interrupt used to perform a context switch
 */
ISR(CONTEXT_SWITCH_VECTOR, ISR_NAKED) {
    SAVE_CONTEXT();
    switch_task();
    RESET_CONTEXT_CHANGE_TRIGGER;
    RESTORE_CONTEXT();

    __asm__ __volatile__ ( "reti" );
}


