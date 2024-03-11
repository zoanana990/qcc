#ifndef __QSTACK_H__
#define __QSTACK_H__

typedef struct stack {
    void            **base;
    void            **top;
    unsigned int    size;
} qstack_t;

void stack_init(qstack_t *ptr_stack, int init_size);
void *stack_push(qstack_t *ptr_stack, void *element, int size);
void *stack_top(qstack_t *ptr_stack);
int stack_isEmpty(qstack_t *ptr_stack);
void stack_free(qstack_t *ptr_stack);

#endif //__QSTACK_H__
