#include <parser.h>
#include <qcc.h>
#include <qlex.h>
#include <qerr.h>

int syntax_state;
int indent_level;

extern int token;
extern int token_value;

void syntax_indent() {

}

void initializer() {

}

void function_body() {

}

void parameter_type_list() {

}

/**
 * <direct declarator postfix> ::= {<TOKEN_OPEN_BRACKET><TOKEN_CINT><TOKEN_CLOSE_BRACKET> |
 *                                  <TOKEN_OPEN_BRACKET><TOKEN_CLOSE_BRACKET> |
 *                                  <TOKEN_OPEN_PARENTH><parameter list><TOKEN_CLOSE_PARENTH> |
 *                                  <TOKEN_OPEN_PARENTH><TOKEN_CLOSE_PARENTH>}
 *
 * example
 * <TOKEN_OPEN_BRACKET><TOKEN_CINT><TOKEN_CLOSE_BRACKET>        : [2]
 * <TOKEN_OPEN_BRACKET><TOKEN_CLOSE_BRACKET>                    : []
 * <TOKEN_OPEN_PARENTH><parameter list><TOKEN_CLOSE_PARENTH>    : (int a, char b)
 * <TOKEN_OPEN_PARENTH><TOKEN_CLOSE_PARENTH>                    : ()
 * */
void direct_declarator_postfix() {
    int n;
    if(token == TOKEN_OPEN_PARENTH)
        parameter_type_list();
    else if(token == TOKEN_OPEN_BRACKET) {
        get_token();
        if(token == TOKEN_CINT) {
            get_token();
            n = token_value;
        }

        skip(TOKEN_CLOSE_BRACKET);
        direct_declarator_postfix();
    }
}

/**
 * <direct declarator> ::= <TOKEN_KEY_IDENT><direct declarator postfix>
 * */
void direct_declarator() {
    if(token >= TOKEN_KEY_IDENT)
        get_token();
    else
        expect("Identifier\n");

    direct_declarator_postfix();
}

#ifdef _WIN32
/**
 * This keyword is used in Windows kernel
 *
 * <structure member alignment> ::= <TOKEN_KEY_ALIGN><TOKEN_OPEN_CURLY><TOKEN_CINT><TOKEN_CLOSE_CURLY>
 * */
void struct_member_alignment() {
    if(token == TOKEN_KEY_ALIGN) {
        get_token();
        skip(TOKEN_OPEN_PARENTH);
        if(token == TOKEN_CINT)
            get_token();
        else
            expect("constant integer\n");
        skip(TOKEN_OPEN_PARENTH);
    }
}


/* This function is used to call windows kernel API,
 * e.g. int __stdcall WinMain(...)
 *
 * In other operating system, like macOS or linux
 * It will no need to use this kind of words.
 *
 * <function calling convention> ::= <TOKEN_KEY_CDECL> | <TOKEN_KEY_STDCALL>
 * */
void function_calling_convention(int *fc) {
    *fc = TOKEN_KEY_CDECL;
    if(token == TOKEN_KEY_CDECL || token == TOKEN_KEY_STDCALL) {
        *fc = token;
        syntax_state = SYNTAX_SPACE;
        get_token();
    }
}
#endif

/**
 * in this function, we need to consider the pointer declaration, array declaration etc.
 *
 * in windows platform
 * <declarator> ::= {<TOKEN_ASTERISK>}[<function calling convention>][<struct member alignment>]<direct declarator>
 *
 * in linux or other unix like platform
 * <declarator> ::= {<TOKEN_ASTERISK>}<direct declarator>
 */
void declarator() {
    int fc;
    while(token == TOKEN_ASTERISK) {
        get_token();
    }

    function_calling_convention(&fc);
    struct_member_alignment();
    direct_declarator();
}

/**
 * <struct_declaration> ::= <type specifier><struct_declaration_list><TOKEN_SEMICOLON>
 * <struct_declaration> ::= <type specifier><declarator>{<TOKEN_COMMA><declarator>}<TOKEN_SEMICOLON>
 */
void struct_declaration() {
    
    type_specifier();
    while(1) {
        declarator();
        if(token == TOKEN_SEMICOLON)
            break;
        
        skip(TOKEN_COMMA);
    }

    syntax_state = SYNTAX_NEWLINE_INDENT;
    skip(TOKEN_SEMICOLON);
}

/**
 * <struct_declaration_list> ::= <struct_declaration>{<struct_declaration>}
 */
void struct_declaration_list() {
    int max_align, offset;
    
    /* the first member is in a newline 
     * that is
     * struct example {
     *      int first_member;
     * }
     * 
     * You will see that first_member is located on the next line of "{"
     */
    syntax_state = SYNTAX_NEWLINE_INDENT;
    indent_level++;

    get_token();

    /* while the token is not '}', then it is still in structure definition */
    while(token != TOKEN_CLOSE_CURLY)
        struct_declaration();
    
    skip(TOKEN_CLOSE_CURLY);

    syntax_state = SYNTAX_NEWLINE_INDENT;
}

/**
 * function name: struct_specifier
 *
 * <struct_specifier> ::= <TOKEN_KEY_STRUCT><TOKEN_KEY_IDENT><TOKEN_OPEN_CURLY><struct_declaration_list><TOKEN_CLOSE_CURLY> |
 *                        <TOKEN_KEY_STRUCT><TOKEN_KEY_IDENT>
 *
 * <struct_declaration_list> ::= <struct_declaration>{<struct_declaration>}
 *
 * <struct_declaration> ::= <type specifier><struct_declaration_list><TOKEN_SEMICOLON>
 * <struct_declaration> ::= <type specifier><declarator>{<TOKEN_COMMA><declarator>}<TOKEN_SEMICOLON>
 * */
