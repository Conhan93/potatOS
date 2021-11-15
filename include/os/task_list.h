#ifndef TASK_LIST_H_INCLUDED
#define TASK_LIST_H_INCLUDED

#include "os/os.h"
#include "stdint.h"

void add_task(TCB_t* new_tcb);
void switch_task(void);
void switch_task_from_yield(void);

#endif