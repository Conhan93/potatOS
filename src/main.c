#include "avr/io.h"
#include "avr/interrupt.h"
#include "util/delay.h"

#include "coms/serial.h"
#include <shell.h>

#include "os/os.h"
#include "os/os_timer.h"

#include "string.h"

#include "task2.h"


// absolute
#define abs(x) (x < 0 ? (-x) : x) 

char* t1 = "task1", *t3 = "task3", *t4 = "task4";




void system_init();


void task1() {
    //cli();
    //create_task(100, task1a);
    //sei();
    while(1) {

            shell_println(t1);
            task_delay(300);
            //task_yield();

    }
}

void task3() {
        while(1) {
            shell_println(t3);
            task_delay(450);
            //task_yield();

    }
}
void task4() {
        while(1) {
            shell_println(t4);
            task_delay(999);
            //task_yield();
    }
}


int main (void) {

    

    //create_task(100, task2);
    create_task(300, task1);
    create_task(100, task3);
    create_task(100, task4);
    
    system_init();
    sei();
    scheduler_start();

    // loop - not supposed to get here
    while(1) {
    }

    return 0;
}


void system_init() {
    
    USART_Init(9600);
    // init os timer with context switching at every 10 ticks
    os_timer_init(3);

    // give shell access USART/serial and init shell
    shell_set_send_char(serial_putc);
}



// list of shell commands
const ShellCommands shell_commands[] = {
    {"help",shell_help_cmd, "List all commands." }
};
const uint8_t nr_commands = sizeof(shell_commands)/sizeof(shell_commands[0]);

