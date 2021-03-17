
struct commandNode {
    char* command;
    struct commandNode* prev;
    struct commandNode* next;
};

void freeHistory(struct commandNode * head) {
    while(head != NULL) {
        struct commandNode * temp = head->next;
        free(head->command);
        free(head);
        head = temp;
    }
}

void printHistory(struct commandNode * head) {
    while(head != NULL) {
        printf("%s\n", head->command);
        head = head->next;
    }
}
