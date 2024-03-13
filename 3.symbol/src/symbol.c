#include <symbol.h>
#include <qstack.h>
#include <token.h>
#include <vector.h>
#include <qerr.h>
#include <lexer.h>

#include <stdio.h>

/**
 * In this file, we need to record each symbol in the code we get.
 * Thus, we need to change the
 * */
qstack_t global_symbol_stack, local_symbol_stack;

extern qvector_t token_table;
extern int token;
/**
 * Description: push the symbol to symbol stack
 *
 * Argument:
 * ptr_qs: stack
 * v: symbol number
 * ptr_type: symbol type
 * c: symbol relation
 * */
symbol_t *symbol_direct_push(qstack_t *ptr_qs, int v, type_t *ptr_type, int c) {
    symbol_t s;
    s.v = v;
    s.type.t = ptr_type->t;
    s.type.ref = ptr_type->ref;
    s.c = c;
    s.next = NULL;
    return stack_push(ptr_qs, &s, sizeof(symbol_t));
}

/**
 * Description: We need to check the symbol is needed to be write to global symbol stack
 * or local symbol stack
 *
 * Argument:
 * v: symbol number
 * ptr_type: symbol type
 * r: restore type
 * c: symbol relation
 * */
symbol_t *symbol_push(int v, type_t *ptr_type, int r, int c) {
    symbol_t *ptr_s, **pptr_s;
    token_t *ptr_ts;
    qstack_t *ptr_ss;

    if(stack_isEmpty(&local_symbol_stack))
        ptr_ss = &local_symbol_stack;
    else
        ptr_ss = &global_symbol_stack;

    ptr_s = symbol_direct_push(ptr_ss, v, ptr_type, c);
    ptr_s->r = r;

    /* We do not store the anonymous symbol and structure member */
    if((v & S_STRUCT) || v < S_ANONYMOUS) {
        /* we need to update the word */
        ptr_ts = token_table.data[(v & ~S_STRUCT)];

        /* update the token table */
        if(v & S_STRUCT)
            pptr_s = &ptr_ts->sym_struct;
        else
            pptr_s = &ptr_ts->sym_identifier;

        ptr_s->prev = *pptr_s;
        *pptr_s = ptr_s;
    }

    return ptr_s;
}

symbol_t *symbol_function_push(int v, type_t *ptr_type) {
    symbol_t *ptr_s, **pptr_s;

    ptr_s = symbol_direct_push(&global_symbol_stack, v, ptr_type, 0);
    pptr_s = &((token_t *) token_table.data[v])->sym_identifier;

    while(*pptr_s)
        pptr_s = &(*pptr_s)->prev;

    ptr_s->prev = NULL;
    *pptr_s = ptr_s;

    return ptr_s;
}

symbol_t *symbol_variable_push(type_t *type, int r, int v, int addr) {
    symbol_t *sym = NULL;

    if((r & S_VAL_MASK) == S_LOCAL)
        sym = symbol_push(v, type, r, addr);
    else if((r & S_VAL_MASK) == S_GLOBAL) {
        sym = symbol_search(v);

        if(sym)
            error("redefine variable, %s\n", ((token_t *)token_table.data[v])->spelling);

        sym = symbol_push(v, type, r | S_SYMBOL, 0);
    }

    return sym;
}

symbol_t *symbol_put_section(char *section, int c) {
    token_t *tp;
    symbol_t *s;
    type_t t;
    t.t = TYPE_INT;
    tp = token_insert(section);
    token = tp->token_code;
    s = symbol_push(token, &t, S_GLOBAL, c);
    return s;
}

symbol_t *symbol_search_struct(int v) {
    return (v >= token_table.count) ? NULL: ((token_t *)token_table.data[v])->sym_struct;
}

symbol_t *symbol_search(int v) {
    return (v >= token_table.count) ? NULL: ((token_t *)token_table.data[v])->sym_identifier;
}

void symbol_pop(qstack_t *ptr_top, symbol_t *b) {
    symbol_t *ptr_s, **pptr_s;
    token_t *ptr_ts;
    int v;

    ptr_s = stack_top(ptr_top);
    while(ptr_s != b) {
        v = ptr_s->v;

        /* update the word list */
        if((v & S_STRUCT) || v < S_ANONYMOUS) {
            /* we need to update the word */
            ptr_ts = token_table.data[(v & ~S_STRUCT)];

            /* update the token table */
            if(v & S_STRUCT)
                pptr_s = &ptr_ts->sym_struct;
            else
                pptr_s = &ptr_ts->sym_identifier;

            *pptr_s = ptr_s->prev;
        }

        stack_pop(ptr_top);
        ptr_s = stack_top(ptr_top);
    }
}

