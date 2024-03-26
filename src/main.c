#include <errno.h>

#include <lexer.h>
#include <parser.h>

/* All C files will include this, where extern_ is replaced with extern.
 * But main.c will remove the extern_, thus, these variables will belongs to main.c
 * */
#define extern_
#include <global_data.h>
#undef extern_

#include <token.h>
#include <ast.h>

static void init() {
    line = 1;
    put_back = '\n';
}

static void usage(char *prog) {
    fprintf(stderr, "Usage: %s <input_file>\n", prog);
    exit(1);
}

int main(int argc, char *argv[]) {

    struct ast_node *n;

    if(argc != 2)
        usage(argv[0]);

    init();

    if((input_file = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
        exit(1);
    }

    scan(&token);
    n = binary_expression();

    printf("%d\n", ast_interpreter(n));

    fclose(input_file);

    return 0;
}