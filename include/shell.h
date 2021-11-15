#ifndef SHELL_H_INCLUDED
#define SHELL_H_INCLUDED

#include <stdint.h>

typedef struct {
    char* cmd;
    void (*cmd_func)(uint8_t, char**);
    char* help;

} ShellCommands;


void shell_recieve_char(char ch);
void shell_set_send_char(void (*func)(char));
void shell_println(char* ln);

void shell_help_cmd(uint8_t argc, char** argv);

#endif