void struct_specifier() {
    int v;
    get_token();
    v = token;

    syntax_state = SYNTAX_DELAY;

    get_token();

    if(token == TOKEN_OPEN_CURLY)
        syntax_state = SYNTAX_NEWLINE_INDENT;
    else if(token == TOKEN_CLOSE_CURLY)
        syntax_state = SYNTAX_NULL;
    else
        syntax_state = SYNTAX_SPACE;
    
    syntax_indent();

    if(v < TOKEN_KEY_IDENT)
        error("Expect structure name\n");
    
    if(token == TOKEN_OPEN_CURLY)
        struct_declaration_list();
}

/**
 * function name: type_specifier
 * description: parse delimiter
 *
 * <type specifier> ::= <TOKEN_KEY_CHAR> |
 *                      <TOKEN_KEY_SHORT> |
 *                      <TOKEN_KEY_INT> |
 *                      <TOKEN_KEY_VOID> |
 *                      <struct specifier> |
 * TODO:                <enum specifier> | <union specifier> etc.
 *
 * in LL(1), there is a select set
 * SELECT(<type specifier> -> <TOKEN_KEY_CHAR>) = FIRST(<TOKEN_KEY_CHAR>) = {<TOKEN_KEY_CHAR>}
 * SELECT(<type specifier> -> <TOKEN_KEY_SHORT>) = FIRST(<TOKEN_KEY_SHORT>) = {<TOKEN_KEY_SHORT>}
 * SELECT(<type specifier> -> <TOKEN_KEY_INT>) = FIRST(<TOKEN_KEY_INT>) = {<TOKEN_KEY_INT>}
 * SELECT(<type specifier> -> <TOKEN_KEY_VOID>) = FIRST(<TOKEN_KEY_VOID>) = {<TOKEN_KEY_VOID>}
 * SELECT(<type specifier> -> <struct specifier>) = FIRST(<struct specifier>) = {<TOKEN_KEY_STRUCT>}
 * */
int type_specifier() {
    int type_found = 0;
    switch(token) {
        case TOKEN_KEY_CHAR:
        case TOKEN_KEY_SHORT:
        case TOKEN_KEY_INT:
        case TOKEN_KEY_VOID:
            type_found = 1;
            syntax_state = SYNTAX_SPACE;
            get_token();
            break;
        case TOKEN_KEY_STRUCT:
            type_found = 1;
            syntax_state = SYNTAX_SPACE;
            struct_specifier();
            break;
        default:
            break;
    }

    return type_found;
}

/**
 * function name: external_declaration
 * description: parse global declaration and function body
 *
 * note:
 * Here we need to use EBNF representation to implement the parser
 * <external declaration> ::= <function definition> | <declaration>
 * where:
 *  <function definition> ::= <type specifier><declarator><function body>
 *  <declaration> ::= <type specifier><TOKEN_SEMICOLON> |
 *                    <type specifier><init declarator list><TOKEN_SEMICOLON>
 * 
 * <init declarator list> ::= <init declarator>{<TOKEN_COMMA><init declarator>}
 * 
 * <init declarator> ::= <declarator>{<TOKEN_ASSIGN><initializer>}
 *  e.g. int a = 3; where "int a" is declarator, "=" is TOKEN_ASSIGN and "3" is initializer
 * 
 * Thus, we can expand the EBNF representation
 * <external declaration> ::= <type specifier><declarator><function body> |
 *                            <type specifier><TOKEN_SEMICOLON> |
 *                            <type specifier><init declarator list><TOKEN_SEMICOLON>
 * 
 * continue,
 * <external declaration> ::= <type specifier><declarator><function body> |
 *                            <type specifier><TOKEN_SEMICOLON> |
 *                            <type specifier><declarator>{<TOKEN_ASSIGN><initializer>}{<TOKEN_COMMA><init declarator>}<TOKEN_SEMICOLON>
 * 
 * simplify
 * <external declaration> ::= <type specifier>(<declarator><function body> | <TOKEN_SEMICOLON> |
 *                                             <declarator>{<TOKEN_ASSIGN><initializer>}{<TOKEN_COMMA><init declarator>}<TOKEN_SEMICOLON>)
 *
 * <external declaration> ::= <type specifier>(<TOKEN_SEMICOLON> | <declarator>(<function body> | ...)<TOKEN_SEMICOLON>)
 *
 * An example for external_declaration function
 * int g_a;                     // This is global declaration
 * int add(int x, int y);       // This is function declaration
 * int add(int x, int y) {      // This is function definition
 *      int z = x + y;          // local declaration
 *      return z;
 * }
 * */
void external_declaration(int l) {
    if(!type_specifier()) {
        error("expect <type specifier>\n");
    }

    if(token == TOKEN_SEMICOLON) {
        get_token();
        return;
    }

    while(1) {
        declarator();

        if(token == TOKEN_OPEN_CURLY) {
            if(l == S_LOCAL) {
                error("Do not support nested define\n");
                function_body();
                break;
            } else {
                if(token == TOKEN_ASSIGN) {
                    get_token();
                    initializer();
                }

                if(token == TOKEN_COMMA) {
                    get_token();
                } else {
                    syntax_state = SYNTAX_NEWLINE_INDENT;
                    skip(TOKEN_SEMICOLON);
                    break;
                }
            }
        }
    }
}

/**
 * <translation unit> ::= {<external declaration>}<TOKEN_EOF>
 * */
void translation_unit() {
    while(token != TOKEN_EOF) {
        external_declaration(S_GLOBAL);
    }
}