#include "task2.h"
#include "util/delay.h"

#include "shell.h"


static char* t2 = "task2";

void task2() {
        while(1) {
            shell_println(t2);
            _delay_ms(100);
            //TASK_DELAY(520);
            // kill this task
            //task_kill();
    }
}