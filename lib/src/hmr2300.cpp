#include <stddef.h>

#include <hmr2300.h>
#include <hmr2300/callbacks.h>

hmr2300_status_t hmr2300_init(hmr2300_t* dev) {
    if (dev == NULL) {
        return HMR2300_ERROR;
    }

    hmr2300_log("Initializing HMR2300 device");
    if (auto status = hmr2300_write(dev, "*99#\r", 5); status != HMR2300_OK) {
        hmr2300_log("Failed to write initialization command to HMR2300 device");
        return status;
    }

    char response[22];
    if (auto status = hmr2300_read(dev, response, 22); status != HMR2300_OK) {
        hmr2300_log("Failed to read serial number from HMR2300 device");
        return status;
    }

    dev->initialized = true;
    return HMR2300_OK;
}
