#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "auth.h"
#include "common.h"

#define USERS_FILE "users.txt"

int authenticate_user(const char *username, const char *password, char *role) {
    FILE *file = fopen(USERS_FILE, "r");
    if (!file) {
        perror(RED "Failed to open users file");
        return 0;
    }

    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), file)) {
        char stored_username[50], stored_password[50], stored_role[50];

        if (sscanf(line, "%49[^:]:%49[^:]:%49[^\n]", stored_username, stored_password, stored_role) != 3) {
            fprintf(stderr, YELLOW "Malformed line in users file: %s" RESET, line);
            continue;
        }

        log_msg(BRIGHT_BLUE "Comparing username='%s' with stored_username='%s'", username, stored_username);
        log_msg(BRIGHT_BLUE "Comparing password='%s' with stored_password='%s'", password, stored_password);

        if (strcmp(username, stored_username) == 0 && strcmp(password, stored_password) == 0) {
            strncpy(role, stored_role, 50);
            role[49] = '\0';
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

