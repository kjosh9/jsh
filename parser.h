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

bool has_substring(char ** command, char * substring, int * op_index)
{
    int i = 0;
    while(command[i] != NULL) {
        if (strcmp(command[i], substring) == 0) {
            *op_index = i;
            return true;
        }
        i++;
    }
    return false;
}
