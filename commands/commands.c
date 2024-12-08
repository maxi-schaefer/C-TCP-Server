#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "commands.h"
#include "connection.h"
#include "utils.h"

typedef struct {
  char *name;
  char *description;
  char *permission;
  void (*execute)(int client_socket, const char *permission);
} command_t;

static command_t commands[] = {
    {"list users", "List all connected Users (Admin only)", "admin", list_command},
    {"broadcast", "Broadcast a message to all users (Admin only)", "admin", broadcast_command},
    {"clear", "Clear the Terminal Window", "user", clear_command},
    {"exit", "Quit the server", "user", exit_command},
    {"help", "Show available commands", "user", help_command},
    {NULL, NULL, NULL, NULL}
};

int check_permission(int socket, const char* user_perm, const char* perm) {
  if(strcmp(user_perm, perm) != 0) {
     send(socket, RED "Permission denied!\n", strlen(RED "Permission denied!\n"), 0);
     return 1;
  }
  return 0;
}

void execute_command(int client_socket, const char *input, const char *role) {
  for(int i = 0; commands[i].name != NULL; ++i) {
    if(strcmp(input, commands[i].name) == 0) {
      commands[i].execute(client_socket, role);
      return;
    }
  }
  send(client_socket, BRIGHT_RED "Unkown command.\n", strlen(BRIGHT_RED "Unkown command.\n"), 0);
}

void *handle_commands(void *arg) {
    client_info *info = (client_info *)arg;
    int client_socket = info->client_socket;
    char buffer[BUFFER_SIZE];
    char *role = info->role;
    char *username = info->username;

    while (1) {
        char prompt[256];
        snprintf(prompt, sizeof(prompt), BRIGHT_RED "%s" GRAY "@" BRIGHT_RED "Nexus" WHITE "Net " GRAY "➤ " RESET, username);
        send(client_socket, prompt, strlen(prompt), 0);

        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(client_socket, buffer, BUFFER_SIZE);
        if (bytes_read <= 0) {
            log_msg(RED "Client disconnected!");
            remove_connection(client_socket);
            break;
        }

        buffer[strcspn(buffer, "\r\n")] = 0;

        execute_command(client_socket, buffer, role);
    }

    close(client_socket);
    free(info);
    return NULL;
}

void broadcast_command(int client_socket, const char *permission) {
  char message[BUFFER_SIZE];
  memset(message, 0, sizeof(message));

  send(client_socket, BRIGHT_RED "Enter a message" GRAY ": " RESET, strlen(BRIGHT_RED "Enter a message" GRAY ": " RESET), 0);

  read_line(client_socket, message, sizeof(message));

  pthread_mutex_lock(&connections_mutex);

  client_info *curr = connections;
  while(curr != NULL) {
    char broadcast_msg[256];
    snprintf(broadcast_msg, sizeof(broadcast_msg), "\n\n" GRAY "[" BRIGHT_RED "BROADCAST" GRAY "] " RESET "%s\n\n", message);
    if(curr->client_socket != client_socket) {
      send(curr->client_socket, broadcast_msg, strlen(broadcast_msg), 0);

      char prompt[256];
      snprintf(prompt, sizeof(prompt), BRIGHT_RED "%s" GRAY "@" BRIGHT_RED "Nexus" WHITE "Net " GRAY "➤ " RESET, curr->username);
      send(curr->client_socket, prompt, strlen(prompt), 0);
    }
    curr = curr->next;
  }

  pthread_mutex_unlock(&connections_mutex);
  log_msg(GREEN "Broadcast message sent to all clients: %s" RESET, message);
}

void list_command(int client_socket, const char *permission) {
  if(check_permission(client_socket, permission, "admin") != 0) return;
  list_connections(client_socket);
}

void help_command(int client_socket, const char *permission) {
  send(client_socket, "\n" GRAY "═══════════════ " BRIGHT_RED "Help" GRAY " ═══════════════\n", sizeof("\n" GRAY "═══════════════ " BRIGHT_RED "Help" GRAY " ═══════════════\n"), 0);
  for(size_t i = 0; commands[i].name != NULL; ++i) {
    if(strcmp(commands[i].permission, "admin") == 0) {
      if(strcmp(commands[i].permission, permission) != 0) continue;;
    }

    char command_msg[256];
    snprintf(command_msg, sizeof(command_msg), BRIGHT_RED "%s" GRAY " - " RESET "%s\n", commands[i].name, commands[i].description);
    send(client_socket, command_msg, strlen(command_msg), 0);
  }
  send(client_socket, GRAY "═══════════════ " BRIGHT_RED "Help" GRAY " ═══════════════\n\n", sizeof(GRAY "═══════════════ " BRIGHT_RED "Help" GRAY " ═══════════════\n\n"), 0);
}

void clear_command(int client_socket, const char *permission) {
  clear(client_socket);
}

void exit_command(int client_socket, const char *permission) {
    const char *goodbye_msg = BRIGHT_RED "See you soon! Goodbye!";
    remove_connection(client_socket);
    send(client_socket, goodbye_msg, strlen(goodbye_msg), 0);
    close(client_socket);
    log_msg("Client with socket %d disconnected.", client_socket);
    return;
}

