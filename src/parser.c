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
    switch(token.token) {
        case T_INT_LITERAL:
            n = ast_makeLeaf(A_INTEGER_LITERAL, token.integer_value);
            
            /* this scan token may meet T_EOF */
            scan(&token);     
            return n;
        default:
            fprintf(stderr, "syntax error on line %d\n", line);
            exit(1);
    }
}

/**
 * <multiplicative expression> := <T_INTEGER_LITERAL> | 
 *                                <T_INTEGER_LITERAL><T_STAR><multiplicative expression> |
 *                                <T_INTEGER_LITERAL><T_SLASH><multiplicative expression>
 * 
 * return the ast tree those root is a '*' or '/' binary operator
 */
struct ast_node *multiplicative_expression(void) {
    struct ast_node *left, *right;
    int token_type;

    /**
     * get the integer literal on the left
     * fetch the next token at the same time
     */
    left = primary();

    token_type = token.token;
    if(token_type == T_EOF)
        return left;
    
    while(token_type == T_STAR || token_type == T_SLASH) {
        /* fetch the next integer literal*/
        scan(&token);
        right = primary();

        /* make a tree node */
        left = ast_makeNode(arithmetic_operator(token_type), 0, left, right);

        token_type = token.token;
        if(token_type == T_EOF)
            break;
    }

    return left;
}


/**
 * <additive expression> := <multiplicative expression> |
 *                          <additive expression><T_PLUS><additive expression> |
 *                          <additive expression><T_MINUS><additive expression>  
 * 
 * return the ast tree those root is a '+' or '-' binary operator
 */
struct ast_node *additive_expression(void) {
    struct ast_node *left, *right;
    int token_type;

    /**
     * get the integer literal on the left
     * fetch the next token at the same time
     */
    left = primary();

    /* check the token_type is notend of file */
    token_type = token.token;
    if(token_type == T_EOF)
        return left;
    
    /* loop at our level precedence */
    while(token_type != T_EOF) {
        /* fetch the next integer literal*/
        scan(&token);

        /* get the right subtree at a higher precedence than us */
        right = multiplicative_expression();

        /* make a tree node */
        left = ast_makeNode(arithmetic_operator(token_type), 0, left, right);

        token_type = token.token;
    }

    return left;
}

static int operator_precedence[] = {
    0,  /* T_EOF */
    10, /* T_PLUS */
    10, /* T_MINUX */
    20, /* T_STAR */
    20, /* T_SLASH */
    0   /* T_INTEGER_LITERAL */
};

static int op_precedence(int token_type) {
    int precedence = operator_precedence[token_type];

    if(precedence == 0) {
        fprintf(stderr, "syntax error on line %d, token %d\n", line, token_type);
        exit(1);
    }

    return precedence;
}

struct ast_node *binary_expression(int ptp) {
    struct ast_node *left, *right;
    int token_type;

    /**
     * get the integer literal on the left
     * fetch the next token at the same time
     */
    left = primary();

    /* check the token_type is notend of file */
    token_type = token.token;
    if(token_type == T_EOF)
        return left;
    
    /* loop at our level precedence */
    while(op_precedence(token_type) > ptp) {
        /* fetch the next integer literal*/
        scan(&token);

        /* get the right subtree at a higher precedence than us */
        right = binary_expression(operator_precedence[token_type]);

        /* make a tree node */
        left = ast_makeNode(arithmetic_operator(token_type), 0, left, right);

        token_type = token.token;
        if(token_type == T_EOF)
            return left;
    }

    return left;
}
