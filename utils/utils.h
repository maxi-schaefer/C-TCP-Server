#ifndef UTILS_H
#define UTILS_H

#include <sys/socket.h>
#include <stddef.h>

void read_line(int socket, char *buffer, size_t size);
void clear(int socket);
void log_msg(const char *message, ...);

#endif
