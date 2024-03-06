#include <parser.h>
#include <qcc.h>
#include <qlex.h>
#include <qerr.h>
#include <stdbool.h>
#include <stdio.h>

int syntax_state;
int indent_level;

extern int token;
extern int token_value;

void print_tab(int n) {
    int i = 0;
    for(;i < n; i++)
        printf("\t");
}

void syntax_indent() {
    switch(syntax_state) {
        case SYNTAX_NULL:
            syntax_on();
            break;
        case SYNTAX_SPACE:
            printf(" ");
            syntax_on();
            break;
        case SYNTAX_NEWLINE_INDENT:
            {
                if(token == TOKEN_CLOSE_CURLY)
                    indent_level--;
                printf("\n");
                print_tab(indent_level);
            }
            syntax_on();
            break;
        case SYNTAX_DELAY:
            break;
    }

    syntax_state = SYNTAX_NULL;
}

/**
 * <sizeof expression> ::= <TOKEN_KEY_SIZEOF><TOKEN_OPEN_PARENTH><type specifier><TOKEN_CLOSE_PARENTH>
 * */
void sizeof_expression() {
    get_token();
    skip(TOKEN_OPEN_PARENTH);
    type_specifier();
    skip(TOKEN_CLOSE_PARENTH);
}

/**
 * <primary expression> ::= <TOKEN_KEY_IDENT> |
 *                          <TOKEN_CINT> |
 *                          <TOKEN_CCHAR> |
 *                          <TOKEN_CSTR> |
 *                          <TOKEN_OPEN_PARENTH><expression><TOKEN_CLOSE_PARENTH>
 * */
void primary_expression() {

    int t;

    switch(token) {
        case TOKEN_CINT:
        case TOKEN_CCHAR:
        case TOKEN_CSTR:
            get_token();
            break;
        case TOKEN_OPEN_PARENTH:
            get_token();
            expression();
            skip(TOKEN_CLOSE_PARENTH);
            break;
        default:
            t = token;
            get_token();
            if(t < TOKEN_KEY_IDENT)
                expect("identifier or constant\n");
            break;
    }
}

/**
 * <argument expression list> ::= <assignment expression>{<TOKEN_COMMA><assignment expression>}
 * */
void argument_expression_list() {
    get_token();

    while (token != TOKEN_CLOSE_PARENTH) {
        assignment_expression();

        if (token == TOKEN_CLOSE_PARENTH) {
            break;
        }

        skip(TOKEN_COMMA);
    }

    skip(TOKEN_CLOSE_PARENTH);
}

/**
 * <postfix expression> ::= <primary expression>{<TOKEN_OPEN_BRACKET><expression><TOKEN_CLOSE_BRACKET> |
 *                                               <TOKEN_OPEN_PARENTH><TOKEN_CLOSE_PARENTH> |
 *                                               <TOKEN_OPEN_PARENTH><argument expression list><TOKEN_CLOSE_PARENTH> |
 *                                               <TOKEN_DOT><TOKEN_KEY_IDENT> |
 *                                               <TOKEN_POINTTO><TOKEN_KEY_IDENT>}
 *
 * example:
 * <TOKEN_OPEN_BRACKET><expression><TOKEN_CLOSE_BRACKET>                : [ expression ]
 * <TOKEN_OPEN_PARENTH><TOKEN_CLOSE_PARENTH>                            : ( )
 * <TOKEN_OPEN_PARENTH><argument expression list><TOKEN_CLOSE_PARENTH>  : (int a, int b)
 * <TOKEN_DOT><TOKEN_KEY_IDENT>                                         : .n
 * <TOKEN_POINTTO><TOKEN_KEY_IDENT>                                     : ->n
 * */
void postfix_expression() {
    primary_expression();
    while(1) {
        if(token == TOKEN_DOT || token == TOKEN_POINTTO) {
            get_token();
            token |= S_MEMBER;
            get_token();
        } else if(token == TOKEN_OPEN_BRACKET) {
            get_token();
            expression();
            skip(TOKEN_CLOSE_BRACKET);
        } else if(token == TOKEN_OPEN_PARENTH)
            argument_expression_list();
        else
            break;
    }
}

/**
 * This function is used to parse pointer, indirect pointer and minus number etc.
 * <unary expression> ::= <postfix expression> |
 *                        <TOKEN_AND><unary expression> |
 *                        <TOKEN_ASTERISK><unary expression> |
 *                        <TOKEN_PLUS><unary expression> |
 *                        <TOKEN_MINUS><unary expression> |
 *                        <sizeof expression>
 * */
