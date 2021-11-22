#include "os/os.h"

#include "shell.h" // DEBUG

extern volatile TCB_t * current_tcb;

#define MAX_THREADS 10

static volatile TCB_t* tcb_list[MAX_THREADS];
static volatile uint8_t nr_threads = 0;
static volatile uint8_t active = 0;

volatile uint8_t perform_switch = 0;

static inline uint16_t contain_rollover(int16_t x, uint32_t sz);

void add_task(TCB_t* new_tcb) {
    // task added latest starts first
    active = nr_threads;
    // copy control block into list
    tcb_list[nr_threads++] = new_tcb;
}

void switch_task(void) {
    // find next ready task
    do {
        active = contain_rollover(++active, nr_threads);
    }while(tcb_list[active]->task_state != READY);
    
    // point to next ready task
    current_tcb = tcb_list[active];

}

void remove_task(TCB_t* _tcb) {

    for(uint8_t i = 0 ; i < nr_threads ; i++ )
        // if tcb found
        if(tcb_list[i]->top_of_stack == _tcb->top_of_stack) {

            // free memory
            free(_tcb->top_of_stack);
            free(_tcb);

            // close holes in array
            for( ; i < nr_threads - 1 ; i++)
		        tcb_list[i] = tcb_list[i+1];
            
            nr_threads--;

            return;
        }
}


TCB_t * get_tasks() { return tcb_list; }
uint8_t get_nr_tasks() {return nr_threads; }

// contain with roll over in both directions
static inline uint16_t contain_rollover(int16_t x, uint32_t sz) {
    return ((x >= sz) ? 0 : (x < 0) ? (sz - 1) : x);
}