#ifndef __QARR_H__
#define __QARR_H__

typedef struct {
    int count;
    int capacity;
    void **data;
} qarr_t;

int qarr_init(qarr_t *ptr_qarr, int size);

int qarr_realloc(qarr_t *ptr_qarr, int size);

int qarr_add(qarr_t *ptr_qarr, void *data);

void qarr_free(qarr_t *ptr_qarr);

int qarr_search(qarr_t *ptr_qarr, int key);

#endif