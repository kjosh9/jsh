#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <malloc.h>

#define MAX_INPUT_LEN 256
#define MAX_INPUT_TOKENS 10

char** parse_input(char * input)
{
    char** tokens = malloc(sizeof(char*) * MAX_INPUT_TOKENS);
    char* token = strtok(input, " ");
    int i = 0;
    while(token != NULL && i < MAX_INPUT_TOKENS) {
        tokens[i] = strdup(token);
        token = strtok(NULL, " ");
        i++;
    }
    while(i < MAX_INPUT_TOKENS) {
        tokens[i] = NULL;
        i++;
    }
    return tokens;
}

int main(void)
{
    char input_str[256];
    bool exit_called = false;
    while (!exit_called) {

        printf("[jsh]-->");
        fgets(input_str, sizeof(input_str), stdin);

        //sanitize input if ending with newline
        char* p;
        if ((p = strchr(input_str, '\n')) != NULL)
            *p = '\0';

        char** command = parse_input(input_str);
        
        if (!strcmp(command[0], "exit")) {
            exit_called = true;
        } else if (fork() == 0) {
            execvp(command[0], command);
        } else {
            wait(NULL);
        }
        for (int i=0; i<MAX_INPUT_TOKENS; i++) {
            free(command[i]);
        }
        free(command);
    }
    return 0;
}
