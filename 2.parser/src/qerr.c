#include <qerr.h>
#include <qlex.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

extern int token;

void handle_exception(int stage, int level, char *fmt, va_list ap) {
    char buf[1024];
    vsprintf(buf, fmt, ap);
    if (stage == QSTG_COMPILE) {
        if (level == QLVL_WARNING) {
            printf("\n\n%s (line %d) warning: %s", __FILE__, __LINE__, buf);
        } else if (level == QLVL_ERROR) {
            printf("\n\n%s (line %d) compile error: %s", __FILE__, __LINE__, buf);
            exit(-1);
        } else if (level == QLVL_DEBUG) {
            printf("[DEBUG] %s (line %d): %s", __FILE__, __LINE__, buf);
        }
    } else {
        printf("link error: %s\n", buf);
        exit(-1);
    }
}

void warning(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    handle_exception(QSTG_COMPILE, QLVL_WARNING, fmt, ap);
    va_end(ap);
}

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    handle_exception(QSTG_COMPILE, QLVL_ERROR, fmt, ap);
    va_end(ap);
}

void skip(int v) {
    if(token != v)
        error("missing character '%s\n", get_token_string(v));
    get_token();
}
