#include <qcc.h>
#include <qarr.h>
#include <stdio.h>

#define MAX_KEY                  (1024)

token_t     token_hashtable[MAX_KEY];
qarr_t      token_table;


int elf_hash(char *key) {
    int h=0, g;
    while(*key) {
        h = (h << 4) + *key++;
        g = h & 0xf0000000;
        if(g)
            h ^= g >> 24;
        h &= ~g;
    }

    return h % MAX_KEY;
}

void init_lex() {
    token_t *tp;
    static token_t keywords[] = {
        {TOKEN_PLUS, NULL, "+", NULL, NULL},
        {TOKEN_MINUS, NULL, "+", NULL, NULL},
        {TOKEN_PLUS, NULL, "+", NULL, NULL},
        {TOKEN_PLUS, NULL, "+", NULL, NULL},
        {TOKEN_PLUS, NULL, "+", NULL, NULL},
        {TOKEN_PLUS, NULL, "+", NULL, NULL},
        {TOKEN_PLUS, NULL, "+", NULL, NULL},
        {TOKEN_PLUS, NULL, "+", NULL, NULL},
        {TOKEN_PLUS, NULL, "+", NULL, NULL},
        {TOKEN_PLUS, NULL, "+", NULL, NULL},
        {TOKEN_PLUS, NULL, "+", NULL, NULL},
        {TOKEN_PLUS, NULL, "+", NULL, NULL},
        {TOKEN_PLUS, NULL, "+", NULL, NULL},
        {TOKEN_PLUS, NULL, "+", NULL, NULL},
        {TOKEN_PLUS, NULL, "+", NULL, NULL},
        {TOKEN_PLUS, NULL, "+", NULL, NULL},

    };
}

int main(int argc, char **argv) {
    init_lex();
    return 0;
}