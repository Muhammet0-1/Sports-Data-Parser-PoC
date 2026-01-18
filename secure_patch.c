/*
 * SportsData Stats Parser Engine (SECURE PATCHED VERSION)
 * Author: LordMs
 * Status: PRODUCTION READY
 *
 * Güvenlik Yaması:
 * 1. 'strcpy' yerine 'strncpy' kullanıldı.
 * 2. Buffer sınırları kontrol edildi.
 * 3. Hafıza taşması (Buffer Overflow) engellendi.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char player_name[64];
    int is_admin;
    int career_points;
} UserContext;

void process_player_stats_secure(char *input_data) {
    UserContext user;
    user.is_admin = 0;
    user.career_points = 0;

    // GÜVENLİK YAMASI:
    // 1. Gelen verinin boyutunu kontrol et veya sınırlı kopyalama yap.
    // 2. Buffer'ın sonuna mutlaka NULL terminator ekle.
    
    // strncpy, buffer boyutundan (63) fazlasını kopyalamaz.
    strncpy(user.player_name, input_data, sizeof(user.player_name) - 1);
    
    // Null-byte garantisi
    user.player_name[sizeof(user.player_name) - 1] = '\0';

    printf("[SECURE] İşlenen Oyuncu: %s\n", user.player_name);

    if (user.is_admin != 0) {
        printf("[FAIL] Bu mesajı asla görmemelisiniz. Yama çalışmadı!\n");
    } else {
        printf("[SUCCESS] Sistem Güvenli. Hafıza bütünlüğü korundu.\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Kullanım: %s <oyuncu_verisi>\n", argv[0]);
        return 1;
    }

    printf("--- SportsData İstatistik Motoru (Patched) v1.1 ---\n");
    process_player_stats_secure(argv[1]);

    return 0;
}
