#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

#include <thread>
#include <chrono>
#include <ctime>

#include <hmr2300.h>
#include <hmr2300/callbacks.h>

hmr2300_t hmr = HMR2300_NEW;

#define TX_BUFFER_SIZE 512
#define RX_BUFFER_SIZE 512

uint8_t tx_buffer[TX_BUFFER_SIZE];
uint8_t rx_buffer[RX_BUFFER_SIZE];

size_t rx_head = 0;
size_t rx_tail = 0;
size_t tx_head = 0;
size_t tx_tail = 0;

void hmr2300_log(const char* message) {
    printf("hmr2300: %s\n", message);
}

hmr2300_status_t hmr2300_read(hmr2300_t* dev, char* data, size_t size) {
    size_t read = 0;
    // lock();
    while (read < size && rx_tail != rx_head) {
        data[read++] = rx_buffer[rx_tail];
        rx_tail = (rx_tail + 1) % RX_BUFFER_SIZE;
    }
    // unlock();

    if (read == size) {
        hmr2300_read_complete(dev);
        return HMR2300_OK;
    }
    dev->in_progress_read_buffer = data + read;
    dev->in_progress_read_size = size - read;

    return HMR2300_BUSY;
}

hmr2300_status_t hmr2300_write(hmr2300_t* dev, const char* data, size_t size) {
    size_t written = 0;
    // lock();
    while (written < size) {
        size_t next = (tx_head + 1) % TX_BUFFER_SIZE;
        if (next == tx_tail) {
            // Buffer full
            break;
        }
        tx_buffer[tx_head] = data[written++];
        tx_head = next;
    }
    // unlock();

    return HMR2300_OK;
}

void event_loop(const char* serial_port_path) {
    auto serial_port = open(serial_port_path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (serial_port < 0) {
        perror("Failed to open serial port");
        return;
    }

    struct termios tty;
    if (tcgetattr(serial_port, &tty)) {
        perror("tcgetattr failed");
        return;
    }

    // Set baud rate (e.g., 9600)
    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);

    // Configure data bits (8 bits), parity (none), stop bits (1)
    tty.c_cflag &= ~PARENB;   // No parity
    tty.c_cflag &= ~CSTOPB;   // 1 stop bit
    tty.c_cflag &= ~CSIZE;    // Clear size bits
    tty.c_cflag |= CS8;       // 8 data bits
    tty.c_cflag |= CLOCAL;    // Ignore modem control lines
    tty.c_cflag |= CREAD;     // Enable receiver

    // Disable canonical mode (raw input) and echo
    tty.c_lflag &= ~ICANON;   // Disable canonical mode (no line buffering)
    tty.c_lflag &= ~ECHO;     // Disable echo
    tty.c_lflag &= ~ECHOE;    // Disable erasure
    tty.c_lflag &= ~ISIG;     // Disable signals (e.g., Ctrl+C)

    // Disable software flow control
    tty.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL);

    // Read timeout: wait up to 10 deciseconds (1 second) for data
    tty.c_cc[VTIME] = 0; // 10 * 0.1s = 1s
    tty.c_cc[VMIN] = 0;   // Return immediately if no data (nonblocking read)

    // Apply settings immediately
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        perror("tcsetattr failed");
        return;
    }

    fd_set read_fds;
    fd_set write_fds;
    struct timeval timeout;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(serial_port, &read_fds);
        FD_ZERO(&write_fds);

        size_t bytes_to_write;
        if (tx_head >= tx_tail)
            bytes_to_write = tx_head - tx_tail;
        else
            bytes_to_write = TX_BUFFER_SIZE - tx_tail;

        if (bytes_to_write > 0) {
            FD_SET(serial_port, &write_fds);
        }

        timeout.tv_sec = 0;
        timeout.tv_usec = 50000;

        int ret = select(serial_port + 1, &read_fds, &write_fds, NULL, &timeout);
        if (ret < 0) {
            perror("select error");
            break;
        }

        if (FD_ISSET(serial_port, &write_fds)) {
            auto bytes_written = write(serial_port, &tx_buffer[tx_tail], bytes_to_write);

            if (bytes_written < 0 && (errno != EAGAIN || errno != EWOULDBLOCK)) {
                printf("TTY write error\n");
                break;
            } else if (bytes_written > 0) {
                tx_tail = (tx_tail + bytes_written) % TX_BUFFER_SIZE;

                if (bytes_written == bytes_to_write && tx_tail == tx_head) {
                    hmr2300_write_complete(&hmr);
                }
            }
        } else if (FD_ISSET(serial_port, &read_fds)) {
            uint8_t tmp;
            auto bytes_read = read(serial_port, &tmp, 1);
            if (bytes_read < 0 && (errno != EAGAIN || errno != EWOULDBLOCK)) {
                printf("TTY read error\n");
                break;
            } else if (bytes_read > 0) {
                size_t next = (rx_head + 1) % RX_BUFFER_SIZE;
                if (next != rx_tail) {
                    rx_buffer[rx_head] = tmp;
                    rx_head = next;
                }
            }
        }

        size_t bytes_readable;
        if (rx_head >= rx_tail)
            bytes_readable = rx_head - rx_tail;
        else
            bytes_readable = RX_BUFFER_SIZE - rx_tail + rx_head;

        if (bytes_readable > 0 && hmr.in_progress_read_size > 0 && hmr.in_progress_read_buffer != NULL) {
            size_t to_copy = (bytes_readable < hmr.in_progress_read_size) ? bytes_readable : hmr.in_progress_read_size;
            for (size_t i = 0; i < to_copy; ++i) {
                hmr.in_progress_read_buffer[i] = rx_buffer[rx_tail];
                rx_tail = (rx_tail + 1) % RX_BUFFER_SIZE;
            }
            hmr.in_progress_read_buffer += to_copy;
            hmr.in_progress_read_size -= to_copy;

            if (hmr.in_progress_read_size == 0) {
                hmr2300_read_complete(&hmr);
            }
        }
    }

    close(serial_port);
    return;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <serial-port>\n", argv[0]);
        return -1;
    }

    std::thread event_loop_thread(event_loop, argv[1]);
    event_loop_thread.detach();

    std::this_thread::sleep_for(std::chrono::seconds(3));
    if (hmr2300_init(&hmr, 50) != HMR2300_OK) {
        printf("Failed to initialize HMR2300 device\n");
        return -1;
    }

    printf("HMR2300 device initialized successfully\n");

    while (1) {
        hmr2300_sample_t sample;
        if (hmr2300_sample_oneshot(&hmr, &sample) == HMR2300_OK) {
            auto now = std::chrono::system_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
            std::time_t t = std::chrono::system_clock::to_time_t(now);
            std::tm tm;
            localtime_r(&t, &tm);
            char buf[64];
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
            printf("%s.%03d %6hd %6hd %6hd\n", buf, (int)ms.count(), sample.x, sample.y, sample.z);
        } else {
            printf("Failed to get sample\n");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    return 0;
}
