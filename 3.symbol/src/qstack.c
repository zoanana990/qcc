#include <qstack.h>
#include <qerr.h>
#include <symbol.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void stack_dump(qstack_t *ptr_stack) {
    unsigned int times = ((uintptr_t)ptr_stack->top - (uintptr_t)ptr_stack->base) / sizeof(symbol_t);

    pr_info("--> CUT HERE, STACK DUMP \n");
    pr_info("ptr_stack->top = %p\n", ptr_stack->top);
    pr_info("ptr_stack->base = %p\n", ptr_stack->base);
    pr_info("ptr_stack->size = %d\n", ptr_stack->size);

    for(int i = 0; i < times; i++)
        pr_info("base[%d] = %p", i, ptr_stack->base + i);

    pr_info("-----------------------------------\n");
}

void stack_init(qstack_t *ptr_stack, int init_size) {
    ptr_stack->base = malloc(sizeof(void **) * init_size);
    if(!ptr_stack->base)
        error("Malloc failed\n");

    ptr_stack->top = ptr_stack->base;
    ptr_stack->size = init_size;
}

void *stack_push(qstack_t *ptr_stack, void *element, int size) {
    unsigned int new_size;

    pr_info("check stack status\n");
//    stack_dump(ptr_stack);
    /* if stack is overflow, we need to assign more space */
    if(ptr_stack->top >= ptr_stack->base + ptr_stack->size) {
        new_size = ptr_stack->size * 2;
        ptr_stack->base = realloc(ptr_stack->base, sizeof(void **) * new_size);

        if(!ptr_stack->base)
            return NULL;

        ptr_stack->top = ptr_stack->base + ptr_stack->size;
        ptr_stack->size = new_size;
    }

    *ptr_stack->top = malloc(size);
    memcpy(*ptr_stack->top, element, size);

    return *(ptr_stack->top++);
}

void stack_pop(qstack_t *stack) {
    if(stack->top > stack->base)
        free(*(--stack->top));
}

void *stack_top(qstack_t *ptr_stack) {
    return (ptr_stack->top > ptr_stack->base) ? *(ptr_stack->top - 1) : NULL;
}

int stack_isEmpty(qstack_t *ptr_stack) {
    return ptr_stack->top == ptr_stack->base;
}

void stack_free(qstack_t *ptr_stack) {
    void **ptr_element;

    for(ptr_element = ptr_stack->base; ptr_element < ptr_stack->top; ptr_element++)
        free(*ptr_element);

    if(ptr_stack->base)
        free(ptr_stack->base);

    ptr_stack->base = NULL;
    ptr_stack->top = NULL;
    ptr_stack->size = 0;
}