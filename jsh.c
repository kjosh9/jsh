#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>


char * parse_input(char * input)
{
    return strtok(input, " "); 
}

int main(void)
{
    char input_str[256];
    while (true) {
        printf("[jsh]-->");
        fgets(input_str, sizeof(input_str), stdin);
        char * command = parse_input(input_str);
        
        if (fork() == 0) {
            command = "ls";
            char *args[] = {command, NULL};
            execvp(command, args);
        } else {
            wait(NULL);
        }
    }
    return 0;
}
