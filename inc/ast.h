#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

enum {
    A_ADD,
    A_SUBSTRACT,
    A_MULTIPLY,
    A_DIVIDE,
    A_INTEGER_LITERAL,
};


/***
 * We 
 * 
 */
struct ast_node {
    int opcode;             /* operation to be performed on this tree */
    int integer_value;
    struct ast_node *left;  /* left and right child trees */
    struct ast_node *right;
};

/* build and return a generic ast node */
struct ast_node *ast_makeNode(int opcode, int integer_value, struct ast_node *left, struct ast_node *right);

/* make an ast leaf */
struct ast_node *ast_makeLeaf(int opcode, int integer_value);

/* make a unary ast node, only one child */
struct ast_node *ast_makeUnary(int opcode, int integer_value, struct ast_node *left);

/* ast dump */
int ast_interpreter(struct ast_node *n);