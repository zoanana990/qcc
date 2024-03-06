#include <qlex.h>
#include <qarr.h>
#include <parser.h>

#include <stdio.h>
#include <stdlib.h>

/**
 * global variable
 */
FILE *fin = NULL;
char *filename;

/**
 * extern variable
 */
extern int line_cnt;
extern qarr_t token_table;
extern token_t *ptr_token_hashtable[MAX_KEY];

void deinit() {
    /* TODO: free the memory we used */
    for(int i = 0; i < token_table.count; i++)
        free(token_table.data[i]);

    free(token_table.data);
}

void init() {
    line_cnt = 1;
    init_lex();
}

int main(int argc, char **argv) {

    /* open the file */
    fin = fopen(argv[1], "rb");
    if (fin == NULL) {
        printf("Missing source file!\n");
        printf("Usage: ./qcc <input.c>\n");
        return 1;
    }

    filename = argv[1];

    printf("\n\n[NOTICE] this project do not do the semantic analyze!!!\n\n");

    /* init the lexer */
    init();

    /* get the character */
    getch();
    get_token();

    /* translation unit */
    translation_unit();

    /* free the memory we used */
    deinit();

    /* close the file */
    fclose(fin);

    return 0;
}