#include <qerr.h>
#include <vector.h>
#include <stdlib.h>
#include <stdio.h>
#include <token.h>

int vector_init(qvector_t *ptr_qarr, int size) {

    ptr_qarr->data = malloc(sizeof(void *) * size);
    if (ptr_qarr->data == NULL)
        goto data_nomem;

    ptr_qarr->count = 0;
    ptr_qarr->capacity = size;

    return QERR_PASS;

    data_nomem:
    free(ptr_qarr);
    init_nomem:
    return QERR_NO_MEMORY;
}

void vector_free(qvector_t *ptr_qarr) {
    void **p;
    if (ptr_qarr == NULL)
        return;

    for (p = ptr_qarr->data; ptr_qarr->count;
         p++, ptr_qarr->count--)
        if (*p)
            free(*p);

    ptr_qarr->count = 0;
    ptr_qarr->capacity = 0;
    free(ptr_qarr->data);
    ptr_qarr->data = NULL;
}

int vector_realloc(qvector_t *ptr_qarr, int size) {
    int capacity = GET_ALIGNMENT(size);
    void *data = realloc(ptr_qarr->data, capacity);

    if (data == NULL) {
        error("memory realloc failed\n");
        return QERR_NO_MEMORY;
    }

    ptr_qarr->capacity = capacity;
    ptr_qarr->data = data;
    return QERR_PASS;
}

int vector_add(qvector_t *ptr_qarr, void *data) {
    int count = ptr_qarr->count + 1;
    int size = count * sizeof(void *);
    int ret;

    if (size > ptr_qarr->capacity) {
        ret = vector_realloc(ptr_qarr, size);
        if (ret)
            goto realloc_failed;
    }

    ptr_qarr->data[count - 1] = data;
    ptr_qarr->count = count;
    return QERR_PASS;

    realloc_failed:
    error("realloc failed\n");
    return QERR_NO_MEMORY;
}

int vector_search(qvector_t *ptr_qarr, int key) {
    int i, **p;
    p = (int **) ptr_qarr->data;

    for (int i = 0; i < ptr_qarr->count; p++)
        if (key == **p)
            return i;

    return -1;
}