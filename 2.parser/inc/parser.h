#ifndef __SYNTAX_H__
#define __SYNTAX_H__

/*
 * syntax status
 * */
enum{
    SYNTAX_NULL,                /* null state, no action */
    SYNTAX_SPACE,               /* a space */
    SYNTAX_NEWLINE_INDENT,      /* a new line and indent */
    SYNTAX_DELAY,
};

#define S_GLOBAL            (0xf0U)
#define S_LOCAL             (0xf1U)
#define S_OVERFLOW          (0xf2U)
#define S_GMP               (0xf3U)
#define S_VAL_MASK          (0xffU)
#define S_LVALUE            (0x100U)
#define S_SYMBOL            (0x200U)
#define S_ANONYMOUS         (0x100000000U)
#define S_STRUCT            (0x200000000U)
#define S_MEMBER            (0x400000000U)
#define S_PARAMS            (0x800000000U)

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