#include <parser.h>
#include <token.h>
#include <lexer.h>
#include <qerr.h>
#include <symbol.h>
#include <qstack.h>
#include <stdbool.h>
#include <stdio.h>

int syntax_state;
int indent_level;

extern int token;
extern int token_value;
extern char fch;
extern qstack_t global_symbol_stack, local_symbol_stack;
extern type_t type_int;

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
    int align, size;
    type_t type;

    get_token();
    skip(TOKEN_OPEN_PARENTH);
    type_specifier(&type);
    skip(TOKEN_CLOSE_PARENTH);

    size = type_size(&type, &align);
    if(size < 0)
        error("sizeof(variable type) calculation failed\n");
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
            token |= SYMBOL_MEMBER;
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

/* These operator should depend on priority */
/**
 * <additive expression> ::= <multiplicative expression>{<TOKEN_PLUS><multiplicative expression> |
 *                                                       <TOKEN_MINUS><multiplicative expression> |
 * */
void multiplicative_expression() {
    unary_expression();
    while((token == TOKEN_ASTERISK) || (token == TOKEN_DIVIDE) || (token == TOKEN_MOD) ||
          (token == TOKEN_BAND) || (token == TOKEN_BOR)) {
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
void compound_statement(int *ptr_break_symbol, int *ptr_continue_symbol) {
    symbol_t *ptr_symbol;
    syntax_state = SYNTAX_NEWLINE_INDENT;
    indent_level++;

    pr_info("stack top\n");
    ptr_symbol = stack_top(&local_symbol_stack);

    get_token();
    while(is_type_specifier(token)) {
        external_declaration(SYMBOL_LOCAL);
    }

    while(token != TOKEN_CLOSE_CURLY) {
        statement(ptr_break_symbol, ptr_continue_symbol);
    }

    symbol_pop(&local_symbol_stack, ptr_symbol);
    pr_info("stack pop\n");
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
void break_statement(int *ptr_break_symbol) {
    get_token();
    syntax_state = SYNTAX_NEWLINE_INDENT;
    skip(TOKEN_SEMICOLON);
}

/**
 * <continue statement> ::= <TOKEN_KEY_CONTINUE><TOKEN_SEMICOLON>
 * */
void continue_statement(int *ptr_continue_symbol) {
    get_token();
    syntax_state = SYNTAX_NEWLINE_INDENT;
    skip(TOKEN_SEMICOLON);
}

/**
 * <for statement> ::= <TOKEN_KEY_FOR><TOKEN_OPEN_PARENTH><expression_statement>
 *                     <expression_statement><expression><TOKEN_CLOSE_PARENTH>
 *                     <statement>
 * */
void for_statement(int *ptr_break_symbol, int *ptr_continue_symbol) {
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
    statement(ptr_break_symbol, ptr_continue_symbol);
}

/**
 * <if statement> ::= <TOKEN_KEY_IF><TOKEN_OPEN_PARENTH><expression><TOKEN_CLOSE_PARENTH>
 *                    <statement>[<TOKEN_KEY_ELSE><statement>]
 *
 * here, we can deal with "if", "else if" and "else" three kinds of statement
 * */
void if_statement(int *ptr_break_symbol, int *ptr_continue_symbol) {
    syntax_state = SYNTAX_SPACE;
    get_token();
    skip(TOKEN_OPEN_PARENTH);
    expression();
    syntax_state = SYNTAX_NEWLINE_INDENT;
    skip(TOKEN_CLOSE_PARENTH);
    statement(ptr_break_symbol, ptr_continue_symbol);
    if(token == TOKEN_KEY_ELSE) {
        syntax_state = SYNTAX_NEWLINE_INDENT;
        get_token();
        statement(ptr_break_symbol, ptr_continue_symbol);
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
void statement(int *ptr_break_symbol, int *ptr_continue_symbol) {
    switch(token) {
        case TOKEN_OPEN_CURLY:
            compound_statement(ptr_break_symbol, ptr_continue_symbol);
            break;
        case TOKEN_KEY_IF:
            if_statement(ptr_break_symbol, ptr_continue_symbol);
            break;
        case TOKEN_KEY_RETURN:
            return_statement();
            break;
        case TOKEN_KEY_BREAK:
            break_statement(ptr_break_symbol);
            break;
        case TOKEN_KEY_CONTINUE:
            continue_statement(ptr_continue_symbol);
            break;
        case TOKEN_KEY_FOR:
            for_statement(ptr_break_symbol, ptr_continue_symbol);
            break;
        default:
            expression_statement();
            break;
    }
}

/**
 * <initializer> ::= <assignment expression>
 * */
void initializer(type_t *ptr_type) {
    if(ptr_type->t & TYPE_ARRAY)
        get_token();
    else
        assignment_expression();
}

/**
 * <function body> ::= <compound statement>
 * */
void function_body(symbol_t *ptr_symbol) {
    /* put an anonymous symbol in local symbol table */
    symbol_direct_push(&local_symbol_stack, SYMBOL_ANONYMOUS, &type_int, 0);
    compound_statement(NULL, NULL);

    pr_info("symbol pop ??\n");
    symbol_pop(&local_symbol_stack, NULL);
    pr_info("symbol pop\n");
}

/**
 * <parameter type list> ::= <parameter list> |
 *                           <parameter list><TOKEN_COMMA><TOKEN_ELLIPSE>
 *
 * <parameter list> ::= <parameter declaration>{<TOKEN_COMMA><parameter declaration>}
 *
 * <parameter declaration> ::= <type specifier>{<declarator>}
 * */
void parameter_type_list(type_t *ptr_type, int func_call) {
    int n;
    symbol_t **pptr_last, *ptr_symbol, *ptr_first;
    type_t t;

    get_token();
    ptr_first = NULL, pptr_last = &ptr_first;


    while(token != TOKEN_CLOSE_PARENTH) {
        if(!type_specifier(&t))
            error("Invalid identifier\n");
        declarator(&t, &n, NULL);
        ptr_symbol = symbol_push(n | SYMBOL_PARAMS, &t, 0, 0);
        *pptr_last = ptr_symbol;
        pptr_last = &ptr_symbol->next;

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

    /* function return */
    pr_info("push symbol\n");
    ptr_symbol = symbol_push(SYMBOL_ANONYMOUS, ptr_type, func_call, 0);
    ptr_symbol->next = ptr_first;
    ptr_type->t = TYPE_FUNC;
    ptr_type->ref = ptr_symbol;
    pr_info("push symbol complete\n");
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
void direct_declarator_postfix(type_t *ptr_type, int func_call) {
    int n;
    symbol_t *ptr_s;

    if(token == TOKEN_OPEN_PARENTH)
        parameter_type_list(ptr_type, func_call);
    else if(token == TOKEN_OPEN_BRACKET) {
        get_token();
        n = -1;
        if(token == TOKEN_CINT) {
            get_token();
            n = token_value;
        }

        skip(TOKEN_CLOSE_BRACKET);
        direct_declarator_postfix(ptr_type, func_call);
        ptr_s = symbol_push(SYMBOL_ANONYMOUS, ptr_type, 0, n);
        ptr_type->t = TYPE_ARRAY | TYPE_PTR;
        ptr_type->ref = ptr_s;
    }
}

/**
 * <direct declarator> ::= <TOKEN_KEY_IDENT><direct declarator postfix>
 * */
void direct_declarator(type_t *ptr_type, int *ptr_v, int func_call) {
    if(token >= TOKEN_KEY_IDENT) {
        pr_info("Is identifier\n");
        *ptr_v = token;
        get_token();
    }
    else
        error("expect identifier, current token = %d, fch = %x\n", token, fch);

    pr_info("declarator postfix\n");
    direct_declarator_postfix(ptr_type, func_call);
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
 * parameters:
 * ptr_type: data type
 * ptr_v: the word token
 * ptr_force_align: alignment
 *
 * in windows platform
 * <declarator> ::= {<TOKEN_ASTERISK>}[<function calling convention>][<struct member alignment>]<direct declarator>
 *
 * in linux or other unix like platform
 * <declarator> ::= {<TOKEN_ASTERISK>}<direct declarator>
 */
void declarator(type_t *ptr_type, int *ptr_v, int *ptr_force_align) {
    int fc;

    while(token == TOKEN_ASTERISK) {
        symbol_make_pointer(ptr_type);
        get_token();
    }

    if(ptr_force_align)
#ifndef _W32
        *ptr_force_align = 1;
#else
    struct_member_alignment();
#endif
    function_calling_convention(&fc);
    direct_declarator(ptr_type, ptr_v, fc);
}

/**
 * function: struct_declaration
 * parameter:
 *
 *
 * <struct_declaration> ::= <type specifier><struct_declaration_list><TOKEN_SEMICOLON>
 * <struct_declaration> ::= <type specifier><declarator>{<TOKEN_COMMA><declarator>}<TOKEN_SEMICOLON>
 */
void struct_declaration(int *ptr_max_align, int *ptr_offset, symbol_t ***ppptr_symbol) {

    int v, size, align, force_align;
    symbol_t *ptr_s;
    type_t type_to_get, type_to_push;

    /* in structure declaration, there are several types in the struct
     * example:
     * struct example {
     *  int a;
     *  int b;
     *  char c;
     * };
     *
     * then, we need to get all parameter type.
     * Thus, there is a type_specifier
     * */
    type_specifier(&type_to_get);
    while(1) {
        v = 0;
        type_to_push = type_to_get;
        declarator(&type_to_push, &v, &force_align);

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
void struct_declaration_list(type_t *ptr_type) {
    int max_align, offset;
    symbol_t *ptr_s, **pptr_s;

    ptr_s = ptr_type->ref;
    
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

    if(ptr_s->c != -1)
        error("structure is defined ");

    max_align = 1;
    pptr_s = &ptr_s->next;
    offset = 0;

    /* while the token is not '}', then it is still in structure definition */
    while(token != TOKEN_CLOSE_CURLY)
        struct_declaration(&max_align, &offset, &pptr_s);
    
    skip(TOKEN_CLOSE_CURLY);

    syntax_state = SYNTAX_NEWLINE_INDENT;
}

/**
 * function name: struct_specifier
 *
 * parse the delimiter
 *
 * <struct_specifier> ::= <TOKEN_KEY_STRUCT><TOKEN_KEY_IDENT><TOKEN_OPEN_CURLY><struct_declaration_list><TOKEN_CLOSE_CURLY> |
 *                        <TOKEN_KEY_STRUCT><TOKEN_KEY_IDENT>
 *
 * */
void struct_specifier(type_t *ptr_type) {
    int v;
    symbol_t *ptr_symbol;
    type_t symbol_type;

    /* Here, the token is specifier, the v is used to restore the specifier token */
    get_token();
    v = token;

    syntax_state = SYNTAX_DELAY;

    /* Here, the token should be open curly or close parentheses
     * There is two cases:
     * struct case1 {
     *      int aaa;
     * };
     *
     * case 2: sizeof(struct case2);
     * */
    get_token();

    if(token == TOKEN_OPEN_CURLY)
        syntax_state = SYNTAX_NEWLINE_INDENT;
    else if(token == TOKEN_CLOSE_PARENTH)
        syntax_state = SYNTAX_NULL;
    else
        syntax_state = SYNTAX_SPACE;
    
    syntax_indent();

    if(v < TOKEN_KEY_IDENT)
        error("Expect structure name\n");

    /* search the symbol in structure */
    ptr_symbol = symbol_search_struct(v);

    /* if the symbol is not found, then insert a symbol */
    if(!ptr_symbol) {
        symbol_type.t = TOKEN_KEY_STRUCT;

        /* assign ptr_symbol->c to -1 */
        ptr_symbol = symbol_push(v | SYMBOL_STRUCT, &symbol_type, 0, -1);
        ptr_symbol->r = 0;
    }

    ptr_type->t = TYPE_STRUCT;
    ptr_type->ref = ptr_symbol;
    
    if(token == TOKEN_OPEN_CURLY)
        struct_declaration_list(ptr_type);
}

/**
 * function name: type_specifier
 * description: parse delimiter, get the token type.
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
int type_specifier(type_t *ptr_type) {
    int t, type_found = 0;
    type_t type_in_struct;
    switch(token) {
        case TOKEN_KEY_CHAR:
            t = TYPE_CHAR;
            goto common;
        case TOKEN_KEY_SHORT:
            t = TYPE_SHORT;
            goto common;
        case TOKEN_KEY_INT:
            t = TYPE_INT;
            goto common;
        case TOKEN_KEY_VOID:
            t = TYPE_VOID;
common:
            type_found = 1;
            syntax_state = SYNTAX_SPACE;
            get_token();
            break;
        case TOKEN_KEY_STRUCT:
            type_found = 1;
            syntax_state = SYNTAX_SPACE;
            struct_specifier(&type_in_struct);
            ptr_type->ref = type_in_struct.ref;
            t = TYPE_STRUCT;
            break;
        default:
            break;
    }

    ptr_type->t = t;

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

    type_t btype, type;
    int v, r, addr;
    symbol_t *ptr_symbol;

    /**
     * There are three possible in the head of line,
     * 1. declaration, that is, type specifier
     * 2. statement, arithmetic, identifier, key word such as typedef, for, if, etc.
     * 3. preprocessor, in this parser, we do not need to consider this kind of situation
     * */
    pr_info("type specifier\n");
    if(!type_specifier(&btype)) {
        error("Expect a type specifier");
    }

    if (btype.t == TYPE_STRUCT && token == TOKEN_SEMICOLON) {
        get_token();
        return;
    }

    /**
     * if the declaration is structure,
     * it will be parsed as struct identifier
     * then it the open curly and close curly will not be
     * processed later, it will declare it in this function.
     * */
    pr_info("declarator\n");
    type = btype;
    declarator(&type, &v, NULL);

    if (token == TOKEN_OPEN_CURLY) {
        if (l == SYMBOL_LOCAL)
            error("Do not support nested define\n");

        if((type.t & TYPE_BTYPE) != TYPE_FUNC)
            error("Expect <Function Definition>\n");

        pr_info("symbol search\n");
        ptr_symbol = symbol_search(v);
        if(ptr_symbol) {
            if((type.t & TYPE_BTYPE) != TYPE_FUNC)
                error("function %s is redefinition\n", get_token_string(v));

            ptr_symbol->type = type;
        } else {
            ptr_symbol = symbol_function_push(v, &type);
        }

        pr_info("function body\n");
        ptr_symbol->r = SYMBOL_SYM | SYMBOL_GLOBAL;
        function_body(ptr_symbol);
    } else {
        /* function declaration */
        if((type.t & TYPE_BTYPE) == TYPE_FUNC) {
            pr_info("function declaration\n");
            if (symbol_search(v) == NULL)
                ptr_symbol = symbol_push(v, &type, SYMBOL_GLOBAL | SYMBOL_SYM, 0);
        } else {
            pr_info("variable declaration\n");
            r = 0;
            if(!(type.t & TYPE_ARRAY))
                r |= SYMBOL_LVALUE;

            r |= l;
            if(token == TOKEN_ASSIGN) {
                get_token();
                initializer(&type);
            }
            ptr_symbol = symbol_variable_push(&type, r, v, addr);
        }

        if (token == TOKEN_COMMA) {
            get_token();
        } else {
            syntax_state = SYNTAX_NEWLINE_INDENT;
            skip(TOKEN_SEMICOLON);
        }
    }
}

/**
 * <translation unit> ::= {<external declaration>}<TOKEN_EOF>
 * */
void translation_unit() {
    while(token != TOKEN_EOF) {
        external_declaration(SYMBOL_GLOBAL);
    }
}