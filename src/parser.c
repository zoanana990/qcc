#include <global_data.h>
#include <token.h>
#include <ast.h>
#include <lexer.h>

/* A function to convert the token to arithmetic operator
 */
int arithmetic_operator(int t) {
    switch(t) {
        case T_PLUS:
            return A_ADD;
        case T_MINUS:
            return A_SUBSTRACT;
        case T_STAR:
            return A_MULTIPLY;
        case T_SLASH:
            return A_DIVIDE;
        default:
            fprintf(stderr, "unknown token in arithmetic operator on line %d\n", line);
            exit(1);
    }
}

/* We need a function to check that the next token is an integer literal,
 * and build an AST node to hold the literal value.  
 */
struct ast_node *primary() {
    struct ast_node *n;

    /* for an INT_LITERAL token, make a leaf ast node for it
     * and scan in the next token.
     * Otherwise, a syntax error for any other token type
     */
    // printf("%d\n", token.token);
    switch(token.token) {
        case T_INT_LITERAL:
            n = ast_makeLeaf(A_INTEGER_LITERAL, token.integer_value);
            // printf("%d\n", token.integer_value);
            scan(&token);
            
            return n;
        default:
            fprintf(stderr, "syntax error on line %d\n", line);
            exit(1);
    }
}

/**
 * Current parser:
 * expression := T_INTEGER_LITERAL |
 *               expression arithmetic_operator expression
 */
struct ast_node *binary_expression(void) {
    struct ast_node *left, *right;
    int node_type;

    /* get the integer literal on the left
     * fetch the next token at the same time
     */
    left = primary();

    /* if no token left, return the left node */
    if(token.token == T_EOF)
        return left;

    /* convert the token into a node type */
    node_type = arithmetic_operator(token.token);

    /* get the next token in */
    scan(&token);

    right = binary_expression();

    return ast_makeNode(node_type, 0, left, right);
}