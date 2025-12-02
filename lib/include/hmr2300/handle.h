#ifndef _HMR2300_HANDLE_H
#define _HMR2300_HANDLE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hmr2300 {
    char serial[16];
    bool initialized;
} hmr2300_t;

#define HMR2300_NEW { .initialized = false }

#ifdef __cplusplus
}
#endif

#endif /* _HMR2300_HANDLE_H */
