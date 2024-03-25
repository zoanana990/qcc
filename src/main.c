#include <errno.h>

#include <lexer.h>

/* All C files will include this, where extern_ is replaced with extern.
 * But main.c will remove the extern_, thus, these variables will belongs to main.c
 * */
#define extern_
#include <global_data.h>
#undef extern_

#include <token.h>

static void init() {
    line = 1;
    put_back = '\n';
}

static void usage(char *prog) {
    fprintf(stderr, "Usage: %s <input_file>\n", prog);
    exit(1);
}

char *token_string[] = {
    "+",
    "-",
    "*",
    "/",
    "integer_literal"
};

static void scan_file() {
    struct token t;

    while(scan(&t)) {
        printf("token %s", token_string[t.token]);
        if(t.token == T_INT_LITERAL)
            printf(", value %d", t.integer_value);
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    if(argc != 2)
        usage(argv[0]);

    init();

    if((input_file = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
    }

    scan_file();

    fclose(input_file);

    return 0;
}