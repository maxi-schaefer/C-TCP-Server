#pragma region

#ifndef COMMON_H
#define COMMON_H

#define BLACK "\x1b[30m"
#define GRAY "\x1b[90m"
#define BRIGHT_RED "\x1b[91m"
#define RED "\x1b[31m"
#define BRIGHT_GREEN "\x1b[92m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BRIGHT_BLUE "\x1b[94m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define WHITE "\x1b[37m"
#define RESET "\x1b[0m"

#define BANNER BRIGHT_RED "\t\t ______                         " WHITE "______             \n" BRIGHT_RED "\t\t|  ___ \\                       " WHITE "|  ___ \\       _    \n" BRIGHT_RED "\t\t| |   | | ____ _   _ _   _  ___" WHITE "| |   | | ____| |_  \n" BRIGHT_RED "\t\t| |   | |/ _  | \\ / ) | | |/___)" WHITE " |   | |/ _  )  _) \n" BRIGHT_RED "\t\t| |   | ( (/ / ) X (| |_| |___ " WHITE "| |   | ( (/ /| |__ \n" BRIGHT_RED "\t\t|_|   |_|\\____|_/ \\_)\\____(___/" WHITE "|_|   |_|\\____)\\___)\n\n\n" RESET
#define BUFFER_SIZE 1024
#define PORT 6969
#define MAX_USERNAME_LEN 50
#define PREFIX BRIGHT_RED "Nexus" WHITE "Net" RESET

#include <time.h>
#include <sys/socket.h>

typedef struct client_info {
    int client_socket;
    char username[MAX_USERNAME_LEN];
    char role[50];
    time_t connection_time;
    struct client_info *next;
} client_info;

#endif

#pragma endregion