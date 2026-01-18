/*
 * SportsData Stats Parser Engine (VULNERABLE VERSION)
 * Author: LordMs
 * Status: DEPRECATED - DO NOT USE IN PRODUCTION
 *
 * Zafiyet Analizi:
 * Bu kod, oyuncu isimlerini işlerken sınır kontrolü (bounds check) yapmaz.
 * 64 byte'tan uzun bir veri girildiğinde 'admin_rights' değişkeni üzerine yazılabilir.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char player_name[64];
    int is_admin;  // Hafızada player_name'den hemen sonra gelir
    int career_points;
} UserContext;

void process_player_stats(char *input_data) {
    UserContext user;
    user.is_admin = 0; // Varsayılan olarak yetkisiz
    user.career_points = 0;

    // KRİTİK HATA: strcpy sınır kontrolü yapmaz!
    // Eğer input_data 64 karakterden uzunsa, is_admin alanına taşar.
    strcpy(user.player_name, input_data);

    printf("[INFO] İşlenen Oyuncu: %s\n", user.player_name);

    if (user.is_admin != 0) {
        printf("\n[!!!] KRİTİK: YÖNETİCİ YETKİSİ KAZANILDI! [!!!]\n");
        printf("Hafıza manipülasyonu başarılı. Sistem ele geçirildi.\n");
    } else {
        printf("[INFO] Erişim Normal. Yetki yükseltme yok.\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Kullanım: %s <oyuncu_verisi>\n", argv[0]);
        return 1;
    }

    printf("--- SportsData İstatistik Motoru v1.0 ---\n");
    process_player_stats(argv[1]);

    return 0;
}
