#include <qstack.h>
#include <qerr.h>

#include <stdlib.h>
#include <string.h>

void stack_init(qstack_t *ptr_stack, int init_size) {
    ptr_stack->base = malloc(sizeof(void **) * init_size);
    if(!ptr_stack->base)
        error("Malloc failed\n");

    ptr_stack->top = ptr_stack->base;
    ptr_stack->size = init_size;
}

void *stack_push(qstack_t *ptr_stack, void *element, int size) {
    int new_size;

    /* if stack is overflow, we need to assign more space */
    if(ptr_stack->top >= ptr_stack->base + ptr_stack->size) {
        new_size = ptr_stack->size * 2;
        ptr_stack->base = realloc(ptr_stack->base, sizeof(void **) * new_size);

        if(!ptr_stack)
            return NULL;

        ptr_stack->top = ptr_stack->base + ptr_stack->size;
        ptr_stack->size = new_size;
    }

    *ptr_stack->top = malloc(size);
    memcpy(ptr_stack->top, element, size);
    return *(ptr_stack->top++);
}

void *stack_top(qstack_t *ptr_stack) {
    return (ptr_stack->top > ptr_stack->base) ? (ptr_stack->top - 1) : NULL;
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