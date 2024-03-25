#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

enum {
    T_PLUS,
    T_MINUS,
    T_STAR,
    T_SLASH,
    T_INT_LITERAL,
};

struct token {
    int token;
    int integer_value;
} token_t;