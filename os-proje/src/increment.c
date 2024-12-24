// Ahmet Kürşat Sonkur
// Mustafa Köslü
// Talha Mülayim
#include <stdio.h>
#include <stdlib.h>

int main_inc() {
    int num;
    if (scanf("%d", &num) == 1) { // Standart girdiden bir sayı oku
        printf("%d\n", num + 1);  // Sayıyı 1 artır ve ekrana yaz
    } else {
        fprintf(stderr, "Invalid input\n");
        return 1; // Hata durumunda 1 döndür
    }
    return 0;
}
