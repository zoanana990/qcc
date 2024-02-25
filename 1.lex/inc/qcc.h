#ifndef __QCC_H__
#define __QCC_H__

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
    TOKEN_LPARENTH,         /* left parenthese      :  (    */
    TOKEN_RPARENTH,         /* right parenthese     :  )    */
    TOKEN_LBRACKET,         /* left bracket         :  [    */
    TOKEN_RBRACKET,         /* right bracket        :  ]    */
    TOKEN_BEGIN,            /* left curly bracket   :  {    */
    TOKEN_END,              /* right curly bracket  :  }    */
    TOKEN_SEMICOLON,        /* semicolon            :  ;    */
    TOKEN_COMMA,            /* comma                :  ,    */
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

    TOKEN_LAST
};

typedef struct token {
    int token_code;
} token_t;

#endif