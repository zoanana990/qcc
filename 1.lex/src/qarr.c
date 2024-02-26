#include <qerr.h>
#include <qarr.h>
#include <stdlib.h>
#include <stdio.h>
#include <qcc.h>

int qarr_init(qarr_t *ptr_qarr, int size) {

    ptr_qarr->data = malloc(sizeof(void *) * size);
    if(ptr_qarr->data == NULL)
         goto data_nomem;

    ptr_qarr->count = 0;
    ptr_qarr->capacity = size;

    return QERR_PASS;

data_nomem:
    free(ptr_qarr);
init_nomem:
    return QERR_NO_MEMORY;
}

void qarr_free(qarr_t *ptr_qarr) {
    void **p;
    if(ptr_qarr == NULL)
        return;

    for(p = ptr_qarr->data; ptr_qarr->count; 
        p++, ptr_qarr->count--)
        if(*p)
            free(*p);

    ptr_qarr->count     = 0;
    ptr_qarr->capacity  = 0;
    free(ptr_qarr->data);
    ptr_qarr->data      = NULL;
    free(ptr_qarr);
}

int qarr_realloc(qarr_t *ptr_qarr, int size) {
    int capacity = GET_ALIGNMENT(size);
    void *data = realloc(ptr_qarr->data, capacity);

    if(data == NULL) {
        error("memory realloc failed\n");
        return QERR_NO_MEMORY;
    }

    ptr_qarr->capacity = capacity;
    ptr_qarr->data = data;
    return QERR_PASS;
}

int qarr_add(qarr_t *ptr_qarr, void *data) {
    int count = ptr_qarr->count + 1;
    int size = count * sizeof(void *);
    int ret;

    if(size > ptr_qarr->capacity) {
        ret = qarr_realloc(ptr_qarr, size);
        if(ret)
            goto realloc_failed;
    }

    ptr_qarr->data[count - 1] = data;
    ptr_qarr->count = count;
    return QERR_PASS;

realloc_failed:
    error("realloc failed\n");
    return QERR_NO_MEMORY;
}

int qarr_search(qarr_t *ptr_qarr, int key) {
    int i, **p;
    p = (int **) ptr_qarr->data;

    for(int i = 0; i < ptr_qarr->count; p++)
        if(key == **p)
            return i;

    return -1;
}