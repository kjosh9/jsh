
struct commandNode {
    char* command;
    struct commandNode* prev;
    struct commandNode* next;
};

void freeHistory(struct commandNode * head);

void printHistory(struct commandNode * head);
