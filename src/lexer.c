#include <lexer.h>
#include <global_data.h>
#include <token.h>


static int character_position(char *s, int c) {
    
    /* address of return value */
    char *p;

    /* The strchr() function returns a pointer to the 
     * first occurrence of the character c in the string s.
     */
    p = strchr(s, c);
    
    /* return the index of the digit */
    return (p ? (p - s) : (-1));
}

static int next(void) {
    int c;

    if(put_back) {
        c = put_back;
        put_back = 0;
        return c;
    }

    c = fgetc(input_file);
    if(c == '\n')
        line++;

    return c;
}

static int skip(void) {
    int c;

    c = next();
    while(c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f')
        c = next();

    return c;
}

static void putback(int c) {
    put_back = c;
}

static int scan_int(int c) {
    int k, v = 0;

    /* string token to integer */
    while((k = character_position("0123456789", c)) >= 0) {
        v = v * 10 + k;
        c = next();
    }

    /* When hit a non-integer value, put it back */
    putback(c);

    return v;
}

int scan(struct token *t) {
    int c;

    c = skip();

    switch(c) {
        case EOF:
            return 0;
        case '+':
            t->token = T_PLUS;
            break;
        case '-':
            t->token = T_MINUS;
            break;
        case '*':
            t->token = T_STAR;
            break;
        case '/':
            t->token = T_SLASH;
            break;
        default:
            if(isdigit(c)) {
                t->integer_value = scan_int(c);
                t->token = T_INT_LITERAL;
                break;
            }

            printf("Unrecognised character %c on line %d\n", c, line);
            exit(1);
    }

    return 1;
}