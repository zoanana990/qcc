#ifndef __SYMBOL_H__
#define __SYMBOL_H__

struct symbol;

enum {
    TYPE_CHAR,
    TYPE_SHORT,
    TYPE_INT,
    TYPE_VOID,
    TYPE_PTR,
    TYPE_FUNC,
    TYPE_STRUCT,
    TYPE_BYTE,
    TYPE_ARRAY,
};

typedef struct type {
    int t;                  /* the type is defined as above */
    struct symbol *ref;     /* the reference structure */
} type_t;

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

typedef struct symbol {
    int v;                  /* token code */
    int r;                  /* register */
    int c;                  /* relative */
    struct type type;       /* the data type */
    struct symbol *next;    /* next other symbol */
    struct symbol *prev;    /* previous same name symbol */
} symbol_t;

symbol_t *symbol_search_struct(int v);
symbol_t *symbol_search(int v);

#endif