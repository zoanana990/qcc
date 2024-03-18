#ifndef __PARSER_H__
#define __PARSER_H__

#include <symbol.h>

/*
 * syntax status
 * */
enum{
    SYNTAX_NULL,                /* null state, no action */
    SYNTAX_SPACE,               /* a space */
    SYNTAX_NEWLINE_INDENT,      /* a new line and indent */
    SYNTAX_DELAY,
};

#define ALGIN_SET           (0x100)
void expression();
void assignment_expression();
void statement(int *ptr_break_symbol, int *ptr_continue_symbol);
void declarator(type_t *ptr_type, int *ptr_v, int *ptr_force_align);
int type_specifier(type_t *ptr_type);
void translation_unit();
void external_declaration(int l);
void syntax_indent();

#ifdef _WIN32
void function_calling_convention(int *fc);
void struct_member_alignment();
#else
#define function_calling_convention(fc)
#define struct_member_alignment()
#endif

#endif