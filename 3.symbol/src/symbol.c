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
type_t type_int, default_func_type;
symbol_t symbol_section_rdata;


extern qvector_t token_table;
extern int token;

void symbol_dump(symbol_t *ptr_symbol) {
    pr_info("--> CUT HERE, SYMBOL DUMP \n");
    if(ptr_symbol->v < token_table.count)
        pr_info("symbol name: %s\n", ((token_t *)token_table.data[ptr_symbol->v])->spelling);
    else
        pr_info("Cannot get symbol name, v = %d, count = %u\n", ptr_symbol->v, token_table.count);
    pr_info("ptr_symbol->v = %d, token = %d\n", ptr_symbol->v, token);
    pr_info("ptr_symbol->c = %d\n", ptr_symbol->c);
    pr_info("ptr_symbol->type.t = %d\n", ptr_symbol->type.t);
    pr_info("-----------------------------------\n");
}

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
    pr_info("stack push\n");
//    symbol_dump(&s);
//    pr_info("%s\n", ((token_t *)token_table.data[v])->spelling);
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

/**
 * BUG HERE
 * */
symbol_t *symbol_push(int v, type_t *ptr_type, int r, int c) {
    symbol_t *ptr_s, **pptr_s;
    token_t *ptr_ts;
    qstack_t *ptr_ss;

    pr_info("check stack is empty\n");
    if(!stack_isEmpty(&local_symbol_stack)) {
        pr_info("local symbol\n");
        ptr_ss = &local_symbol_stack;
    }
    else {
        pr_info("global symbol\n");
        ptr_ss = &global_symbol_stack;
    }

    pr_info("push symbol directly\n");
    ptr_s = symbol_direct_push(ptr_ss, v, ptr_type, c);
    ptr_s->r = r;

    pr_info("check token type\n");
    /* We do not store the anonymous symbol and structure member */
    if((v & SYMBOL_STRUCT) || v < SYMBOL_ANONYMOUS) {
        /* we need to update the word */
        ptr_ts = token_table.data[(v & ~SYMBOL_STRUCT)];

        /* update the token table */
        if(v & SYMBOL_STRUCT)
            pptr_s = &ptr_ts->sym_struct;
        else
            pptr_s = &ptr_ts->sym_identifier;

        ptr_s->prev = *pptr_s;
        *pptr_s = ptr_s;
    }
    pr_info("return\n");

    return ptr_s;
}

symbol_t *symbol_function_push(int v, type_t *ptr_type) {
    symbol_t *ptr_s, **pptr_s;

    ptr_s = symbol_direct_push(&global_symbol_stack, v, ptr_type, 0);
    pptr_s = &((token_t *) token_table.data[v])->sym_identifier;
    pr_info("%s\n", ((token_t *)token_table.data[v])->spelling);

    while(*pptr_s)
        pptr_s = &(*pptr_s)->prev;

    ptr_s->prev = NULL;
    *pptr_s = ptr_s;

    return ptr_s;
}

symbol_t *symbol_variable_push(type_t *type, int r, int v, int addr) {
    symbol_t *sym = NULL;

    if((r & SYMBOL_VAL_MASK) == SYMBOL_LOCAL)
        sym = symbol_push(v, type, r, addr);
    else if((r & SYMBOL_VAL_MASK) == SYMBOL_GLOBAL) {
        sym = symbol_search(v);

        if(sym)
            error("redefine variable, %s\n", ((token_t *)token_table.data[v])->spelling);

        sym = symbol_push(v, type, r | SYMBOL_SYM, 0);
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
    s = symbol_push(token, &t, SYMBOL_GLOBAL, c);
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

    pr_info("stack top\n");
    ptr_s = stack_top(ptr_top);
    symbol_dump(ptr_s);

    while(ptr_s != b) {
        v = ptr_s->v;

        pr_info("update list\n");
        /* update the word list */
        if((v & SYMBOL_STRUCT) || (v < SYMBOL_ANONYMOUS)) {
            pr_info("v = 0x%x\n", v);

            /* we need to update the word */
            ptr_ts = (token_t *)token_table.data[(v & ~SYMBOL_STRUCT)];

            /* update the token table */
            pr_info("update table\n");
            if(v & SYMBOL_STRUCT)
                pptr_s = &ptr_ts->sym_struct;
            else
                pptr_s = &ptr_ts->sym_identifier;

            *pptr_s = ptr_s->prev;
        }

        pr_info("stack pop\n");
        stack_pop(ptr_top);
        pr_info("stack top\n");
        ptr_s = stack_top(ptr_top);
    }
}

void symbol_make_pointer(type_t *ptr_t) {
    symbol_t *ptr_s;
    ptr_s = symbol_push(SYMBOL_ANONYMOUS, ptr_t, 0, -1);

    /* the token is star(*) */
    ptr_t->t = TOKEN_ASTERISK;
    ptr_t->ref = ptr_s;
}

int type_size(type_t *ptr_type, int *ptr_align) {
    symbol_t *ptr_symbol;
    int btype;

    int ptr_size = 4;

    btype = ptr_type->t & TYPE_BTYPE;
    switch(btype) {
        case TYPE_STRUCT:
            ptr_symbol = ptr_type->ref;
            *ptr_align = ptr_symbol->r;
            return ptr_symbol->c;
        case TYPE_PTR:
            if(ptr_type->t & TYPE_ARRAY) {
                ptr_symbol = ptr_type->ref;
                return type_size(&ptr_symbol->type, ptr_align) * ptr_symbol->c;
            } else {
                *ptr_align = ptr_size;
                return ptr_size;
            }
        case TYPE_INT:
            *ptr_align = SIZE_INT;
            return SIZE_INT;
        case TYPE_SHORT:
            *ptr_align = SIZE_SHORT;
            return SIZE_SHORT;
        case TYPE_VOID:
            *ptr_align = SIZE_VOID;
            return SIZE_VOID;
        case TYPE_CHAR:
        default:
            *ptr_align = SIZE_CHAR;
            return SIZE_CHAR;
    }
}
