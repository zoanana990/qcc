#include <qlex.h>
#include <stdio.h>

/**
 * global variable
 */
FILE *fin = NULL;

/**
 * extern variable
 */
extern int line_cnt;

void deinit() {
    /* TODO: free the mememory we used */
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

    printf("\n\n[NOTICE] this project do not do the semantic analyze!!!\n\n");

    /* init the lexer */
    init();

    /* syntax shader */
    lexical_coloring();

    /* free the memory we used */
    deinit();

    /* close the file */
    fclose(fin);

    return 0;
}