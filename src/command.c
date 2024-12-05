#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "command.h"

// Prototipler
void handle_redirection(char *command, char **args, char **input_file, char **output_file);

void execute_command(char *command) {
    char *args[64];
    int is_background = 0;

    // Arka plan kontrolü
    if (command[strlen(command) - 1] == '&') {
        is_background = 1;
        command[strlen(command) - 1] = '\0'; // '&' karakterini kaldır
    }

    // Komut ve argümanları ayrıştır
    char *token = strtok(command, " ");
    int i = 0;
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    pid_t pid = fork();
    if (pid == 0) {
        // Çocuk süreç
        if (execvp(args[0], args) == -1) {
            perror("Execution failed");
        }
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        if (is_background) {
            printf("[PID %d] Running in background\n", pid);
        } else {
            waitpid(pid, NULL, 0); // Ön plan süreci bekle
        }
    } else {
        perror("Fork failed");
    }
}

void handle_redirection(char *command, char **args, char **input_file, char **output_file) {
    char *token = strtok(command, " ");
    int i = 0;

    while (token != NULL) {
        if (strcmp(token, "<") == 0) {
            *input_file = strtok(NULL, " ");
        } else if (strcmp(token, ">") == 0) {
            *output_file = strtok(NULL, " ");
        } else {
            args[i++] = token;
        }
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
}


void execute_pipe(char *commands) {
    char *args[64];
    int num_commands = 0;
    char *command_list[64];
    int pipefd[2];
    pid_t pid;
    int fd_in = 0;

    char *token = strtok(commands, "|");
    while (token != NULL) {
        command_list[num_commands++] = token;
        token = strtok(NULL, "|");
    }

    for (int i = 0; i < num_commands; i++) {
        pipe(pipefd);
        pid = fork();

        if (pid == 0) {
            dup2(fd_in, STDIN_FILENO);
            if (i < num_commands - 1) {
                dup2(pipefd[1], STDOUT_FILENO);
            }
            close(pipefd[0]);

            int j = 0;
            char *cmd_token = strtok(command_list[i], " ");
            while (cmd_token != NULL) {
                args[j++] = cmd_token;
                cmd_token = strtok(NULL, " ");
            }
            args[j] = NULL;

            if (execvp(args[0], args) == -1) {
                perror("Execution failed");
                exit(EXIT_FAILURE);
            }
        } else if (pid > 0) {
            waitpid(pid, NULL, 0);
            close(pipefd[1]);
            fd_in = pipefd[0];
        } else {
            perror("Fork failed");
        }
    }
}

void execute_sequential(char *commands) {
    char *command;
    char *copy = strdup(commands); // Komutların bir kopyasını al
    char *saveptr = NULL;

    command = strtok_r(copy, ";", &saveptr); // İlk komutu al

    while (command != NULL) {
        // Başındaki ve sonundaki boşlukları temizle
        while (*command == ' ') command++; // Başındaki boşlukları kaldır
        char *end = command + strlen(command) - 1;
        while (end > command && *end == ' ') *end-- = '\0'; // Sonundaki boşlukları kaldır

        // Debugging için ayrıştırılan komutu yazdır
        printf("Executing command: %s\n", command);

        // Her komutu sırayla çalıştır
        execute_command(command);

        // Sonraki komutu al
        command = strtok_r(NULL, ";", &saveptr);
    }

    free(copy); // Kopyalanan string'i serbest bırak
}
