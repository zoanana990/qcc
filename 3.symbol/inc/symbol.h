#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <qstack.h>

struct symbol;

enum {
    TYPE_CHAR,
    TYPE_SHORT,
    TYPE_INT,
    TYPE_VOID,
    TYPE_PTR,
    TYPE_FUNC,
    TYPE_STRUCT,
    TYPE_BTYPE = 0xf,
    TYPE_ARRAY,
};

#define SIZE_VOID           0
#define SIZE_CHAR           1
#define SIZE_SHORT          2
#define SIZE_INT            4


typedef struct type {
    int t;                  /* the type is defined as above */
    struct symbol *ref;     /* the reference structure */
} type_t;

#define SYMBOL_GLOBAL            (0xf0U)
#define SYMBOL_LOCAL             (0xf1U)
#define SYMBOL_OVERFLOW          (0xf2U)
#define SYMBOL_GMP               (0xf3U)
#define SYMBOL_VAL_MASK          (0xffU)
#define SYMBOL_LVALUE            (0x100U)
#define SYMBOL_SYM               (0x200U)
#define SYMBOL_ANONYMOUS         (0x10000000U)
#define SYMBOL_STRUCT            (0x20000000U)
#define SYMBOL_MEMBER            (0x40000000U)
#define SYMBOL_PARAMS            (0x80000000U)

typedef struct symbol {
    int v;                  /* token code */
    int r;                  /* register */
    int c;                  /* relative */
    struct type type;       /* the data type */
    struct symbol *next;    /* next other symbol */
    struct symbol *prev;    /* previous same name symbol */
} symbol_t;

symbol_t *symbol_direct_push(qstack_t *ptr_qs, int v, type_t *ptr_type, int c);
symbol_t *symbol_push(int v, type_t *ptr_type, int r, int c);
symbol_t *symbol_search_struct(int v);
symbol_t *symbol_search(int v);
symbol_t *symbol_function_push(int v, type_t *ptr_type);
symbol_t *symbol_variable_push(type_t *type, int r, int v, int addr);
void symbol_pop(qstack_t *ptr_top, symbol_t *b);
void symbol_make_pointer(type_t *ptr_t);

int type_size(type_t *ptr_type, int *ptr_align);
#endif