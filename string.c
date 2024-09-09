#include "string.h"

char ** string_parse(char * str, char * delimiter){
    char ** tokens = malloc(sizeof(char*));
    char * token = strtok(str, delimiter);
    int i = 0;
    
    while(token != NULL) {
        tokens[i++] = token;
        tokens = realloc(tokens, sizeof(char*) * (i+1));
        token = strtok(NULL, delimiter);
    }
    tokens[i] = NULL;

    return tokens;
}