#ifndef _HMR2300_H
#define _HMR2300_H

#include <stdint.h>

#include <hmr2300/state.h>
#include <hmr2300/status.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hmr2300_sample {
    uint64_t id;
    int16_t x;
    int16_t y;
    int16_t z;
} hmr2300_sample_t;

#define HMR2300_SAMPLE_ONESHOT ((uint64_t)(-1))

hmr2300_status_t hmr2300_init(hmr2300_t* dev, uint8_t sample_rate);
hmr2300_status_t hmr2300_sample_oneshot(hmr2300_t* dev, hmr2300_sample_t* sample);

#ifdef __cplusplus
}
#endif

#endif /* _HMR2300_H */
