#include <lexer.h>
#include <vector.h>
#include <parser.h>
#include <qstack.h>

#include <stdio.h>

/**
 * global variable
 */
FILE *fin = NULL;
char *filename;

/**
 * extern variable
 */
extern int line_cnt;
extern qvector_t token_table;
extern token_t *ptr_token_hashtable[MAX_KEY];
extern qstack_t global_symbol_stack, local_symbol_stack;
extern type_t type_int;

void deinit() {
    /* TODO: free the memory we used */
}

void init() {
    line_cnt = 1;
    init_lex();

    stack_init(&local_symbol_stack, 8);
    stack_init(&global_symbol_stack, 8);

    type_int.t = TYPE_INT;
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