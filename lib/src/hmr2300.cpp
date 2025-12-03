#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <hmr2300.h>
#include <hmr2300/callbacks.h>

/* Maybe only for debug builds? */
#define CHECK_INITIALIZED_OR_RETURN(dev, ret)     \
    do {                                          \
        if ((dev) == NULL || !(dev)->initialized) \
            return ret;                           \
    } while (0)

#define CHECK_INITIALIZED(dev) CHECK_INITIALIZED_OR_RETURN(dev, HMR2300_ERROR)
#define CHECK_INITIALIZED_VOID(dev) CHECK_INITIALIZED_OR_RETURN(dev, )

hmr2300_status_t read(hmr2300_t* dev, char* data, size_t size) {
    dev->busy = true;
    auto status = hmr2300_read(dev, data, size);
    if (status == HMR2300_ERROR)
        dev->busy = false;

    return status;
}

hmr2300_status_t write(hmr2300_t* dev, const char* data, size_t size) {
    dev->busy = true;
    auto status = hmr2300_write(dev, data, size);
    if (status == HMR2300_ERROR)
        dev->busy = false;

    return status;
}

void poll(hmr2300_t* dev) {
    while (dev->busy) {
        // FIXME
    }
}

hmr2300_status_t read_poll(hmr2300_t* dev, char* data, size_t size) {
    if (auto status = read(dev, data, size); status != HMR2300_OK)
        return status;

    poll(dev);
    return HMR2300_OK;
}

hmr2300_status_t write_poll(hmr2300_t* dev, const char* data, size_t size) {
    if (auto status = write(dev, data, size); status != HMR2300_OK)
        return status;

    poll(dev);
    return HMR2300_OK;
}

hmr2300_status_t hmr2300_init(hmr2300_t* dev) {
    if (dev == NULL || dev->initialized) {
        return HMR2300_ERROR;
    }

    hmr2300_log("Initializing HMR2300 device");
    if (auto status = write_poll(dev, "*99#\r", 5); status != HMR2300_OK) {
        hmr2300_log("Failed to write initialization command to HMR2300 device");
        return status;
    }

    char response[22];
    if (auto status = read_poll(dev, response, 22); status != HMR2300_OK) {
        hmr2300_log("Failed to read serial number from HMR2300 device");
        return status;
    }

    if (memcmp(response, "SER# ", 5)) {
        hmr2300_log("Invalid response received from HMR2300 device");
        return HMR2300_ERROR;
    }

    strncpy(dev->serial, response + 5, 16);

    char log_msg[64];
    snprintf(log_msg, sizeof(log_msg), "HMR2300 serial number: %s", dev->serial);
    hmr2300_log(log_msg);

    dev->initialized = true;
    return HMR2300_OK;
}

void hmr2300_read_complete(hmr2300_t* dev) {
    CHECK_INITIALIZED_VOID(dev);
    dev->busy = false;
}

void hmr2300_write_complete(hmr2300_t* dev) {
    CHECK_INITIALIZED_VOID(dev);
    dev->busy = false;
}
