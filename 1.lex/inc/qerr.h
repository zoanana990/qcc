#ifndef __QERR_H__
#define __QERR_H__

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

#endif