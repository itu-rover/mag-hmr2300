#include <stdio.h>

#include <hmr2300.h>
#include <hmr2300/callbacks.h>

hmr2300_t hmr = HMR2300_NEW;

void hmr2300_log(const char* message) {
    printf("hmr2300: %s\n", message);
}

hmr2300_status_t hmr2300_read(hmr2300_t* dev, char* data, size_t size) {
    // FIXME
    return HMR2300_OK;
}

hmr2300_status_t hmr2300_write(hmr2300_t* dev, const char* data, size_t size) {
    // FIXME
    return HMR2300_OK;
}

int main(char** argv, int argc) {
    if (hmr2300_init(&hmr) != HMR2300_OK) {
        printf("Failed to initialize HMR2300 device\n");
        return -1;
    }

    printf("HMR2300 device initialized successfully\n");

    return 0;
}
