#include <stdio.h>
#include <stdlib.h>
#include "command.h"

void freeHistory(struct commandNode * head) {
    while(head != NULL) {
        struct commandNode * temp = head->next;
        free(head->command);
        free(head);
        head = temp;
    }
}

void printHistory(struct commandNode * head) {
    int command_count = 1;
    while(head != NULL) {
        printf("%d: %s\n", command_count, head->command);
        head = head->next;
        command_count++;
    }
}
