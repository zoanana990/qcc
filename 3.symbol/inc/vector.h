#ifndef __VECTOR_H__
#define __VECTOR_H__

typedef struct {
    int count;
    int capacity;
    void **data;
} qvector_t;

int vector_init(qvector_t *ptr_qarr, int size);

int vector_realloc(qvector_t *ptr_qarr, int size);

int vector_add(qvector_t *ptr_qarr, void *data);

void vector_free(qvector_t *ptr_qarr);

int vector_search(qvector_t *ptr_qarr, int key);

#endif