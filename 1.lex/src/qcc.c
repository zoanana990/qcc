#include <qcc.h>
#include <qarr.h>
#include <qlex.h>
#include <stdio.h>
#include <stdlib.h>
#include <color.h>

/**
 * global variable
 */
FILE        *fin = NULL;
char        *filename;
char        *outfile;

/**
 * extern variable
 */
extern int   line_num;

void init() {
    line_num = 1;
    init_lex();
}

int main(int argc, char **argv) {

    /* open the file */
    fin = fopen(argv[1], "rb");
    if(fin == NULL) {
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
    

    /* close the file */
    fclose(fin);

    return 0;
}