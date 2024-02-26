#include <qlex.h>
#include <qarr.h>
#include <qerr.h>

#include <stdio.h>


int          line_num;
int          token;
qarr_t       token_table;
token_t     *ptr_token_hashtable[MAX_KEY];
char         fch;

extern FILE *fin;

/**
 * elf hash function
 */
int elf_hash(char *key) {
    int h=0, g;
    while(*key) {
        h = (h << 4) + *key++;
        g = h & 0xf0000000;
        if(g)
            h ^= g >> 24;
        h &= ~g;
    }

    return h % MAX_KEY;
}

token_t* token_direct_insert(token_t *tp) {
    int key_num;

    tp->sym_identifier = NULL;
    tp->sym_struct = NULL;

    qarr_add(&token_table, tp);

    key_num = elf_hash(tp->spelling);

    tp->next = ptr_token_hashtable[key_num];
    ptr_token_hashtable[key_num] = tp;
    return tp;
}

void init_lex() {
    token_t *tp;

    DBG("start init keywords\n");
    static token_t keywords[] = {
        {TOKEN_PLUS,            NULL, "+",            NULL, NULL},
        {TOKEN_MINUS,           NULL, "-",            NULL, NULL},
        {TOKEN_STAR,            NULL, "*",            NULL, NULL},
        {TOKEN_DIVIDE,          NULL, "/",            NULL, NULL},
        {TOKEN_MOD,             NULL, "%",            NULL, NULL},
        {TOKEN_EQ,              NULL, "==",           NULL, NULL},
        {TOKEN_NEQ,             NULL, "!=",           NULL, NULL},
        {TOKEN_LT,              NULL, "<",            NULL, NULL},
        {TOKEN_LEQ,             NULL, "<=",           NULL, NULL},
        {TOKEN_GT,              NULL, ">",            NULL, NULL},
        {TOKEN_GEQ,             NULL, ">=",           NULL, NULL},
        {TOKEN_ASSIGN,          NULL, "=",            NULL, NULL},
        {TOKEN_POINTTO,         NULL, "->",           NULL, NULL},
        {TOKEN_DOT,             NULL, ".",            NULL, NULL},
        {TOKEN_BAND,            NULL, "&",            NULL, NULL},
        {TOKEN_AND,             NULL, "&&",           NULL, NULL},
        {TOKEN_BOR,             NULL, "|",            NULL, NULL},
        {TOKEN_OR,              NULL, "||",           NULL, NULL},
        {TOKEN_OPEN_PARENTH,    NULL, "(",            NULL, NULL},
        {TOKEN_CLOSE_PARENTH,   NULL, ")",            NULL, NULL},
        {TOKEN_OPEN_BRACKET,    NULL, "[",            NULL, NULL},
        {TOKEN_CLOSE_BRACKET,   NULL, "]",            NULL, NULL},
        {TOKEN_OPEN_CURLY,      NULL, "{",            NULL, NULL},
        {TOKEN_CLOSE_CURLY,     NULL, "}",            NULL, NULL},
        {TOKEN_SEMICOLON,       NULL, ";",            NULL, NULL},
        {TOKEN_COMMA,           NULL, ",",            NULL, NULL},
        {TOKEN_COLON,           NULL, ":",            NULL, NULL},
        {TOKEN_ELLIPSIS,        NULL, "...",          NULL, NULL},
        {TOKEN_EOF,             NULL, "end of file",  NULL, NULL},
        {TOKEN_CINT,            NULL, "int const",    NULL, NULL},
        {TOKEN_CCHAR,           NULL, "char const",   NULL, NULL},
        {TOKEN_CSTR,            NULL, "string const", NULL, NULL},
        {TOKEN_KEY_CHAR,        NULL, "char",         NULL, NULL},
        {TOKEN_KEY_SHORT,       NULL, "short",        NULL, NULL},
        {TOKEN_KEY_INT,         NULL, "int",          NULL, NULL},
        {TOKEN_KEY_VOID,        NULL, "void",         NULL, NULL},
        {TOKEN_KEY_STRUCT,      NULL, "struct",       NULL, NULL},
        {TOKEN_KEY_IF,          NULL, "if",           NULL, NULL},
        {TOKEN_KEY_ELSE,        NULL, "else",         NULL, NULL},
        {TOKEN_KEY_FOR,         NULL, "for",          NULL, NULL},
        {TOKEN_KEY_CONTINUE,    NULL, "continue",     NULL, NULL},
        {TOKEN_KEY_BREAK,       NULL, "break",        NULL, NULL},
        {TOKEN_KEY_RETURN,      NULL, "return",       NULL, NULL},
        {TOKEN_KEY_SIZEOF,      NULL, "sizeof",       NULL, NULL},
        {TOKEN_KEY_ALIGN,       NULL, "__align",      NULL, NULL},
        {TOKEN_KEY_CDECL,       NULL, "__cdcel",      NULL, NULL},
        {TOKEN_KEY_STDCALL,     NULL, "__stdcall",    NULL, NULL},
        {0,                     NULL, NULL,           NULL, NULL},
    };
    qarr_init(&token_table, 8);
    
    for(tp = keywords; tp->spelling != NULL; tp++) {
        token_direct_insert(tp);
    }
}

/* function name: getch
 * description: get a character from standard library 
 */
void getch() {
    fch = fgetc(fin);
}

void skip_white_space() {

}

void parse_comment() {
    
}

/* function name: word_extract
 * note: ignore the comments and delimiters
 */
void word_extract() {
    while(1) {
        if((fch == ' ') || (fch == '\t') || (fch == '\r'))
            skip_white_space();
        else if(fch == '/') {
            /* // : comment
             * /* : comment
             */
            getch();
            if(fch == '*')
                parse_comment();
            else {

            }
        }
    }
}

/* get the token */
void get_token() {
    word_extract();
}

void syntax_on() {

}

void lexical_coloring() {
    do {
        get_token();
        syntax_on();
        break;
    } while(token != TOKEN_EOF);

    printf("Lines of code: %d\n", line_num);
}