Project UAS IoT | SMART ENVIRONMENT MONITORING SYSTEM 
# Smart Environment Monitoring System

Project UAS Mata Kuliah Internet of Things — Kelompok 5, Kelas A
Program Studi Ilmu Komputer, Universitas Pendidikan Ganesha (Undiksha)

## Deskripsi

Sistem pemantauan lingkungan berbasis IoT yang memantau dua parameter utama, yaitu kualitas udara dan simulasi ketinggian air. Sistem terdiri dari tiga node yang saling terhubung melalui protokol MQTT, dengan output berupa indikator visual, suara, layar OLED, dan dashboard monitoring jarak jauh melalui Blynk.

## Arsitektur Sistem

Tiga node berkomunikasi lewat broker MQTT HiveMQ Cloud (TLS, autentikasi username dan password). Node 3 berperan sebagai pusat pengolahan data sekaligus jembatan pengiriman data ke Blynk untuk monitoring jarak jauh.

```
Node 1 (ESP8266) ---\
                      --> MQTT (HiveMQ Cloud) --> Node 3 (ESP8266) --> Blynk Dashboard
Node 2 (ESP32) ------/                              |
                                                      --> LED, Buzzer, OLED
```

## Node dan Komponen

### Node 1 — Kualitas Udara (ESP8266)
- DHT22 — membaca suhu dan kelembaban udara
- Sensor gas MQ2 — mendeteksi asap dan gas mudah terbakar
- Publish data ke topic `environment/node1` dalam format JSON

### Node 2 — Simulasi Ketinggian Air (ESP32)
- Soil moisture sensor — dimodifikasi fungsinya untuk membaca level ketinggian air berdasarkan prinsip konduktivitas air
- Publish data ke topic `environment/node2` dalam format JSON, lengkap dengan kategori RENDAH, SEDANG, TINGGI

### Node 3 — Output dan Dashboard (ESP8266)
- Subscribe ke topic `environment/node1` dan `environment/node2`
- Mengolah data menjadi status AMAN, SIAGA, atau BAHAYA
- Output: LED hijau/merah, buzzer, layar OLED 128x32
- Meneruskan data ke dashboard Blynk beserta notifikasi otomatis saat status BAHAYA

## Platform dan Komunikasi

| Komponen | Keterangan |
|---|---|
| Protokol utama | MQTT (HiveMQ Cloud, port 8883, TLS) |
| Dashboard tambahan | Blynk IoT |
| Format data | JSON |

## Struktur Repository

```
Node1_KualitasUdara/
  node1_environment.ino
Node2_LevelAir/
  node2_environment.ino
Node3_Output/
  node3_environment.ino
README.md
```

## Library yang Digunakan

- PubSubClient
- DHT sensor library (Adafruit) + Adafruit Unified Sensor
- ArduinoJson
- Blynk (by Blynk)
- Adafruit GFX Library
- Adafruit SSD1306

## Logika Status

| Kondisi | Air | Gas | Output |
|---|---|---|---|
| AMAN | RENDAH | ≤ 400 | LED hijau |
| SIAGA | SEDANG | 401–700 | LED hijau + merah |
| BAHAYA | TINGGI | > 700 | LED merah + buzzer + notifikasi Blynk |

## Catatan

Soil moisture sensor digunakan sebagai pengganti sensor ketinggian air karena keterbatasan alat yang tersedia. Prinsip kerja yang dimanfaatkan adalah konduktivitas air, semakin dalam probe tercelup air, semakin rendah nilai resistansi yang terbaca.

## Anggota Kelompok

1. I Made Krisna Satya Utama — NIM 2315101034
2. Ida Bagus Danendra Putra Sentanu — NIM 2315101055
3. Kadek Andre Maliana — NIM 2315101049
4. Gerrad Sakti Ekatana Sitohang — NIM 2315101033
5. I Wayan Yuya Erawan - NIM 2315101036
6. Hayyul Arzyil - NIM 2315101052
7. Ida Bagus Putu Angga Wicaksana - NIM 2315101038

