#include "auth.h"
#include "commands.h"
#include "utils.h"
#include "connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

void *handle_client(void *arg) {
    int new_socket = *((int *)arg);
    free(arg);  // Free the allocated memory for socket after usage

    char username[50], password[50], role[50];
    send(new_socket, BRIGHT_RED "Username" GRAY ": " RESET, strlen(BRIGHT_RED "Username" GRAY ": " RESET), 0);
    memset(username, 0, sizeof(username));
    read_line(new_socket, username, sizeof(username));

    send(new_socket, BRIGHT_RED "Password" GRAY ": " RESET, strlen(BRIGHT_RED "Password" GRAY ": " RESET), 0);
    memset(password, 0, sizeof(password));
    read_line(new_socket, password, sizeof(password));

    username[strcspn(username, "\r\n")] = 0;
    password[strcspn(password, "\r\n")] = 0;

    if (!authenticate_user(username, password, role)) {
        const char *error_msg = RED "Authentication failed. Disconnecting.\n";
        send(new_socket, error_msg, strlen(error_msg), 0);
        close(new_socket);
        return NULL;
    }

    clear(new_socket);
    log_msg(BRIGHT_GREEN "New Connection established. User: %s, Role: %s" RESET, username, role);

    // Allocate and initialize the client_info struct
    client_info *info = malloc(sizeof(client_info));
    if (!info) {
        perror(RED "Failed to allocate memory for client_info");
        close(new_socket);
        return NULL;
    }
    memset(info, 0, sizeof(client_info));
    info->client_socket = new_socket;
    strcpy(info->username, username);
    strcpy(info->role, role);
    info->connection_time = time(NULL);
    info->next = NULL;

    add_connection(info);

    handle_commands(info);

    remove_connection(new_socket);
    close(new_socket);
    return NULL;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror(RED "Socket Failed!");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror(RED "Bind Failed!");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror(RED "Listen Failed!");
        exit(EXIT_FAILURE);
    }

    log_msg(GRAY "Listening on Port " BRIGHT_RED "%d" RESET, PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror(RED "Accept failed!");
            continue;
        }

        clear(new_socket);

        int *new_socket_ptr = malloc(sizeof(int));
        if (!new_socket_ptr) {
            perror("Failed to allocate memory for new socket");
            close(new_socket);
            continue;
        }
        *new_socket_ptr = new_socket;

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, new_socket_ptr) != 0) {
            perror(RED "pthread_create failed!");
            free(new_socket_ptr);
            close(new_socket);
        }

        pthread_detach(thread_id);
    }

    close(server_fd);
    cleanup_connections();
    return 0;
}
