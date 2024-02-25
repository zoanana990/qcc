#include <qstr.h>
#include <qerr.h>
#include <stdio.h>
#include <stdlib.h>

#define ALIGNMENT_SIZE          (2)
#define GET_ALIGNMENT(size)     (((size + ALIGNMENT_SIZE - 1) / ALIGNMENT_SIZE) * ALIGNMENT_SIZE)

int qstr_init(qstr_t *ptr_qstr, int size) {
    if(ptr_qstr == NULL) {
        ptr_qstr = malloc(sizeof(qstr_t));
        if(ptr_qstr == NULL)
            goto init_nomem;
    }

    ptr_qstr->data = malloc(sizeof(char) * size);
    if(ptr_qstr->data == NULL)
         goto data_nomem;

    ptr_qstr->count = 0;
    ptr_qstr->capacity = size;

    return QERR_PASS;

data_nomem:
    free(ptr_qstr);
init_nomem:
    return QERR_NO_MEMORY;
}

void qstr_free(qstr_t *ptr_qstr) {
    if(ptr_qstr == NULL)
        return;
    
    if(ptr_qstr->data)
        free(ptr_qstr->data);

    ptr_qstr->count = 0;
    ptr_qstr->capacity = 0;
    free(ptr_qstr);
}

void qstr_reset(qstr_t *ptr_qstr) {
    qstr_free(ptr_qstr);
    qstr_init(ptr_qstr, 8);
}

/* if ptr_qstr is NULL, the realloc is functional identical
 * with malloc. Therefore, it is same as qstr_realloc
 *
 * return value
 *  - QERR_BAD_PARAM: the ptr_qstr is NULL pointer
 */
int qstr_realloc(qstr_t *ptr_qstr, int size) {
    int      capacity;
    char    *data;

    if(ptr_qstr == NULL)
        return QERR_BAD_PARAM;

    capacity = GET_ALIGNMENT(size);

    data = realloc(ptr_qstr->data, capacity);
    if(data == NULL) {
        error("memory realloc failed\n");
        return QERR_NO_MEMORY;
    }
    
    ptr_qstr->capacity = capacity;
    ptr_qstr->data = data;
    return QERR_PASS;
}

int qstr_chcat(qstr_t *ptr_qstr, int ch) {
    int count, ret;

    count = ptr_qstr->count + 1;

    if(count > ptr_qstr->capacity) {
        ret = qstr_realloc(ptr_qstr, count);
        if(ret)
            goto realloc_failed;
    }

    ptr_qstr->data[count - 1] = ch;
    ptr_qstr->count = count;
    return QERR_PASS;

realloc_failed:
    error("realloc failed\n");
    return QERR_NO_MEMORY;
}