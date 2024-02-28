#include <qlex.h>
#include <qarr.h>
#include <qerr.h>
#include <qstr.h>
#include <qcc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int          line_num;
int          token;
qarr_t       token_table;
token_t     *ptr_token_hashtable[MAX_KEY];
char         fch;

qstr_t       token_string;

extern FILE *fin;

/* check the char is number */
int is_digit(char c) {
    return (c >= '0' && c <= '9');
}

int is_alpha(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

/* check the char is alphabet or underline */
int is_alpha_or_underline(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'));
}

/* check the char is alphabet, number or underline */
int is_alnum(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || (c == '_'));
}

/**
 * emlf hash function
 */
int elf_hash(char *key) {
    int h = 0, g;
    while(*key) {
        h = (h << 4) + *key++;
        g = h & 0xf0000000;
        if(g)
            h ^= g >> 24;
        h &= ~g;
    }

    return h % MAX_KEY;
}

void *mallocz(int size) {
    void *ptr;
    ptr = malloc(size);
    if(!ptr)
        error("malloc failed\n");

    memset(ptr, 0, size);
    return ptr;
}

/* hash table find */
token_t *token_find(char *ptr_data, int key_num) {

    token_t *ptr_store = NULL, *ptr_curr;

    for(ptr_curr = ptr_token_hashtable[key_num]; ptr_curr; ptr_curr = ptr_curr->next) {
        if(!strcmp(ptr_data, ptr_curr->spelling)) {
            token = ptr_curr->token_code;
            ptr_store = ptr_curr;
        }
    }

    return ptr_store;
}

token_t *token_insert(char *ptr_data) {

    token_t *ptr_t;
    int key_num, length;
    char *s, *end;

    key_num = elf_hash(ptr_data);
    ptr_t = token_find(ptr_data, key_num);

    if(ptr_t == NULL) {

        length = strlen(ptr_data);

        ptr_t = mallocz(sizeof(token_t) + length + 1);

        ptr_t->next = ptr_token_hashtable[key_num];
        ptr_token_hashtable[key_num] = ptr_t;

        qarr_add(&token_table, ptr_t);
        ptr_t->token_code = token_table.count - 1;

        s = (char *) ptr_t + sizeof(token_t);

        ptr_t->spelling = (char *) s;

        for(end = ptr_data + length; ptr_data < end;)
            *s++ = *ptr_data++;

        *s = (char) '\0';

        ptr_t->sym_identifier = NULL;
        ptr_t->sym_struct = NULL;
    }

    return ptr_t;
}

token_t *parse_identifier(void) {

    qstr_reset(&token_string);
    qstr_chcat(&token_string, fch);

    getch();

    while(is_alnum(fch)) {
        qstr_chcat(&token_string, fch);
        getch();
    }
    
    qstr_chcat(&token_string, '\0');
    return token_insert(token_string.data);
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

/* function: skip_white_space
 * 
 * here we need to identify '\r' '\n'
 * '\r': return the mouse to the head of line 
 * '\n': newline
 * 
 * Windows: \r\n to newline
 * Unix:    \n   newline
 * macOS:   \r   to get the newline
 */
void skip_white_space() {
    while((fch = ' ') || (fch = '\t') || (fch = '\r')) {
#if defined (_WIN32)
        if(fch == '\r') {
            getch();
            if(fch != '\n')
                return;
            line_num++;
        }
#elif defined(__linux__)
        if(fch == '\n') {
            line_num++;
        }
#elif defined(__APPLE__)
        if(fch == '\r') {
            line_num++;
        }
#endif
        printf("%c", fch);
        getch();
    }
}

/* function: parse_comment 
 * we need to get the end ot comment string
 */
void parse_comment() {

    getch();

    do {
        do {
            if((fch = '*') || (fch = '\n') || (fch = CH_EOF))
                break;
            else getch();
        } while(1);

        /* deal with * */
        if(fch == '*') {
            getch();
            if(fch == '/') {
                getch();
                return;
            }
        } else if(fch == '\n') {
            line_num++;
            getch();
        } else {
            error("Missing comment string */\n");
            return;
        }
    } while(1);
}

/* function name: word_extract
 * note: ignore the comments and delimiters
 */
void word_extract() {
    while(1) {
        if((fch == ' ') || (fch == '\t') || (fch == '\r'))
            skip_white_space();
        else if(fch == '/') {
            getch();
            /* this is the syntax */
            if(fch == '*')
                parse_comment();
            else if(fch == '/')
                /* here, we need to read the line to the end */
                while(fch != '\n' && fch != EOF)
                    getch();
            else {
                ungetc(fch, fin);
                fch = '/';
                break;
            }
        }
        else 
            break;
    }
}

/* get the token */
void get_token() {
    word_extract();
    switch(fch) {
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': 
        case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': 
        case 'o': case 'p': case 'q': case 'r': case 's': case 't':
        case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': 
        case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': 
        case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
        case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z': 
        case '_':
        {
            token_t *tp;
            tp = parse_identifier();
            token = tp->token_code;
            break;
        }

        case EOF:
            printf("EOF\n");
            token = TOKEN_EOF;
            break;
        default:
            error("cannot recognize character %c\n", fch);
            getch();
            break;
    }
}

void syntax_on() {

}

void lexical_coloring() {
    /* get the character */
    getch();

    do {
        get_token();
        syntax_on();
        break;
    } while(token != TOKEN_EOF);

    printf("Lines of code: %d\n", line_num);
}