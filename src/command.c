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
    char *input_file = NULL;
    char *output_file = NULL;

    // Arka plan kontrolü
    if (command[strlen(command) - 1] == '&') {
        is_background = 1;
        command[strlen(command) - 1] = '\0'; // '&' karakterini kaldır
    }

    // Yönlendirme kontrolü
    handle_redirection(command, args, &input_file, &output_file);

    pid_t pid = fork();
    if (pid == 0) {
        // Çocuk süreç

        // Giriş yönlendirmesi
        if (input_file) {
            int fd_in = open(input_file, O_RDONLY);
            if (fd_in == -1) {
                perror("Input file open failed");
                exit(EXIT_FAILURE);
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }

        // Çıkış yönlendirmesi
        if (output_file) {
            int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out == -1) {
                perror("Output file open failed");
                exit(EXIT_FAILURE);
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }

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

        // Her komutu sırayla çalıştır
        execute_command(command);

        // Sonraki komutu al
        command = strtok_r(NULL, ";", &saveptr);
    }

    free(copy); // Kopyalanan string'i serbest bırak
}


void execute_pipe(char *commands) {
    char *args[64];
    char *command_list[64];
    int num_commands = 0;
    int pipefd[2];
    pid_t pid;

    // Komutları "|" ile ayrıştır
    char *token = strtok(commands, "|");

    while (token != NULL) {
        // Komutun başındaki ve sonundaki boşlukları temizle
        while (*token == ' ') token++;
        char *end = token + strlen(token) - 1;
        while (end > token && *end == ' ') *end-- = '\0';

        command_list[num_commands++] = token;
        token = strtok(NULL, "|");
    }

    int fd_in = 0; // İlk süreç için `stdin` başlangıcı

    // Pipe işlemlerini sırayla çalıştır
    for (int i = 0; i < num_commands; i++) {
        pipe(pipefd); // Pipe oluştur

        if ((pid = fork()) == 0) {
            // Çocuk süreç
            dup2(fd_in, STDIN_FILENO); // Önceki sürecin çıktısını stdin olarak bağla
            if (i < num_commands - 1) {
                dup2(pipefd[1], STDOUT_FILENO); // Sonraki pipe'ın girişini stdout olarak bağla
            }
            close(pipefd[0]); // Pipe'ın okuma ucunu kapat

            // Komutu ayrıştır
            int j = 0;
            char *cmd_token = strtok(command_list[i], " ");
            while (cmd_token != NULL) {
                args[j++] = cmd_token;
                cmd_token = strtok(NULL, " ");
            }
            args[j] = NULL;

            // Komutu çalıştır
            if (execvp(args[0], args) == -1) {
                perror("Execution failed");
                exit(EXIT_FAILURE);
            }
        } else if (pid > 0) {
            // Ebeveyn süreç
            wait(NULL); // Çocuk sürecin tamamlanmasını bekle
            close(pipefd[1]); // Pipe'ın yazma ucunu kapat
            fd_in = pipefd[0]; // Sonraki süreç için input ayarla
        } else {
            perror("Fork failed");
        }
    }
}
