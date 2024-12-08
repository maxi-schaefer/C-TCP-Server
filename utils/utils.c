#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "utils.h"
#include "common.h"

void read_line(int socket, char *buffer, size_t size) {
    memset(buffer, 0, size);
    size_t bytes_read = 0;
    while (bytes_read < size - 1) {
        char c;
        ssize_t result = read(socket, &c, 1);
        if (result <= 0) break;
        if (c == '\n') break;
        buffer[bytes_read++] = c;
    }
    buffer[bytes_read] = '\0';
}

void clear(int socket) {
    const char *clear_screen_and_banner = "\x1b[2J\x1b[H" BANNER;
    send(socket, clear_screen_and_banner, strlen(clear_screen_and_banner), 0);
}

void log_msg(const char *message, ...) {
    time_t rawtime;
    struct tm *timeinfo;
    char time_str[9];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(time_str, sizeof(time_str), "%H:%M:%S", timeinfo);

    printf("[%s] " PREFIX " ", time_str);

    va_list args;
    va_start(args, message);

    vprintf(message, args);

    va_end(args);
    printf("\n" RESET);
}
