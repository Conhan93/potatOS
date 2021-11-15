#include <shell.h>

#include <string.h>
#include <stdlib.h>

/*
    Command pattern used to define a list of commands for shell to use.
    define elsewhere in a sourcefile.
*/
extern const ShellCommands shell_commands[];
extern const uint8_t nr_commands;

#define MAX_BUFFER_SIZE 256
static char buffer[MAX_BUFFER_SIZE];
static uint16_t buffer_size = 0;

#define MAX_ARGS_NR 20
#define MAX_ARGS_LEN 44
#define PROMPT ">>"

static char** argv;
static void (*send_char)(char);


static void send_prompt();
static void process_input();
static void echo(char ch);
static void echo_string(const char* str);
static void reset_buffer();

// input function for shell
void shell_recieve_char(char ch) {

    if(buffer_size >= MAX_BUFFER_SIZE) {
        return;
    }
        
    echo(ch);
    
    if(ch == '\b') { // if backspace
        buffer[buffer_size--] = '\0';
        return;
    }

    if(ch != '\r' || ch != '\n') // add char to input
        buffer[buffer_size++] = ch;
    
    if(ch == '\r' || ch == '\n') {
        process_input();
    }
    
}
/*
    Sets the function used by shell to communicate
    and initializes shell
*/
void shell_set_send_char(void (*func)(char)) {
    send_char = func;
    memset(buffer, 0, MAX_BUFFER_SIZE);
    echo_string("\n" PROMPT);
}
static void echo(char ch) {

    if ('\n' == ch || '\r' == ch) { // End of line
        send_char('\r');
        send_char('\n');
    } else if ('\b' == ch) { // backspace
        send_char('\b');
        send_char(' ');
        send_char('\b');
    } else {
        send_char(ch);
    }
}
static void echo_string(const char* str) {
    const char* _str = str;
    while(*_str != '\0')
        echo(*_str++);
}
static void send_prompt() {
    echo_string(PROMPT);
}
// prints all command help strings
void shell_help_cmd(uint8_t argc, char** argv) {
    uint8_t _nr_cmds = nr_commands;
    while(_nr_cmds--) {
        echo('\n');
        echo_string(shell_commands[_nr_cmds].cmd);
        echo('\t');
        echo_string(shell_commands[_nr_cmds].help);
    }
    echo('\n');
}
/*
    Loops through list of commands, comparing input string
    to command identifier/cmd string looking for a match.

    Returns command if match found else NULL
*/
static const ShellCommands* match_command(char* word) {
    for(uint8_t index = 0 ; index < nr_commands ; index++) 
        if(!strcmp(word, shell_commands[index].cmd)) 
            return &shell_commands[index];
        
    return NULL;
}
/*
    Tokenizes buffer.

    Splits buffer on space, carriage return.
    stores results in argv and number of results in argc

    modifies buffer!
*/
static void tokenize(uint8_t* argc, char** argv) {
    char *next_arg = NULL;

    for (size_t i = 0; i < buffer_size && *argc < MAX_ARGS_NR; ++i) {
        char *const c = &buffer[i];
        if (*c == ' ' || *c == '\r' || i == buffer_size - 1) {
            *c = '\0';
            if (next_arg) {
                argv[(*argc)++] = next_arg;
                next_arg = NULL;
            }
        } else if (!next_arg) {
            next_arg = c;
        }
    }
}
/*
    Processes string stored in buffer.

    splits string and looks for a matching cmd,
    if found runs cmd function else prints error
    message.

    clears buffer and prints prompt

*/
static void process_input() {
    char *argv[MAX_ARGS_LEN] = {0};
    uint8_t argc = 0;
    
    tokenize(&argc, argv);

    if(buffer_size == MAX_BUFFER_SIZE)
        echo('\n');

    if(argc) {

        const ShellCommands* cmd = match_command(argv[0]);

        if(cmd) // execute cmd
            cmd->cmd_func(argc, argv);
        else { // help message
            echo_string("\nInvalid command : ");
            echo_string(argv[0]);
            echo('\n');
            echo_string("Type 'help' to list all commands\n");
        }

    }

    reset_buffer();
    echo('\n');
    send_prompt();
}
static void reset_buffer() {
    buffer_size = 0;
    memset(buffer, 0, MAX_BUFFER_SIZE);
}

void shell_println(char* ln) {
    echo_string(ln);
    echo('\n');
}