void unary_expression() {
    switch(token) {
        case TOKEN_AND:
        case TOKEN_ASTERISK:
        case TOKEN_PLUS:
        case TOKEN_MINUS:
            get_token();
            unary_expression();
            break;
        case TOKEN_KEY_SIZEOF:
            sizeof_expression();
            break;
        default:
            postfix_expression();
            break;
    }
}

/**
 * <additive expression> ::= <multiplicative expression>{<TOKEN_PLUS><multiplicative expression> |
 *                                                       <TOKEN_MINUS><multiplicative expression> |
 * */
void multiplicative_expression() {
    unary_expression();
    while((token == TOKEN_ASTERISK) || (token == TOKEN_DIVIDE) || (token == TOKEN_MOD)) {
        get_token();
        unary_expression();
    }
}

/**
 * <additive expression> ::= <multiplicative expression>{<TOKEN_PLUS><multiplicative expression> |
 *                                                       <TOKEN_MINUS><multiplicative expression> |
 * */
void additive_expression() {
    multiplicative_expression();
    while((token == TOKEN_PLUS) || (token == TOKEN_MINUS)) {
        get_token();
        multiplicative_expression();
    }
}

/**
 * <relation expression> ::= <additive expression>{<TOKEN_LT><additive expression> |
 *                                                 <TOKEN_LT><additive expression> |
 *                                                 <TOKEN_LT><additive expression> |
 *                                                 <TOKEN_LT><additive expression>}
 * */
void relation_expression() {
    additive_expression();
    while((token == TOKEN_LT) || (token == TOKEN_LEQ) ||
          (token == TOKEN_GT) || (token == TOKEN_GEQ)) {
        get_token();
        additive_expression();
    }
}

/**
 * <equality expression> ::= <relational expression>{(<TOKEN_EQ><>)}
 * */
void equality_expression() {
    relation_expression();
    while(token == TOKEN_EQ || token == TOKEN_NEQ) {
        get_token();
        relation_expression();
    }
}

/**
 * <assignment expression> ::= <equality expression> |
 *                             <unary expression><TOKEN_ASSIGN><assignment expression>
 *
 * <assignment expression> ::= <equality expression>{<TOKEN_ASSIGN><assignment expression>}
 *
 * We need to consider these case:
 * 5 = a
 * -8 = 9
 * a = 6
 * arr[1] = 1
 * * a = 8
 * *a = 8
 * a > b = 8
 *
 * In these case, we can use <equality expression> and <unary expression> to check that
 *
 * */
void assignment_expression() {
    equality_expression();
    if(token == TOKEN_ASSIGN) {
        get_token();
        assignment_expression();
    }
}

/**
 * <expression> ::= <assignment expression>{<TOKEN_COMMA><assignment expression>}
 * */
void expression() {
    while(1) {
        assignment_expression();

        if(token != TOKEN_COMMA)
            break;

        get_token();
    }
}

bool is_type_specifier(int t) {
    return (TOKEN_KEY_CHAR <= t) && (t <= TOKEN_KEY_STRUCT);
}

/**
 * <compound statement> ::= <TOKEN_OPEN_CURLY>{<declaration>}{<statement>}<TOKEN_CLOSE_CURLY>
 * first, the indent_level need to plus 1
 * */
void compound_statement() {
    syntax_state = SYNTAX_NEWLINE_INDENT;
    indent_level++;

    get_token();
    while(is_type_specifier(token)) {
        external_declaration(S_LOCAL);
    }

    while(token != TOKEN_CLOSE_CURLY) {
        statement();
    }

    syntax_state = SYNTAX_NEWLINE_INDENT;
    get_token();
}

/**
 * <return statement> ::= <TOKEN_KEY_RETURN><TOKEN_SEMICOLON> |
 *                        <TOKEN_KEY_RETURN><expression><TOKEN_SEMICOLON>
 *
 * simplify that:
 * <return statement> ::= <TOKEN_KEY_RETURN>[<expression>]<TOKEN_SEMICOLON>
 * */
void return_statement() {
    syntax_state = SYNTAX_DELAY;
    get_token();

    if(token == TOKEN_SEMICOLON)
        syntax_state = SYNTAX_NULL;
    else
        syntax_state = SYNTAX_SPACE;

    syntax_indent();

    if(token != TOKEN_SEMICOLON)
        expression();

    syntax_state = SYNTAX_NEWLINE_INDENT;
    skip(TOKEN_SEMICOLON);
}

/**
 * <break statement> ::= <TOKEN_KEY_BREAK><TOKEN_SEMICOLON>
 * */
