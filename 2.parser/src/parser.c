#include <parser.h>
#include <qcc.h>
#include <qlex.h>
#include <qerr.h>

int syntax_state;
int syntax_level;

extern int token;



void initializer() {

}

void function_body() {

}

void declarator() {

}

void struct_declaration() {

}

void struct_declaration_list() {

}

/**
 * function name: struct_identifier
 *
 * <struct_specifier> ::= <TOKEN_KEY_STRUCT><TOKEN_KEY_IDENT><TOKEN_OPEN_CURLY><struct_declaration_list><TOKEN_CLOSE_CURLY> |
 *                        <TOKEN_KEY_STRUCT><TOKEN_KEY_IDENT>
 *
 * <struct_declaration_list> ::= <struct_declaration>{<struct_declaration>}
 *
 * <struct_declaration> ::= <type specifier><struct_declaration_list><TOKEN_SEMICOLON>
 * <struct_declaration> ::= <type specifier><declarator>{<TOKEN_COMMA><declarator>}<TOKEN_SEMICOLON>
 * */
void struct_identifier() {

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
            struct_identifier();
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