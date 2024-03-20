#ifndef __UTILS_H__
#define __UTILS_H__

#define MAX_KEY                  (1024)
#define GET_ALIGNMENT(size)      ((size_t)1 << (64 - __builtin_clzll((size_t)(size) - 1)))
#define calc_align(n, align)     (((n) + (align) - 1) & (~((align) - 1)))

#endif