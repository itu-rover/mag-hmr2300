#ifndef _HMR2300_HANDLE_H
#define _HMR2300_HANDLE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hmr2300_state {
    char serial[16];
    size_t in_progress_read_size;
    char* in_progress_read_buffer;
    bool initialized;
    bool busy;
} hmr2300_t;

#define HMR2300_NEW { .serial = {0}, .in_progress_read_size = 0, \
                      .in_progress_read_buffer = NULL, .initialized = false, \
                      .busy = false }

#ifdef __cplusplus
}
#endif

#endif /* _HMR2300_HANDLE_H */
