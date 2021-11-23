#ifndef TASK_LIST_H_INCLUDED
#define TASK_LIST_H_INCLUDED

#include "os/os.h"
#include "stdint.h"

void add_task(TCB_t* new_tcb);
void remove_task(TCB_t* _tcb);

void next_task(void);

TCB_t ** get_tasks();
uint8_t get_nr_tasks();


#endif