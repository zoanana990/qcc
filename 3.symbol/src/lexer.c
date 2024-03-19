#include <lexer.h>
#include <vector.h>
#include <qerr.h>
#include <qstr.h>
#include <token.h>

#include <parser.h>
#include <color.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* line count */
int line_cnt;

/* current string token code number (enum) */
int token;

/* word list, we can use trie to implement this, at the future */
qvector_t token_table;

/* global word hashtable */
token_t *ptr_token_hashtable[MAX_KEY];

/* current character get from the file */
char fch;

/* current word string */
qstr_t token_string;

/* current source code string */
qstr_t source_string;

/* current token value */
int token_value;

extern FILE *fin;
extern int syntax_state;

/* check the char is number */
int is_digit(char c) {
    return c >= '0' && c <= '9';
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
            (c >= '0' && c <= '9') || c == '_');
}

/**
 * emlf hash function
 */
int elf_hash(char *key) {
    int h = 0, g;
    while (*key) {
        h = (h << 4) + *key++;
        g = h & 0xf0000000;
        if (g)
            h ^= g >> 24;
        h &= ~g;
    }

    return h % MAX_KEY;
}

/* hash table find */
token_t *token_find(char *ptr_data, int key_num) {

    token_t *ptr_store = NULL, *ptr_curr;

    for (ptr_curr = ptr_token_hashtable[key_num]; ptr_curr; ptr_curr = ptr_curr->next) {
        /**
         * find the node, if the node is found, return the pointer of the token
         * */
        if (!strcmp(ptr_data, ptr_curr->spelling)) {
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

    /**
     * find the node in the hashtable,
     * if the node is exist, then return
     * if the node is not exist, then create the node and
     * insert to the global hashtable
     * */
    ptr_t = token_find(ptr_data, key_num);

    if (ptr_t == NULL) {

        length = strlen(ptr_data);

        ptr_t = calloc(1, sizeof(token_t) + length + 1);

        ptr_t->next = ptr_token_hashtable[key_num];
        ptr_token_hashtable[key_num] = ptr_t;

        vector_add(&token_table, ptr_t);
        ptr_t->token_code = token_table.count - 1;

        s = (char *) ptr_t + sizeof(token_t);

        ptr_t->spelling = (char *) s;

        for (end = ptr_data + length; ptr_data < end;)
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

    while (is_alnum(fch)) {
        qstr_chcat(&token_string, fch);
        getch();
    }

    qstr_chcat(&token_string, '\0');

    return token_insert(token_string.data);
}

void parse_num() {
/**
 * Here we can parse floating point and later we
 * will implement the hex and other base value
 */
    qstr_reset(&token_string);
    qstr_reset(&source_string);

    do {
        qstr_chcat(&token_string, fch);
        qstr_chcat(&source_string, fch);
        getch();
    } while (is_digit(fch));

    if (fch == '.') {
        do {
            qstr_chcat(&token_string, fch);
            qstr_chcat(&source_string, fch);
            getch();
        } while (is_digit(fch));
    }

    qstr_chcat(&token_string, '\0');
    qstr_chcat(&source_string, '\0');

    token_value = atoi(token_string.data);
}

/**
 *
 * */
void parse_string(char sep) {
    char c;

    qstr_reset(&token_string);
    qstr_reset(&source_string);

    qstr_chcat(&source_string, sep);
    getch();

    while (1) {
        if (fch == sep)
            break;
        else if (fch == '\\') {
            qstr_chcat(&source_string, fch);
            getch();
            switch (fch) {
                case '0':
                    c = '\0';
                    break;
                case 'a':
                    c = '\a';
                    break;
                case 'b':
                    c = '\b';
                    break;
                case 't':
                    c = '\t';
                    break;
                case 'n':
                    c = '\n';
                    break;
                case 'v':
                    c = '\v';
                    break;
                case 'r':
                    c = '\r';
                    break;
                case 'f':
                    c = '\f';
                    break;
                case '\\':
                    c = '\\';
                    break;
                case '\'':
                    c = '\'';
                    break;
                case '\"':
                    c = '\"';
                    break;
                default:
                    c = fch;
                    break;
            }
            qstr_chcat(&token_string, c);
            qstr_chcat(&source_string, fch);
            getch();
        } else {
            qstr_chcat(&token_string, fch);
            qstr_chcat(&source_string, fch);
            getch();
        }
    }

    qstr_chcat(&token_string, '\0');
    qstr_chcat(&source_string, sep);
    qstr_chcat(&source_string, '\0');

    getch();
}

token_t *token_direct_insert(token_t *tp) {
    int key_num;

    tp->sym_identifier = NULL;
    tp->sym_struct = NULL;

    vector_add(&token_table, tp);

    key_num = elf_hash(tp->spelling);

    tp->next = ptr_token_hashtable[key_num];
    ptr_token_hashtable[key_num] = tp;
    return tp;
}

void init_lex() {
    token_t *tp;

    static token_t keywords[] = {
            {TOKEN_PLUS, NULL, "+", NULL, NULL},
            {TOKEN_PLUS_EQ, NULL, "+=", NULL, NULL},
            {TOKEN_MINUS, NULL, "-", NULL, NULL},
            {TOKEN_MINUS_EQ, NULL, "-=", NULL, NULL},
            {TOKEN_ASTERISK, NULL, "*", NULL, NULL},
            {TOKEN_MULTI_EQ, NULL, "*=", NULL, NULL},
            {TOKEN_DIVIDE, NULL, "/", NULL, NULL},
            {TOKEN_DIVIDE_EQ, NULL, "/=", NULL, NULL},
            {TOKEN_MOD, NULL, "%", NULL, NULL},
            {TOKEN_MOD_EQ, NULL, "%=", NULL, NULL},
            {TOKEN_EQ, NULL, "==", NULL, NULL},
            {TOKEN_NOT, NULL, "!", NULL, NULL},
            {TOKEN_NEQ, NULL, "!=", NULL, NULL},
            {TOKEN_SHIFT_LEFT, NULL, "<<", NULL, NULL},
            {TOKEN_SHIFT_LEFT_EQ, NULL, "<<=", NULL, NULL},
            {TOKEN_SHIFT_RIGHT, NULL, ">>", NULL, NULL},
            {TOKEN_SHIFT_RIGHT_EQ, NULL, ">>=", NULL, NULL},
            {TOKEN_LT, NULL, "<", NULL, NULL},
            {TOKEN_LEQ, NULL, "<=", NULL, NULL},
            {TOKEN_GT, NULL, ">", NULL, NULL},
            {TOKEN_GEQ, NULL, ">=", NULL, NULL},
            {TOKEN_ASSIGN, NULL, "=", NULL, NULL},
            {TOKEN_POINTTO, NULL, "->", NULL, NULL},
            {TOKEN_DOT, NULL, ".", NULL, NULL},
            {TOKEN_BAND, NULL, "&", NULL, NULL},
            {TOKEN_BAND_EQ, NULL, "&=", NULL, NULL},
            {TOKEN_AND, NULL, "&&", NULL, NULL},
            {TOKEN_BOR, NULL, "|", NULL, NULL},
            {TOKEN_BOR_EQ, NULL, "|=", NULL, NULL},
            {TOKEN_OR, NULL, "||", NULL, NULL},
            {TOKEN_OPEN_PARENTH, NULL, "(", NULL, NULL},
            {TOKEN_CLOSE_PARENTH, NULL, ")", NULL, NULL},
            {TOKEN_OPEN_BRACKET, NULL, "[", NULL, NULL},
            {TOKEN_CLOSE_BRACKET, NULL, "]", NULL, NULL},
            {TOKEN_OPEN_CURLY, NULL, "{", NULL, NULL},
            {TOKEN_CLOSE_CURLY, NULL, "}", NULL, NULL},
            {TOKEN_SEMICOLON, NULL, ";", NULL, NULL},
            {TOKEN_COMMA, NULL, ",", NULL, NULL},
            {TOKEN_COLON, NULL, ":", NULL, NULL},
            {TOKEN_ELLIPSIS, NULL, "...", NULL, NULL},
            {TOKEN_POUND, NULL, " ", NULL, NULL},
            {TOKEN_EOF, NULL, "\n\nend of file\n", NULL, NULL},
            {TOKEN_CINT, NULL, "int const", NULL, NULL},
            {TOKEN_CCHAR, NULL, "char const", NULL, NULL},
            {TOKEN_CSTR, NULL, "string const", NULL, NULL},
            {TOKEN_KEY_CHAR, NULL, "char", NULL, NULL},
            {TOKEN_KEY_SHORT, NULL, "short", NULL, NULL},
            {TOKEN_KEY_INT, NULL, "int", NULL, NULL},
            {TOKEN_KEY_VOID, NULL, "void", NULL, NULL},
            {TOKEN_KEY_STRUCT, NULL, "struct", NULL, NULL},
            {TOKEN_KEY_IF, NULL, "if", NULL, NULL},
            {TOKEN_KEY_ELSE, NULL, "else", NULL, NULL},
            {TOKEN_KEY_FOR, NULL, "for", NULL, NULL},
            {TOKEN_KEY_CONTINUE, NULL, "continue", NULL, NULL},
            {TOKEN_KEY_BREAK, NULL, "break", NULL, NULL},
            {TOKEN_KEY_RETURN, NULL, "return", NULL, NULL},
            {TOKEN_KEY_SIZEOF, NULL, "sizeof", NULL, NULL},
#ifdef _WIN32
            {TOKEN_KEY_ALIGN,       NULL, "__align",      NULL, NULL},
            {TOKEN_KEY_CDECL,       NULL, "__cdcel",      NULL, NULL},
            {TOKEN_KEY_STDCALL,     NULL, "__stdcall",    NULL, NULL},
#endif
            {0, NULL, NULL, NULL, NULL},
    };
    vector_init(&token_table, 8);

    for (tp = keywords; tp->spelling != NULL; tp++) {
        token_direct_insert(tp);
    }
}

/**
 * function name: getch
 * description: get a character from standard library 
 */
void getch() {
    fch = fgetc(fin);
}

/**
 * function: skip_white_space
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
    while ((fch == ' ') || (fch == '\t') || (fch == '\r') || (fch == '\n')) {
#if defined (_WIN32)
        if(fch == '\r') {
            getch();
            if(fch != '\n')
                return;
            line_cnt++;
        }
#elif defined(__linux__) || defined(__APPLE__)
        if (fch == '\n') {
            line_cnt++;
        }
#endif
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
            if ((fch = '*') || (fch = '\n') || (fch = CH_EOF))
                break;
            else getch();
        } while (1);

        /* deal with * */
        if (fch == '*') {
            getch();
            if (fch == '/') {
                getch();
                return;
            }
        } else if (fch == '\n') {
            line_cnt++;
            getch();
        } else {
            error("Missing comment string */\n");
            return;
        }
    } while (1);
}

void preprocessor() {
    while (fch != '\n' && fch != EOF)
        getch();
    if(fch == '\n')
        getch();
}

/* function name: word_extract
 * note: ignore the comments and delimiters
 */
void word_extract() {
    while (1) {
        if ((fch == ' ') || (fch == '\t') || (fch == '\r') || (fch == '\n'))
            skip_white_space();
        else if (fch == '/') {
            getch();
            /**
             * Here, we need to deal with two kinds of commenet
             * */
            if (fch == '*')
                parse_comment();
            else if (fch == '/')
                /* here, we need to read the line to the end */
                while (fch != '\n' && fch != EOF)
                    getch();
            else {
                ungetc(fch, fin);
                fch = '/';
                break;
            }
        } else
            break;
    }
}

/* get the token */
void get_token() {
    word_extract();
    switch (fch) {
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case '_': {
            token_t *tp;
            tp = parse_identifier();
            token = tp->token_code;
            break;
        }
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            parse_num();
            token = TOKEN_CINT;
            break;
        case '+':
            getch();
            if (fch == '=') {
                token = TOKEN_PLUS_EQ;
                getch();
            } else
                token = TOKEN_PLUS;
            break;
        case '-':
            getch();
            if (fch == '=') {
                token = TOKEN_MINUS_EQ;
                getch();
            } else if (fch == '>') {
                token = TOKEN_POINTTO;
                getch();
            } else
                token = TOKEN_MINUS;
            break;
        case '*':
            getch();
            if (fch == '=') {
                token = TOKEN_MULTI_EQ;
                getch();
            } else
                token = TOKEN_ASTERISK;
            break;
        case '/':
            getch();
            if (fch == '=') {
                token = TOKEN_DIVIDE_EQ;
                getch();
            } else
                token = TOKEN_DIVIDE;
            break;
        case '%':
            getch();
            if (fch == '=') {
                token = TOKEN_MOD_EQ;
                getch();
            } else
                token = TOKEN_MOD;
            break;
        case '=':
            getch();
            if (fch == '=') {
                token = TOKEN_EQ;
                getch();
            } else
                token = TOKEN_ASSIGN;
            break;
        case '!':
            getch();
            if (fch == '=') {
                token = TOKEN_NEQ;
                getch();
            } else
                token = TOKEN_NOT;
            break;
        case '<':
            getch();
            if (fch == '<') {
                getch();
                if (fch == '=') {
                    token = TOKEN_SHIFT_LEFT_EQ;
                    getch();
                } else
                    token = TOKEN_SHIFT_LEFT;
            } else if (fch == '=') {
                token = TOKEN_LEQ;
                getch();
            } else
                token = TOKEN_LT;
            break;
        case '>':
            getch();
            if (fch == '>') {
                getch();
                if (fch == '=') {
                    token = TOKEN_SHIFT_RIGHT_EQ;
                    getch();
                } else
                    token = TOKEN_SHIFT_RIGHT;
            } else if (fch == '=') {
                token = TOKEN_GEQ;
                getch();
            } else
                token = TOKEN_GT;
            break;
        case '.':
            getch();
            if (fch == '.') {
                getch();
                if (fch == '.') {
                    token = TOKEN_ELLIPSIS;
                    getch();
                } else
                    error("It is not ellipsis\n");
            } else {
                token = TOKEN_DOT;
            }
            break;
        case '&':
            getch();
            if (fch == '=') {
                token = TOKEN_BAND_EQ;
                getch();
            } else if (fch == '&') {
                token = TOKEN_AND;
                getch();
            } else
                token = TOKEN_BAND;
            break;
        case '|':
            getch();
            if (fch == '=') {
                token = TOKEN_BOR_EQ;
                getch();
            } else if (fch == '|') {
                token = TOKEN_OR;
                getch();
            } else
                token = TOKEN_BOR;
            break;
        case '(':
            token = TOKEN_OPEN_PARENTH;
            getch();
            break;
        case ')':
            token = TOKEN_CLOSE_PARENTH;
            getch();
            break;
        case '[':
            token = TOKEN_OPEN_BRACKET;
            getch();
            break;
        case ']':
            token = TOKEN_CLOSE_BRACKET;
            getch();
            break;
        case '{':
            token = TOKEN_OPEN_CURLY;
            getch();
            break;
        case '}':
            token = TOKEN_CLOSE_CURLY;
            getch();
            break;
        case ',':
            token = TOKEN_COMMA;
            getch();
            break;
        case ';':
            token = TOKEN_SEMICOLON;
            getch();
            break;
        case ':':
            token = TOKEN_COLON;
            getch();
            break;
        case '#':
            /* do nothing here */
            preprocessor();
            token = TOKEN_POUND;
            get_token();
            syntax_state = SYNTAX_DELAY;
            break;
        case '\'':
            parse_string(fch);
            token = TOKEN_CCHAR;
            token_value = *(char *) token_string.data;
            break;
        case '\"':
            parse_string(fch);
            token = TOKEN_CSTR;
            break;
        case EOF:
            token = TOKEN_EOF;
            break;
        default:
            error("cannot recognize character \"%c\"\n", fch);
            getch();
            break;
    }

    syntax_indent();
}

char *get_token_string(int str_num) {
    if (str_num > token_table.count)
        return NULL;
    else if (str_num >= TOKEN_CINT && str_num <= TOKEN_CSTR)
        return source_string.data;
    else
        /* find the table we create at lex_init */
        return ((token_t *) token_table.data[str_num])->spelling;
}

void syntax_on() {

    char *ptr_c = get_token_string(token);

    if (token >= TOKEN_KEY_IDENT)
        pr_identifier("%s", ptr_c);
    else if (token >= TOKEN_KEY_CHAR)
        pr_keyword("%s", ptr_c);
    else if (token >= TOKEN_CINT)
        pr_constant("%s", ptr_c);
    else
        pr_token("%s", ptr_c);

    pr_info("eof???\n");
}