#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <malloc.h>
#include <fcntl.h>
#include "command.h"

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

bool has_redirect(char ** command, int * output_index)
{
    int i = 0;
    while(command[i] != NULL) {
        if (strcmp(command[i], "->") == 0) {
            *output_index = i+1;
            return true;
        }
        i++;
    }
    return false;
}

void execute_command(char ** command)
{
   if (fork() == 0) {
        int output_index = 0;
        int fd;
        if (has_redirect(command, &output_index)) {
            fd = open(command[output_index], O_CREAT|O_TRUNC|O_WRONLY, 0666);
            if (fd < 0) {
                printf("Error: cannot open file %s\n", command[output_index]);
            } 
            for (int i = output_index-1; i < MAX_INPUT_TOKENS; i++) {
                command[i] = NULL;
            }
            dup2(fd, 1);
        }
        execvp(command[0], command);
        printf("Failed to invoke command\n");
    } else {
        wait(NULL);
    }
}

void free_command(char ** command)
{
    for (int i=0; i<MAX_INPUT_TOKENS; i++) {
        free(command[i]);
    }
    free(command);
}

int main(void)
{
    char input_str[256];
    bool exit_called = false;
    struct commandNode * head;
    head = malloc(sizeof *head);
    head->prev = NULL;
    head->next = NULL;
    struct commandNode * curr = head;

    while (!exit_called) {

        printf("[jsh]-->");
        fgets(input_str, sizeof(input_str), stdin);

        //sanitize input if ending with newline
        char* p;
        if ((p = strchr(input_str, '\n')) != NULL)
            *p = '\0';
        curr->command = strdup(input_str);
        char** command = parse_input(input_str);

        if (!strcmp(command[0], "exit")) {
            exit_called = true;
        } else if (!strcmp(command[0], "history")) {
            printHistory(head);
        } else if (!strcmp(command[0], "!!")) {
            if (curr->prev != NULL) {
                free_command(command);
                command = parse_input(curr->prev->command);
                if (strcmp(command[0], "!!")) {
                    execute_command(command);
                } else {
                    printf("Error: cannot call \"!!\" consecutively\n");
                }
            } else {
                printf("Error: No previous commands to be run\n");
            }
        } else {
            execute_command(command);
        }

        free_command(command);
        if(!exit_called) {
            struct commandNode * next = malloc(sizeof *head);
            curr->next = next;
            next->prev = curr;
            next->next = NULL;
            curr = next;
        }
    }

    freeHistory(head);
    return 0;
}
