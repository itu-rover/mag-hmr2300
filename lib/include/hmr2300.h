#ifndef _HMR2300_H
#define _HMR2300_H

#include <stdint.h>

#include <hmr2300/state.h>
#include <hmr2300/status.h>

#ifdef __cplusplus
extern "C" {
#endif

hmr2300_status_t hmr2300_init(hmr2300_t* dev, uint8_t sample_rate);

#ifdef __cplusplus
}
#endif

#endif /* _HMR2300_H */
