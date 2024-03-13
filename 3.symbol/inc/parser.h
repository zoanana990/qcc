#ifndef __PARSER_H__
#define __PARSER_H__

/*
 * syntax status
 * */
enum{
    SYNTAX_NULL,                /* null state, no action */
    SYNTAX_SPACE,               /* a space */
    SYNTAX_NEWLINE_INDENT,      /* a new line and indent */
    SYNTAX_DELAY,
};

enum {
    T_INT,                  /* integer      */
    T_CHAR,                 /* character    */
    T_SHORT,                /* short        */
    T_VOID,                 /* void         */
    T_PTR,                  /* pointer      */
    T_FUNC,                 /* function     */
    T_STRUCT,               /* structure    */
    T_BYTE = 0xf,           /* byte         */
    T_ARRAY,                /* array        */
};

#define ALGIN_SET           (0x100)
void expression();
void assignment_expression();
void statement();
void declarator();
int type_specifier();
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