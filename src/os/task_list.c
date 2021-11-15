#include "os/os.h"



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
    
    if(!perform_switch)
        return;

    // save any changes to tcb
    *tcb_list[active] = *current_tcb;

    // find next ready task
    do {
        active = contain_rollover(++active, nr_threads);
    }while(tcb_list[active]->task_state != READY);
    
    // point to next ready task
    current_tcb = tcb_list[active];

    // reset tick switch
    perform_switch = 0;
}
void switch_task_from_yield(void) {
    // save any changes to tcb
    *tcb_list[active] = *current_tcb;

    // find next ready task
    do {
        active = contain_rollover(++active, nr_threads);
    }while(tcb_list[active]->task_state != READY);
    
    // point to next ready task
    current_tcb = tcb_list[active];

}


TCB_t * get_tasks() { return tcb_list; }
uint8_t get_nr_tasks() {return nr_threads; }

// contain with roll over in both directions
static inline uint16_t contain_rollover(int16_t x, uint32_t sz) {
    return ((x >= sz) ? 0 : (x < 0) ? (sz - 1) : x);
}