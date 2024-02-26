#ifndef __QCC_H__
#define __QCC_H__

#define MAX_KEY                  (1024)
#define DBG                      printf

enum e_token_code {
    TOKEN_PLUS,             /* plus                 :  +    */
    TOKEN_MINUS,            /* minux                :  -    */
    TOKEN_STAR,             /* start                :  *    */
    TOKEN_DIVIDE,           /* divide               :  /    */
    TOKEN_MOD,              /* mod                  :  %    */
    TOKEN_EQ,               /* equal                :  ==   */
    TOKEN_NEQ,              /* not equal            :  !=   */
    TOKEN_LT,               /* less than            :  <    */
    TOKEN_LEQ,              /* less or equal        :  <=   */
    TOKEN_GT,               /* greater than         :  >    */
    TOKEN_GEQ,              /* greater or equal     :  >=   */
    TOKEN_ASSIGN,           /* assign               :  =    */
    TOKEN_POINTTO,          /* points to            :  ->   */
    TOKEN_DOT,              /* dot                  :  .    */
    TOKEN_BAND,             /* bitwise and          :  &    */
    TOKEN_AND,              /* logical and          :  &&   */
    TOKEN_BOR,              /* bitwise or           :  |    */
    TOKEN_OR,               /* logical or           :  ||   */
    TOKEN_OPEN_PARENTH,     /* left parenthese      :  (    */
    TOKEN_CLOSE_PARENTH,    /* right parenthese     :  )    */
    TOKEN_OPEN_BRACKET,     /* left bracket         :  [    */
    TOKEN_CLOSE_BRACKET,    /* right bracket        :  ]    */
    TOKEN_OPEN_CURLY,       /* left curly bracket   :  {    */
    TOKEN_CLOSE_CURLY,      /* right curly bracket  :  }    */
    TOKEN_SEMICOLON,        /* semicolon            :  ;    */
    TOKEN_COMMA,            /* comma                :  ,    */
    TOKEN_COLON,            /* colon                :  :    */
    TOKEN_ELLIPSIS,         /* ellipsis             :  ...  */

    TOKEN_EOF,              /* end of file      */

    TOKEN_CINT,             /* variable type    */
    TOKEN_CCHAR,            /* variable type    */
    TOKEN_CSTR,             /* variable type    */

    TOKEN_KEY_CHAR,         /* char             */
    TOKEN_KEY_SHORT,        /* short            */
    TOKEN_KEY_INT,          /* int              */
    TOKEN_KEY_VOID,         /* void             */
    TOKEN_KEY_STRUCT,       /* struct           */
    TOKEN_KEY_IF,           /* if               */
    TOKEN_KEY_ELSE,         /* else             */
    TOKEN_KEY_FOR,          /* for              */
    TOKEN_KEY_CONTINUE,     /* continue         */
    TOKEN_KEY_BREAK,        /* break            */
    TOKEN_KEY_RETURN,       /* return           */
    TOKEN_KEY_SIZEOF,       /* sizeof           */

    TOKEN_KEY_ALIGN,        /* __align          */
    TOKEN_KEY_CDECL,        /* __cdecl          */
    TOKEN_KEY_STDCALL,      /* __stdcall        */

    TOKEN_KEY_IDENT,        /* identifier       */
    TOKEN_KEY_NUM,          /* numeric          */

    TOKEN_LAST
};

struct symbol;

typedef struct type {
    int t;
    struct symbol *ref;
} type_t;

typedef struct symbol {
    int v;          /* token code */
    int r;          /* register */
    int c;          /* relative */
    struct type type;
    struct symbol *next;
    struct symbol *prev;
} symbol_t;

typedef struct token {
    int token_code;
    struct token *next; /* the synonym in the linked list */
    char *spelling;
    struct symbol *sym_struct;
    struct symbol *sym_identifier;
} token_t;

#define GET_ALIGNMENT(size)     ((size_t)1 << (64 - __builtin_clzll((size_t)(size) - 1)))

#endif