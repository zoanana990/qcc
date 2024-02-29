#ifndef __QERR_H__
#define __QERR_H__

#include <stdarg.h>
/**
 * error code
 * */
enum {
    QERR_PASS,
    QERR_ALREADY_INIT,
    QERR_NOT_INIT,
    QERR_NO_MEMORY,
    QERR_BAD_PARAM,
    QERR_OP_FAIL,
};

/**
 * error level
 * */
enum {
    QLVL_ERROR,
    QLVL_WARNING,
    QLVL_DEBUG,
};

/**
 * error stage
 * */
enum {
    QSTG_COMPILE,
    QSTG_LINK,
};

void handle_exception(int stage, int level, char *fmt, va_list ap);
void warning(char *fmt, ...);
void error(char *fmt, ...);

#define pr_info(s, ...)     printf("[%s, line %d] " s, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#endif