# 🏀 Sports Data Parser - Memory Corruption PoC

![Language](https://img.shields.io/badge/Language-C-blue?style=for-the-badge)
![Vulnerability](https://img.shields.io/badge/Type-Buffer%20Overflow-red?style=for-the-badge)

Bu depo, **Dünya çapında büyük bir spor organizasyonunun** web altyapısında keşfedilen ve **HackerOne** üzerinden raporlanan kritik bir **Stack-based Buffer Overflow** zafiyetinin *Proof of Concept (PoC)* simülasyonudur.

Bu çalışma, keşfedilen zafiyetin mantığını göstermek amacıyla sıfırdan yazılmış bir simülasyondur. Gerçek sistem kodlarını içermez.

## 🚨 Zafiyet Analizi (Simülasyon)

`vulnerable_engine.c` dosyası, zafiyetin çalışma prensibini gösteren örnek bir koddur.
Sorun, kullanıcı verileri işlenirken `strcpy()` fonksiyonunun kullanılması ve girdi boyutunun kontrol edilmemesinden kaynaklanmaktadır.

### Saldırı Senaryosu:
Oyuncu verisi işleyen fonksiyona 64 byte'tan uzun bir veri gönderildiğinde, hafızadaki yetki bayrakları (admin flags) üzerine yazılabilmektedir.

**PoC Çalıştırma:**
```bash
make
./vulnerable_app $(python3 -c "print('A'*64 + '\x01')")

Yasal Uyarı: Bu kodlar tamamen eğitim amaçlıdır ve kapatılmış bir zafiyetin mantıksal simülasyonunu içerir.