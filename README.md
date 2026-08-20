# Sports Data Parser — Secure C Memory-Safety Lab

[![CI](https://github.com/Muhammet0-1/Sports-Data-Parser-PoC/actions/workflows/ci.yml/badge.svg)](https://github.com/Muhammet0-1/Sports-Data-Parser-PoC/actions/workflows/ci.yml)
[![Language](https://img.shields.io/badge/Language-C11-00599C?logo=c)](https://en.cppreference.com/w/c/11)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

Bu depo, C dilinde sınır kontrollü veri ayrıştırma, güvenli derleme seçenekleri ve sanitizer
tabanlı regresyon testlerini gösteren tamamen **yerel ve sentetik** bir eğitim laboratuvarıdır.
Komut satırından verilen basit bir sporcu kaydını doğrular ve normalize edilmiş JSON çıktısı
üretir.

> [!IMPORTANT]
> Bu proje gerçek bir spor kuruluşuna ait kod, veri veya doğrulanmış bir güvenlik açığı içermez.
> Gerçek bir bug bounty raporunun kanıtı olarak sunulmaz. Kasıtlı hatalı örnek yalnızca yerel
> bellek güvenliği eğitiminde sanitizer tarafından yakalanmak üzere tutulur.

## Neler gösteriliyor?

- Uzunluk bilgisi alan ve embedded NUL byte'ları reddeden C API
- Tam olarak üç alanlı `player_id,player_name,career_points` şeması
- Taşma kontrollü `uint32_t` ayrıştırma
- 1–63 byte aralığında, kontrol karakteri içermeyen geçerli UTF-8 isim doğrulaması
- Başarısız ayrıştırmada çıktı nesnesini değiştirmeyen transaction benzeri davranış
- JSON stringlerinde tırnak ve ters eğik çizgi kaçışları
- `-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wformat=2 -Werror`
- Stack protector, `_FORTIFY_SOURCE=3`, PIE, RELRO, immediate binding ve non-executable stack
- AddressSanitizer ve UndefinedBehaviorSanitizer test hedefleri
- GCC/Clang CI matrisi

## Güvenlik modeli

Üretim hedefi olan `sports-parser`:

- dosya veya ağ girdisi açmaz;
- yalnızca tek bir komut satırı kaydını işler;
- dinamik bellek ayırmaz;
- shell veya subprocess çağırmaz;
- girdi sınırlarını derlemeden önce değil çalışma zamanında doğrular;
- hata durumunda gerekçe ve kararlı bir çıkış kodu döndürür.

`vulnerable_engine.c` kasıtlı bir sınırsız indeksli kopyalama hatası içerir, fakat doğrudan derleme
compile-time guard ile engellenir. Makefile bu örneği yalnızca ASan/UBSan etkin şekilde derler. Eski
`-fno-stack-protector` ve executable-stack seçenekleri kaldırılmıştır.

## Gereksinimler

- Linux ortamı
- GNU Make
- C11 destekli GCC veya Clang

Ubuntu/Debian örneği:

```bash
sudo apt install build-essential clang make
```

## Derleme

```bash
git clone https://github.com/Muhammet0-1/Sports-Data-Parser-PoC.git
cd Sports-Data-Parser-PoC
make
```

Oluşan binary:

```text
build/sports-parser
```

## Kullanım

```bash
./build/sports-parser '23,Michael Jordan,32292'
```

Çıktı:

```json
{"career_points":32292,"player_id":23,"player_name":"Michael Jordan"}
```

UTF-8 isimler ve alanların çevresindeki ASCII boşlukları desteklenir:

```bash
./build/sports-parser ' 1 , Alperen Şengün , 0 '
```

## Veri sözleşmesi

| Alan | Kural |
| --- | --- |
| `player_id` | `1`–`4294967295` arasında ondalık tamsayı |
| `player_name` | Geçerli UTF-8, 1–63 byte, kontrol karakteri ve virgül içermez |
| `career_points` | `0`–`4294967295` arasında ondalık tamsayı |
| Toplam girdi | En fazla 256 byte ve tam olarak iki virgül |

Bu basit eğitim formatı quoted CSV değildir; isim içinde virgül desteklenmez.

## Çıkış kodları

| Kod | Anlam |
| ---: | --- |
| `0` | Kayıt başarıyla doğrulandı |
| `1` | Kayıt şema veya değer doğrulamasından geçmedi |
| `2` | Komut satırı kullanımı hatalı |

## Test ve sanitizer kontrolleri

```bash
make test
make sanitize
```

67 kontrollü assertion içeren normal test hedefi; sınır değerlerini, sayısal taşmaları, UTF-8
hatalarını, embedded NUL byte'ları, alan sayılarını ve API sözleşmesini kapsar.

Kasıtlı hatalı örneği güvenli laboratuvar ayarlarıyla derlemek ve sanitizer'ın hatayı yakaladığını
doğrulamak için:

```bash
make lab
make lab-check
```

Bu hedef yalnızca sentetik yerel girdiyi kullanır ve exploit üretmez.

## Proje yapısı

```text
include/sports_parser.h  Public parser API
src/sports_parser.c      Doğrulama ve ayrıştırma uygulaması
src/main.c               Güvenli JSON CLI
tests/test_parser.c      Bağımsız C test koşucusu
secure_patch.c           Eski güvenli giriş noktası için uyumluluk wrapper'ı
vulnerable_engine.c      Guard + sanitizer ile sınırlanmış eğitim örneği
Makefile                 Hardened build, test, sanitize ve lab hedefleri
```

## Sınırlamalar

- Format gerçek CSV/JSON yerine bilinçli olarak küçük bir eğitim şemasıdır.
- Unicode normalizasyonu veya grapheme sayımı yapmaz; yalnızca UTF-8 byte dizisini doğrular.
- JSON çıktısı tek kayıt içindir; batch işleme ve kalıcı depolama sağlamaz.
- Kasıtlı hatalı örnek yalnızca bellek güvenliği mekanizmalarını öğretir; gerçek bir ürün
  zafiyetini temsil etmez.

## Geliştirme

```bash
make clean
make CC=gcc test sanitize
make clean
make CC=clang test sanitize
```

Katkı kuralları için [CONTRIBUTING.md](CONTRIBUTING.md), güvenlik bildirimleri için
[SECURITY.md](SECURITY.md), sürüm geçmişi için [CHANGELOG.md](CHANGELOG.md) dosyasına bakın.

## Lisans

[MIT License](LICENSE) — Copyright (c) 2026 Muhammet0-1
