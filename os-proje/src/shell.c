// Ahmet Kürşat Sonkur
// Mustafa Köslü
// Talha Mülayim


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include "command.h"

// Aktif süreçlerin sayısını takip etmek için
int active_processes = 0;

// Signal handler: SIGCHLD sinyalini ele alır
void handle_sigchld(int sig) {
    int status;
    pid_t pid;

    // Tüm tamamlanan süreçleri ele al
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        active_processes--; // Aktif süreç sayısını azalt

        // Çıkış durumunu raporla
        if (WIFEXITED(status)) {
            printf("\n[%d] retval: %d\n> ", pid, WEXITSTATUS(status));
            fflush(stdout); // Tamponu boşalt ve hemen yazdır
        } else {
            printf("\n[%d] terminated abnormally\n", pid);
            fflush(stdout); // Tamponu boşalt ve hemen yazdır
        }
    }
}


// Ana kabuk döngüsü
void run_shell() {
    char command[1024];
    struct sigaction sa;
    bool quit_flag = false;

    // Signal handler ayarla
    sa.sa_handler = &handle_sigchld;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    while (1) {
        if (!quit_flag) {
            printf("> ");
            fflush(stdout);
        }

        if (fgets(command, sizeof(command), stdin) == NULL) {
            break; // Kullanıcı çıkış yaparsa
        }

        // Komutun sonundaki yeni satır karakterini kaldır
        command[strcspn(command, "\n")] = '\0';

        // Boş komut girildiyse atla
        if (strlen(command) == 0) {
            continue;
        }

        // Quit komutu kontrolü
        if (strcmp(command, "quit") == 0) {
            quit_flag = true; // Komut girişini durdur
            printf("Arkaplan proseslerinin bitmesi bekleniyor...");
			fflush(stdout);

            // Tüm arka plan işlemlerini bekle ve raporla
            while (active_processes == 0) {
                pause(); // Arka planda çalışan süreçler için bekle
            }

            // Shell'i kapat
            printf("\nKabuktan çıkılıyor.\n");
			fflush(stdout);
            break;
        }

        // Quit bayrağı set edildikten sonra başka komut almayın
        if (quit_flag) {
            continue;
        }

        fflush(stdout);
      
        // Noktalı virgül ile ayrılmış komutları sırayla çalıştır
        if (strstr(command, ";")) {
			fflush(stdout);
            execute_sequential(command);
        }
		// Boru ile ayrılmış komutları çalıştır
        else if (strstr(command, "|")) {
            execute_pipe(command);
        }
        // Tek bir komut varsa
        else {
            execute_command(command);
        }
    }
}
