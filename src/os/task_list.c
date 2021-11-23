#include "os/os.h"

#include "stdlib.h"

#include "shell.h" // DEBUG

extern volatile TCB_t * current_tcb;

#define MAX_THREADS 10

static volatile TCB_t* tcb_list[MAX_THREADS];
static volatile uint8_t nr_threads = 0;

#if SCHEDULER_BEHAVIOR == ROUND_ROBIN

static volatile uint8_t active = 0;
static inline uint16_t contain_rollover(int16_t x, uint32_t sz);

#elif SCHEDULER_BEHAVIOR == PRIORITY_SCHEDULING

static int cmp_tcb_prio(const void * a, const void* b);

#endif

#if SCHEDULER_BEHAVIOR == PRIORITY_SCHEDULING
void add_task(TCB_t* new_tcb) {

    if(nr_threads >= MAX_THREADS)
        return;
    
    // add task
    tcb_list[nr_threads++] = new_tcb;
    
    // sort by priority
    qsort(tcb_list, nr_threads, sizeof(TCB_t*), cmp_tcb_prio);
    
    
}
#elif SCHEDULER_BEHAVIOR == ROUND_ROBIN
void add_task(TCB_t* new_tcb) {

    if(nr_threads >= MAX_THREADS)
        return;

    // task added latest starts first
    active = nr_threads;
    // copy control block into list
    tcb_list[nr_threads++] = new_tcb;
}
#endif
void next_task(void) {

    #if SCHEDULER_BEHAVIOR == PRIORITY_SCHEDULING

    for(uint8_t index = 0 ; index < nr_threads ; index++)
        if(tcb_list[index]->task_state == READY || tcb_list[index]->task_state == RUNNING) {
            current_tcb = tcb_list[index];
            return;
        }

    #elif SCHEDULER_BEHAVIOR == ROUND_ROBIN

    do {
        active = contain_rollover(++active, nr_threads);
    }while(tcb_list[active]->task_state != READY);
    
    // point to next ready task
    current_tcb = tcb_list[active];

    #endif
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

#if SCHEDULER_BEHAVIOR == ROUND_ROBIN
// contain with roll over in both directions
static inline uint16_t contain_rollover(int16_t x, uint32_t sz) {
    return ((x >= sz) ? 0 : (x < 0) ? (sz - 1) : x);
}
#elif SCHEDULER_BEHAVIOR == PRIORITY_SCHEDULING
static int cmp_tcb_prio(const void * a, const void* b) {
    uint8_t prio_a = ((TCB_t *)a)->priority;
    uint8_t prio_b = ((TCB_t *)b)->priority;

    return prio_a == prio_b ? 0 : prio_a > prio_b ? 1 : -1;
}
#endif