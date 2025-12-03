#include <string.h>

#include <hmr2300/callbacks.h>
#include <hmr.h>
#include <main.h>

hmr2300_t hmr = HMR2300_NEW;

void hmr2300_log(const char* message) {
    HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), 1000);
}

hmr2300_status_t hmr2300_read(hmr2300_t* dev, char* data, size_t size) {
    switch (HAL_UART_Receive(&huart4, (uint8_t*)data, size, HAL_MAX_DELAY)) {
    case HAL_OK:
        break;
    case HAL_BUSY:
        return HMR2300_BUSY;
    default:
        return HMR2300_ERROR;
    }

    hmr2300_read_complete(dev);
    return HMR2300_OK;
}

hmr2300_status_t hmr2300_write(hmr2300_t* dev, const char* data, size_t size) {
    switch (HAL_UART_Transmit(&huart4, (uint8_t*)data, size, 1000)) {
    case HAL_OK:
        break;
    case HAL_BUSY:
        return HMR2300_BUSY;
    default:
        return HMR2300_ERROR;
    }

    hmr2300_write_complete(dev);
    return HMR2300_OK;
}
