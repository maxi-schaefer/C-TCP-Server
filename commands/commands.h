#pragma region
#include "common.h"

void *handle_commands(void *arg);
void execute_command(int client_socket, const char *input, const char *role);
void list_command(int client_socket, const char *permission);
void broadcast_command(int client_socket, const char *permission);
void help_command(int client_socket, const char *permission);
void clear_command(int client_socket, const char *permission);
void exit_command(int client_socket, const char *permission);

#pragma endregion