void break_statement() {
    get_token();
    syntax_state = SYNTAX_NEWLINE_INDENT;
    skip(TOKEN_SEMICOLON);
}

/**
 * <continue statement> ::= <TOKEN_KEY_CONTINUE><TOKEN_SEMICOLON>
 * */
void continue_statement() {
    get_token();
    syntax_state = SYNTAX_NEWLINE_INDENT;
    skip(TOKEN_SEMICOLON);
}

/**
 * <for statement> ::= <TOKEN_KEY_FOR><TOKEN_OPEN_PARENTH><expression_statement>
 *                     <expression_statement><expression><TOKEN_CLOSE_PARENTH>
 *                     <statement>
 * */
void for_statement() {
    get_token();
    skip(TOKEN_OPEN_PARENTH);
    if(token != TOKEN_SEMICOLON) {
        expression();
    }

    skip(TOKEN_OPEN_PARENTH);
    if(token != TOKEN_SEMICOLON) {
        expression();
    }

    skip(TOKEN_OPEN_PARENTH);
    if(token != TOKEN_CLOSE_PARENTH) {
        expression();
    }
    syntax_state = SYNTAX_NEWLINE_INDENT;
    skip(TOKEN_CLOSE_PARENTH);
    statement();
}

/**
 * <if statement> ::= <TOKEN_KEY_IF><TOKEN_OPEN_PARENTH><expression><TOKEN_CLOSE_PARENTH>
 *                    <statement>[<TOKEN_KEY_ELSE><statement>]
 *
 * here, we can deal with "if", "else if" and "else" three kinds of statement
 * */
void if_statement() {
    syntax_state = SYNTAX_SPACE;
    get_token();
    skip(TOKEN_OPEN_PARENTH);
    expression();
    syntax_state = SYNTAX_NEWLINE_INDENT;
    skip(TOKEN_CLOSE_PARENTH);
    statement();
    if(token == TOKEN_KEY_ELSE) {
        syntax_state = SYNTAX_NEWLINE_INDENT;
        get_token();
        statement();
    }
}

/**
 * <expression statement> ::= <TOKEN_SEMICOLON> |
 *                            <expression><TOKEN_SEMICOLON>
 *
 * we can also simplify the definition:
 * <expression statement> ::= [<expression>]<TOKEN_SEMICOLON>
 * */
void expression_statement() {
    if(token != TOKEN_SEMICOLON)
        expression();

    syntax_state = SYNTAX_NEWLINE_INDENT;
    skip(TOKEN_SEMICOLON);
}


/**
 * <statement> ::=  <compound statement> |
 *                  <if statement> |
 *                  <return statement> |
 *                  <break statement> |
 *                  <continue statement> |
 *                  <for statement> |
 *                  <expression statement>
 * */
void statement() {
    switch(token) {
        case TOKEN_OPEN_CURLY:
            compound_statement();
            break;
        case TOKEN_KEY_IF:
            if_statement();
            break;
        case TOKEN_KEY_RETURN:
            return_statement();
            break;
        case TOKEN_KEY_BREAK:
            break_statement();
            break;
        case TOKEN_KEY_CONTINUE:
            continue_statement();
            break;
        case TOKEN_KEY_FOR:
            for_statement();
            break;
        default:
            expression_statement();
            break;
    }
}

/**
 * <initializer> ::= <assignment expression>
 * */
void initializer() {
    assignment_expression();
}

/**
 * <function body> ::= <compound statement>
 * */
void function_body() {
    compound_statement();
}

/**
 * <parameter type list> ::= <parameter list> |
 *                           <parameter list><TOKEN_COMMA><TOKEN_ELLIPSE>
 *
 * <parameter list> ::= <parameter declaration>{<TOKEN_COMMA><parameter declaration>}
 *
 * <parameter declaration> ::= <type specifier>{<declarator>}
 * */
void parameter_type_list() {
    get_token();
    while(token != TOKEN_CLOSE_PARENTH) {
        if(!type_specifier())
            error("Invalid identifier\n");
        declarator();

        if(token == TOKEN_CLOSE_PARENTH)
            break;
        skip(TOKEN_COMMA);
        if(token == TOKEN_ELLIPSIS){
            get_token();
            break;
        }
    }

    syntax_state = SYNTAX_DELAY;
    skip(TOKEN_CLOSE_PARENTH);
    if(token == TOKEN_OPEN_CURLY)
        syntax_state = SYNTAX_NEWLINE_INDENT;
    else
        syntax_state = SYNTAX_NULL;
    syntax_indent();
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
    printf("translation_unit start\n");
    while(token != TOKEN_EOF) {
        external_declaration(S_GLOBAL);
    }
}