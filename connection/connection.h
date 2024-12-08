#pragma region

#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "common.h"

#define MAX_USERNAME_LEN 50

extern client_info *connections;
extern pthread_mutex_t connections_mutex;

void add_connection(client_info *info);
void remove_connection(int client_socket);
void list_connections(int client_socket);
void cleanup_connections();

#endif

#pragma endregion