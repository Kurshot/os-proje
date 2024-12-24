// Ahmet Kürşat Sonkur
// Mustafa Köslü
// Talha Mülayim
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "io_redirect.h"

void redirect_input(char *file) {
    int fd = open(file, O_RDONLY); // Dosyayı sadece okuma modunda aç
    if (fd < 0) {
        perror("Input file error");
        return;
    }
    dup2(fd, STDIN_FILENO); // Standart girdiyi dosyaya yönlendir
    close(fd);
}

void redirect_output(char *file) {
    int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644); // Dosyayı yazma modunda aç
    if (fd < 0) {
        perror("Output file error");
        return;
    }
    dup2(fd, STDOUT_FILENO); // Standart çıktıyı dosyaya yönlendir
    close(fd);
}
