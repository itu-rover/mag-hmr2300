#ifndef _HMR2300_STATUS_H
#define _HMR2300_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum hmr2300_status {
    HMR2300_OK = 0,
    HMR2300_BUSY,
    HMR2300_ERROR,
    HMR2300_TIMEOUT,
} hmr2300_status_t;

#ifdef __cplusplus
}
#endif

#endif /* _HMR2300_STATUS_H */
