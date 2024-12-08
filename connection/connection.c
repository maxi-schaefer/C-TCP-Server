#include "connection.h"
#include "common.h"

client_info *connections = NULL;
pthread_mutex_t connections_mutex = PTHREAD_MUTEX_INITIALIZER;

void add_connection(client_info *info) {
    pthread_mutex_lock(&connections_mutex);

    info->next = connections;
    connections = info;

    pthread_mutex_unlock(&connections_mutex);
}

void remove_connection(int client_socket) {
    pthread_mutex_lock(&connections_mutex);

    client_info *prev = NULL, *curr = connections;
    while (curr != NULL) {
        if (curr->client_socket == client_socket) {
            if (prev == NULL) {
                connections = curr->next;
            } else {
                prev->next = curr->next;
            }

            break;
        }
        prev = curr;
        curr = curr->next;
    }

    pthread_mutex_unlock(&connections_mutex);
}

void list_connections(int client_socket) {
    pthread_mutex_lock(&connections_mutex);
    send(client_socket, "\n" GRAY "═══════════════ " BRIGHT_RED "Connections" GRAY " ═══════════════\n", sizeof("\n" GRAY "═══════════════ " BRIGHT_RED "Connections" GRAY " ═══════════════\n"), 0);

    client_info *curr = connections;
    if (curr == NULL) {
        send(client_socket, "No active connections.\n", strlen("No active connections.\n"), 0);
    } else {
        char buffer[1024];
        while (curr != NULL) {
            // Calculate the elapsed time since the connection was established
            time_t current_time = time(NULL);
            time_t elapsed_time = current_time - curr->connection_time;

            // Convert elapsed time to hours, minutes, seconds
            int hours = elapsed_time / 3600;
            int minutes = (elapsed_time % 3600) / 60;
            int seconds = elapsed_time % 60;

            // Format the connection information, including the connection time
            snprintf(buffer, sizeof(buffer), WHITE "Username" GRAY ": " BRIGHT_RED "%s" GRAY " - " WHITE "Connected" GRAY": " BRIGHT_RED "%dh %dm %ds ago\n", curr->username, hours, minutes, seconds);

            send(client_socket, buffer, strlen(buffer), 0);
            curr = curr->next;
        }
    }
    send(client_socket, GRAY "═══════════════ " BRIGHT_RED "Connections" GRAY " ═══════════════\n\n", sizeof(GRAY "═══════════════ " BRIGHT_RED "Connections" GRAY " ═══════════════\n\n"), 0);

    pthread_mutex_unlock(&connections_mutex);
}

void cleanup_connections() {
    pthread_mutex_lock(&connections_mutex);

    client_info *curr = connections;
    while (curr != NULL) {
        client_info *temp = curr;
        curr = curr->next;
        free(temp);
    }

    pthread_mutex_unlock(&connections_mutex);
}