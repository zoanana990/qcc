#include <ast.h>

/* build and return a generic ast node */
struct ast_node *ast_makeNode(int opcode, int integer_value, struct ast_node *left, struct ast_node *right) {
    struct ast_node *n;
    
    n = malloc(sizeof(struct ast_node));
    if(n == NULL) {
        fprintf(stderr, "malloc fail, unable to make a ast node\n");
        exit(1);
    }

    n->opcode = opcode;
    n->integer_value = integer_value;
    n->left = left;
    n->right = right;

    return n;
}

/* make an ast leaf */
struct ast_node *ast_makeLeaf(int opcode, int integer_value) {
    return ast_makeNode(opcode, integer_value, NULL, NULL);
}

/* make a unary ast node, only one child */
struct ast_node *ast_makeUnary(int opcode, int integer_value, struct ast_node *left) {
    return ast_makeNode(opcode, integer_value, left, NULL);
}

int ast_interpreter(struct ast_node *n) {
    int lv, rv;
    if(n->left)
        lv = ast_interpreter(n->left);
    if(n->right)
        rv = ast_interpreter(n->right);

    if(n->opcode == A_INTEGER_LITERAL)
        printf("int %d\n", n->integer_value);
    else
        printf("operator %d, %d, %d\n", lv, n->opcode, rv);

    switch(n->opcode) {
        case A_ADD:
            return lv + rv;
        case A_SUBSTRACT:
            return lv - rv;
        case A_MULTIPLY:
            return lv * rv;
        case A_DIVIDE:
            return lv / rv;
        case A_INTEGER_LITERAL:
            return n->integer_value;
        default:
            fprintf(stderr, "unknown ast operator %d\n", n->opcode);
            exit(1);
    }
}