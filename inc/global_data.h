#pragma once
#ifndef extern_
#define extern_ extern
#endif

#include <stdio.h>
#include <token.h>

extern_ int              line;
extern_ int              put_back;
extern_ FILE            *input_file;
extern_ struct token     token;