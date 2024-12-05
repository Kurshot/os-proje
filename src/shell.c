#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "command.h"

void run_shell() {
    char command[1024];

    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(command, sizeof(command), stdin) == NULL) {
            break; // Kullanıcı çıkış yaparsa
        }

        // Komutun sonundaki yeni satır karakterini kaldır
        command[strcspn(command, "\n")] = '\0';

        // Boş komut girildiyse atla
        if (strlen(command) == 0) {
            continue;
        }

        // Noktalı virgül ile ayrılmış komutları sırayla çalıştır
        if (strstr(command, ";")) {
            execute_sequential(command);
        } else {
            // Tek bir komut varsa
            execute_command(command);
        }
    }
}
