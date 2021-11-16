#include "avr/io.h"
#include "avr/interrupt.h"
#include "util/delay.h"

#include "coms/serial.h"
#include <shell.h>

#include "os/os.h"

#include "string.h"


// absolute
#define abs(x) (x < 0 ? (-x) : x) 




void system_init();

void task1a() {
    while(1) {
        serial_puts("task1a\n\r", NULL);
        TASK_DELAY(650);
    }
}

void task1() {
    uint8_t a = 0;
    // spawn new task from within thread
    //create_task(200, task1a);

    while(1) {
            if(serial_available()) // read USART/serial and pass char to shell
                shell_recieve_char(serial_getc());

    }
}
void task2() {
        while(1) {
            shell_println("task2");
            TASK_DELAY(520);
            // kill this task
            task_kill();
    }
}
void task3() {
        while(1) {
            shell_println("task3");
            TASK_DELAY(510);
    }
}
void task4() {
        while(1) {
            shell_println("task4");
            TASK_DELAY(520);

    }
}


int main (void) {

    system_init();

    create_task(100, task2);
    create_task(300, task1);
    create_task(100, task3);
    create_task(100, task4);
    
    
    sei();
    scheduler_start();

    // loop - not supposed to get here
    while(1) {
        
        if(serial_available()) // read USART/serial and pass char to shell
            shell_recieve_char(serial_getc());

        
    }

    return 0;
}


void system_init() {
    
    USART_Init(9600);
    // init os timer with context switching at every 10 ticks
    os_timer_init(10);

    // give shell access USART/serial and init shell
    shell_set_send_char(serial_putc);
}



// list of shell commands
const ShellCommands shell_commands[] = {
    {"help",shell_help_cmd, "List all commands." }
};
const uint8_t nr_commands = sizeof(shell_commands)/sizeof(shell_commands[0]);

