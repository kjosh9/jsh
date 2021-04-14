#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <malloc.h>
#include <fcntl.h>
#include "command.h"
#include "parser.h"

#define MAX_INPUT_LEN 256
#define MAX_INPUT_TOKENS 10
#define MAX_PATH 256

void execute_command(char ** command)
{
    int pid = fork();
    // child process
    if (pid == 0) {
        int op_index = 0;
        int fd;
        int pipe_fd[2];
        if (has_substring(command, "|", &op_index)) {
            if (pipe(pipe_fd)==-1) {
                perror("Error: pipe creation failed");
            }

            //split pre pipe from post pipe
            char** command_2 = &command[op_index+1];
            free(command[op_index]);
            command[op_index] = NULL;

            int pid_2 = fork();
            // childs child process
            if (pid_2 == 0) {
                close(pipe_fd[1]);
                dup2(pipe_fd[0], 0);
                execvp(command[op_index+1], command_2);
            // original child process
            } else if (pid_2 > 0) {
                close(pipe_fd[0]);
                dup2(pipe_fd[1], 1);
                execvp(command[0], command);
            } else {
                perror("Error: failed to create another fork");
            }
        } else if (has_substring(command, "->", &op_index)) {
            fd = open(command[op_index+1], O_CREAT|O_TRUNC|O_WRONLY, 0666);
            if (fd < 0) {
                printf("Error: cannot open file %s\n", command[op_index+1]);
            } 
            for (int i = op_index; i < MAX_INPUT_TOKENS; i++) {
                command[i] = NULL;
            }
            dup2(fd, 1);
        }

        execvp(command[0], command);
        perror("Error: Failed to invoke command\n");

    // parent process
    } else if (pid > 0) {
        wait(NULL);
    } else {
        perror("Error: Fork failed");
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

        char current_dir[MAX_PATH];
        getcwd(current_dir, MAX_PATH);
        printf("(%s)->jsh: ", current_dir);
        fgets(input_str, sizeof(input_str), stdin);

        //sanitize input if ending with newline
        char* p;
        if ((p = strchr(input_str, '\n')) != NULL)
            *p = '\0';
        curr->command = strdup(input_str);
        char** command = parse_input(input_str);

        if (!strcmp(command[0], "exit")) {
            exit_called = true;
        } else if (!strcmp(command[0], "help")) {
            printf("commands:\n");
            printf("history                    | prints history to standard output\n");
            printf("<command> -> <destination> | redirect output\n");
            printf("<command> | <command>      | redirect output of one command to another\n");
            printf("exit                       | exits terminal\n");
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
                perror("Error: No previous commands to be run\n");
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
