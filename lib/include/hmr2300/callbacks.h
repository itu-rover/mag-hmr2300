#ifndef _HMR2300_CALLBACKS_H
#define _HMR2300_CALLBACKS_H

#include <stdint.h>
#include <hmr2300/status.h>

#ifdef __cplusplus
extern "C" {
#endif

hmr2300_status_t hmr2300_write(hmr2300_t* dev, const char* data, uint16_t size);
hmr2300_status_t hmr2300_read(hmr2300_t* dev, const char* data, uint16_t size);

void hmr2300_log(const char* message);

#ifdef __cplusplus
}
#endif

#endif /* _HMR2300_CALLBACKS_H */
