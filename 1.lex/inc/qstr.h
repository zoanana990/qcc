#ifndef __QSTR_H__
#define __QSTR_H__

typedef struct qstr {
    int     count;          /* length of data */
    int     capacity;       /* buffer */
    char   *data;           /* data array */
} qstr_t;

int  qstr_init(qstr_t *ptr_qstr, int size);
void qstr_free(qstr_t *ptr_qstr);
void qstr_reset(qstr_t *ptr_qstr);
int  qstr_chcat(qstr_t *ptr_qstr, int ch);
int  qstr_realloc(qstr_t *ptr_qstr, int size);

#endif