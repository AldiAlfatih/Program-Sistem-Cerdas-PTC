// #include <Arduino.h>
// #include <WiFi.h>
// #include <Firebase_ESP_Client.h>
// #include <DHT.h>
// #include <OneWire.h>
// #include <DallasTemperature.h>
// #include <GravityTDS.h>

// // Wi-Fi dan Firebase Configuration
// #define SSID "ALPHA" // Nama Wi-Fi
// #define PASSWORD "BelumJadi" // Password Wi-Fi
// #define API_KEY "AIzaSyBbgDwXlmMRjsywV3FZMOwtf0HVKmKqGXE" // API Key Firebase
// #define DATABASE_URL "https://hydrogo-cc076-default-rtdb.firebaseio.com/" // URL Database Firebase

// // Pin Definitions
// #define DHT_PIN 18 // Pin untuk DHT22
// #define DHT_TYPE DHT22 // Tipe sensor DHT
// #define DS18B20_PIN 5 // Pin untuk DS18B20
// #define TRIG_PIN 12 // Pin Trig sensor ultrasonik
// #define ECHO_PIN 13 // Pin Echo sensor ultrasonik
// #define TDS_PIN 34 // Pin analog untuk sensor TDS
// #define POMPA_NUTRISI_A 14 // Pin untuk pompa nutrisi A
// #define POMPA_NUTRISI_B 27 // Pin untuk pompa nutrisi B
// #define POMPA_AIR_BERSIH 26 // Pin untuk pompa air bersih
// #define MOTOR_DC 25 // Pin untuk motor DC

// // Firebase instances
// FirebaseData fbdo;
// FirebaseAuth auth;
// FirebaseConfig config;

// // Sensor instances
// DHT dht(DHT_PIN, DHT_TYPE);
// OneWire oneWire(DS18B20_PIN);
// DallasTemperature ds18b20(&oneWire);
// GravityTDS gravityTds;

// // Variabel untuk sensor ultrasonik
// long duration;
// float distance;

// // Variabel untuk logika kontrol
// float airTinggi;       // Ketinggian air
// float suhuAir;         // Suhu air
// float tdsValue;        // Nilai TDS
// float suhuUdara;       // Suhu udara dari DHT22
// float kelembapanUdara; // Kelembapan dari DHT22

// void setup() {
//   Serial.begin(115200);

//   // Inisialisasi sensor
//   dht.begin();
//   ds18b20.begin();
//   gravityTds.setPin(TDS_PIN);
//   gravityTds.setAref(3.3);  // Tegangan referensi ESP32 (3.3V)
//   gravityTds.setAdcRange(4096); // Resolusi ADC ESP32
//   gravityTds.begin();

//   // Inisialisasi pin aktuator
//   pinMode(POMPA_NUTRISI_A, OUTPUT);
//   pinMode(POMPA_NUTRISI_B, OUTPUT);
//   pinMode(POMPA_AIR_BERSIH, OUTPUT);
//   pinMode(MOTOR_DC, OUTPUT);
//   pinMode(TRIG_PIN, OUTPUT);
//   pinMode(ECHO_PIN, INPUT);

//   // Pastikan aktuator mati saat awal
//   digitalWrite(POMPA_NUTRISI_A, LOW);
//   digitalWrite(POMPA_NUTRISI_B, LOW);
//   digitalWrite(POMPA_AIR_BERSIH, LOW);
//   digitalWrite(MOTOR_DC, LOW);

//   // Koneksi Wi-Fi
//   Serial.println("Memulai koneksi Wi-Fi...");
//   WiFi.begin(SSID, PASSWORD);

//   // Tunggu hingga terhubung ke Wi-Fi
//   while (WiFi.status() != WL_CONNECTED) {
//     Serial.print("Menghubungkan ...");
//     delay(300);
//   }

//   Serial.print("Terhubung ke Wi-Fi dengan IP: ");
//   Serial.println(WiFi.localIP());

//   // Konfigurasi Firebase
//   config.api_key = API_KEY;
//   config.database_url = DATABASE_URL;

//   // Coba untuk Sign Up ke Firebase
//   Serial.println("Menghubungkan ke Firebase...");
//   if (Firebase.signUp(&config, &auth, "", "")) {
//     Serial.println("SignUp berhasil");
//   } else {
//     Serial.printf("Error SignUp: %s\n", config.signer.signupError.message.c_str());
//   }

//   Firebase.begin(&config, &auth);
//   Serial.println("Firebase dimulai.");
//   Serial.println("Inisialisasi selesai.");
// }

// void loop() {
//   // Baca sensor DHT22
//   suhuUdara = dht.readTemperature();
//   kelembapanUdara = dht.readHumidity();

//   // Baca sensor DS18B20
//   ds18b20.requestTemperatures();
//   suhuAir = ds18b20.getTempCByIndex(0);

//   // Baca nilai TDS
//   gravityTds.setTemperature(suhuAir); // Set suhu untuk kompensasi
//   gravityTds.update();
//   tdsValue = gravityTds.getTdsValue();

//   // Baca sensor ultrasonik
//   digitalWrite(TRIG_PIN, LOW);
//   delayMicroseconds(2);
//   digitalWrite(TRIG_PIN, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(TRIG_PIN, LOW);
//   duration = pulseIn(ECHO_PIN, HIGH);
//   distance = duration * 0.034 / 2; // Jarak dalam cm
//   airTinggi = distance;           // Simpan sebagai ketinggian air

//   // Tampilkan data sensor
//   Serial.println("===== Data Sensor =====");
//   Serial.print("Suhu Udara: ");
//   Serial.print(suhuUdara);
//   Serial.println(" °C");
//   Serial.print("Kelembapan Udara: ");
//   Serial.print(kelembapanUdara);
//   Serial.println(" %");
//   Serial.print("Suhu Air: ");
//   Serial.print(suhuAir);
//   Serial.println(" °C");
//   Serial.print("Nilai TDS: ");
//   Serial.print(tdsValue);
//   Serial.println(" ppm");
//   Serial.print("Ketinggian Air: ");
//   Serial.print(airTinggi);
//   Serial.println(" cm");
//   Serial.println("=======================");

//   // Kirim data ke Firebase

//   if (Firebase.RTDB.setFloat(&fbdo, "/DATA_SENSOR/Temperature", suhuUdara) &&
//       Firebase.RTDB.setFloat(&fbdo, "/DATA_SENSOR/Humidity", kelembapanUdara) &&
//       Firebase.RTDB.setFloat(&fbdo, "/DATA_SENSOR/WaterTemp", suhuAir) &&
//       Firebase.RTDB.setFloat(&fbdo, "/DATA_SENSOR/TDS", tdsValue)) {
//     Serial.println("Data berhasil dikirim ke Firebase");
//   } else {
//     Serial.print("Gagal mengirim data ke Firebase: ");
//     Serial.println(fbdo.errorReason());
//   }

//   // Logika kontrol aktuator
//   if (airTinggi < 10) {
//     Serial.println("Pompa air bersih diaktifkan.");
//     digitalWrite(POMPA_AIR_BERSIH, HIGH);
//     delay(5000); // Pompa menyala selama 5 detik
//     digitalWrite(POMPA_AIR_BERSIH, LOW);
//   }

//   if (tdsValue < 560) {
//     Serial.println("Pompa nutrisi A dan B diaktifkan untuk menambah nutrisi.");
//     digitalWrite(POMPA_NUTRISI_A, HIGH);
//     digitalWrite(POMPA_NUTRISI_B, HIGH);
//     delay(5000); // Pompa menyala selama 5 detik
//     digitalWrite(POMPA_NUTRISI_A, LOW);
//     digitalWrite(POMPA_NUTRISI_B, LOW);
//   }

//   if (tdsValue > 840) {
//     Serial.println("Pompa air bersih diaktifkan untuk mengencerkan larutan.");
//     digitalWrite(POMPA_AIR_BERSIH, HIGH);
//     delay(5000); // Pompa menyala selama 5 detik
//     digitalWrite(POMPA_AIR_BERSIH, LOW);
//   }

//   if (airTinggi > 15 || (tdsValue >= 560 && tdsValue <= 840)) {
//     Serial.println("Motor pengaduk diaktifkan.");
//     digitalWrite(MOTOR_DC, HIGH);
//     delay(5000); // Motor menyala selama 5 detik
//     digitalWrite(MOTOR_DC, LOW);
//   }

//   delay(2000); // Delay untuk pembacaan sensor berikutnya
//  }
// #include <Arduino.h>
// #include <WiFi.h>
// #include <Firebase_ESP_Client.h>
// #include <DHT.h>
// #include <OneWire.h>
// #include <DallasTemperature.h>
// #include <GravityTDS.h>
// #include <EEPROM.h>
// #include <NTPClient.h>
// #include <WiFiUdp.h>

// // Wi-Fi dan Firebase Configuration
// #define SSID "ALPHA" // Nama Wi-Fi
// #define PASSWORD "BelumJadi" // Password Wi-Fi
// #define API_KEY "AIzaSyBbgDwXlmMRjsywV3FZMOwtf0HVKmKqGXE" // API Key Firebase
// #define DATABASE_URL "https://hydrogo-cc076-default-rtdb.firebaseio.com/" // URL Database Firebase

// // Pin Definitions
// #define DHT_PIN 18 // Pin untuk DHT22
// #define DHT_TYPE DHT22 // Tipe sensor DHT
// #define DS18B20_PIN 5 // Pin untuk DS18B20
// #define TRIG_PIN 12 // Pin Trig sensor ultrasonik
// #define ECHO_PIN 13 // Pin Echo sensor ultrasonik
// #define TDS_PIN 34 // Pin analog untuk sensor TDS
// #define POMPA_NUTRISI_A 14 // Pin untuk pompa nutrisi A
// #define POMPA_NUTRISI_B 27 // Pin untuk pompa nutrisi B
// #define POMPA_AIR_BERSIH 26 // Pin untuk pompa air bersih
// #define MOTOR_DC 25 // Pin untuk motor DC

// // EEPROM Addresses
// #define TDS_CALIBRATION_ADDR 0 // Alamat EEPROM untuk kalibrasi TDS

// // Firebase instances
// FirebaseData fbdo;
// FirebaseAuth auth;
// FirebaseConfig config;

// // Sensor instances
// DHT dht(DHT_PIN, DHT_TYPE);
// OneWire oneWire(DS18B20_PIN);
// DallasTemperature ds18b20(&oneWire);
// GravityTDS gravityTds;

// // NTP Client untuk mendapatkan waktu
// WiFiUDP ntpUDP;
// NTPClient timeClient(ntpUDP, "pool.ntp.org", 28800);

// // Variabel untuk sensor ultrasonik
// long duration;
// float distance;

// // Variabel untuk logika kontrol
// float airTinggi;       // Ketinggian air
// float suhuAir;         // Suhu air
// float tdsValue;        // Nilai TDS
// float suhuUdara;       // Suhu udara dari DHT22
// float kelembapanUdara; // Kelembapan dari DHT22
// float tdsCalibration;  // Nilai kalibrasi TDS

// // Variabel untuk Firebase
// unsigned long lastSendTime = 0;
// const unsigned long interval = 300000; // 5 menit (300000 ms)

// // Konstanta untuk logika kontrol
// const float JARAK_SENSOR_KE_DASAR = 10.0; // Jarak tetap sensor ke dasar kolam
// const float KETINGGIAN_MINIMAL = 3.0;     // Ketinggian air minimal sebelum pompa nyala

// void setup() {
//   Serial.begin(115200);

//   // Inisialisasi EEPROM
//   EEPROM.begin(512);
//   tdsCalibration = EEPROM.readFloat(TDS_CALIBRATION_ADDR);
//   if (isnan(tdsCalibration)) {
//     tdsCalibration = 1.0;
//     EEPROM.writeFloat(TDS_CALIBRATION_ADDR, tdsCalibration);
//     EEPROM.commit();
//   }

//   // Inisialisasi sensor
//   dht.begin();
//   ds18b20.begin();
//   gravityTds.setPin(TDS_PIN);
//   gravityTds.setAref(3.3);
//   gravityTds.setAdcRange(4096);
//   gravityTds.begin();
//   gravityTds.setKvalueAddress(tdsCalibration);

//   // Inisialisasi pin aktuator
//   pinMode(POMPA_NUTRISI_A, OUTPUT);
//   pinMode(POMPA_NUTRISI_B, OUTPUT);
//   pinMode(POMPA_AIR_BERSIH, OUTPUT);
//   pinMode(MOTOR_DC, OUTPUT);
//   pinMode(TRIG_PIN, OUTPUT);
//   pinMode(ECHO_PIN, INPUT);

//   // Pastikan aktuator mati saat awal
//   digitalWrite(POMPA_NUTRISI_A, LOW);
//   digitalWrite(POMPA_NUTRISI_B, LOW);
//   digitalWrite(POMPA_AIR_BERSIH, LOW);
//   digitalWrite(MOTOR_DC, LOW);

//   // Koneksi Wi-Fi
//   Serial.println("Memulai koneksi Wi-Fi...");
//   WiFi.begin(SSID, PASSWORD);
//   while (WiFi.status() != WL_CONNECTED) {
//     Serial.print(".");
//     delay(300);
//   }
//   Serial.println("Terhubung ke Wi-Fi");

//   // NTP Client dimulai
//   timeClient.begin();
//   timeClient.update();

//   // Konfigurasi Firebase
//   config.api_key = API_KEY;
//   config.database_url = DATABASE_URL;

//   if (Firebase.signUp(&config, &auth, "", "")) {
//     Serial.println("SignUp berhasil");
//   } else {
//     Serial.printf("Error SignUp: %s\n", config.signer.signupError.message.c_str());
//   }

//   Firebase.begin(&config, &auth);
//   Serial.println("Firebase dimulai.");
// }

// void kontrolPompaAirBersih() {
//   if (airTinggi < KETINGGIAN_MINIMAL) {
//     Serial.println("Ketinggian air rendah! Menyalakan pompa air bersih...");
//     digitalWrite(POMPA_AIR_BERSIH, HIGH);
//   } else {
//     digitalWrite(POMPA_AIR_BERSIH, LOW);
//   }
// }

// void loop() {
//   timeClient.update(); // Update waktu dari NTP
//   String timestamp = timeClient.getFormattedTime();

//   // Baca sensor DHT22
//   suhuUdara = dht.readTemperature();
//   kelembapanUdara = dht.readHumidity();

//   // Baca sensor DS18B20
//   ds18b20.requestTemperatures();
//   suhuAir = ds18b20.getTempCByIndex(0);

//   // Baca sensor ultrasonik
//   digitalWrite(TRIG_PIN, LOW);
//   delayMicroseconds(2);
//   digitalWrite(TRIG_PIN, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(TRIG_PIN, LOW);
//   duration = pulseIn(ECHO_PIN, HIGH);
//   distance = duration * 0.034 / 2; // Jarak permukaan air ke sensor

//   // Hitung ketinggian air
//   airTinggi = (distance < JARAK_SENSOR_KE_DASAR) ? (JARAK_SENSOR_KE_DASAR - distance) : 0;

//   // Baca nilai TDS
//   gravityTds.setTemperature(suhuAir);
//   gravityTds.update();
//   tdsValue = gravityTds.getTdsValue();

//   // Kirim data ke Firebase setiap 5 menit
//   if (millis() - lastSendTime >= interval) {
//     lastSendTime = millis();
//     String path = "/DATA_SENSOR/" + timestamp;
//     Firebase.RTDB.setFloat(&fbdo, path + "/temperature", suhuUdara);
//     Firebase.RTDB.setFloat(&fbdo, path + "/humidity", kelembapanUdara);
//     Firebase.RTDB.setFloat(&fbdo, path + "/WaterTemp", suhuAir);
//     Firebase.RTDB.setFloat(&fbdo, path + "/TDS", tdsValue);
//     Firebase.RTDB.setFloat(&fbdo, path + "/WaterLevel", airTinggi);
//     Serial.println("Data berhasil dikirim ke Firebase.");
//   }

//   // Tampilkan data sensor di Serial Monitor
//   Serial.println("===== Data Sensor =====");
//   Serial.print("Waktu: ");
//   Serial.println(timestamp);
//   Serial.print("Suhu Udara: ");
//   Serial.print(suhuUdara);
//   Serial.println(" \u00B0C");
//   Serial.print("Kelembapan Udara: ");
//   Serial.print(kelembapanUdara);
//   Serial.println(" %");
//   Serial.print("Suhu Air: ");
//   Serial.print(suhuAir);
//   Serial.println(" \u00B0C");
//   Serial.print("Nilai TDS: ");
//   Serial.print(tdsValue);
//   Serial.println(" ppm");
//   Serial.print("Ketinggian Air: ");
//   Serial.print(airTinggi);
//   Serial.println(" cm");
//   Serial.println("=======================");

//   // Kontrol pompa air bersih
//   kontrolPompaAirBersih();

//   delay(2000); // Delay pembacaan berikutnya
// }
#include <EEPROM.h>
#include "GravityTDS.h"
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Wi-Fi dan Firebase Configuration
#define SSID "Galaxy A20" // Nama Wi-Fi
#define PASSWORD "!!!!!!!!!" // Password Wi-Fi
#define API_KEY "AIzaSyBbgDwXlmMRjsywV3FZMOwtf0HVKmKqGXE" // API Key Firebase
#define DATABASE_URL "https://hydrogo-cc076-default-rtdb.firebaseio.com/" // URL Database Firebase

// Pin Definitions
#define DHT_PIN 18
#define DHT_TYPE DHT22
#define DS18B20_PIN 4
#define TRIG_PIN 12
#define ECHO_PIN 13
#define TDS_PIN 34
#define POMPA_NUTRISI_A 14
#define POMPA_NUTRISI_B 27
#define POMPA_AIR_BERSIH 26
#define MOTOR_DC 25
// EEPROM Addresses
#define TDS_CALIBRATION_ADDR 0 // Alamat EEPROM untuk kalibrasi TDS

// Firebase instances
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Sensor instances
DHT dht(DHT_PIN, DHT_TYPE);
OneWire oneWire(DS18B20_PIN);
DallasTemperature ds18b20(&oneWire);
GravityTDS gravityTds;

// NTP Client untuk mendapatkan waktu
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 28800 + 3600); // Waktu Indonesia Tengah (WITA, GMT+8)

// Variabel untuk sensor ultrasonik
long duration;
float distanceCm;

// Variabel untuk logika kontrol
float suhuAir;         // Suhu air
float tdsValue;        // Nilai TDS
float suhuUdara;       // Suhu udara dari DHT22
float kelembapanUdara; // Kelembapan dari DHT22
float tdsCalibration;  // Nilai kalibrasi TDS
float airTinggi;       // Tinggi air
float distance; // atau nilai lainnya


// Variabel untuk Firebase
unsigned long lastSendTime = 0;
const unsigned long interval = 300000; // 5 menit (300000 ms)

void setup() {
  Serial.begin(115200);

  // Inisialisasi EEPROM
  EEPROM.begin(512);
  gravityTds.setPin(TDS_PIN);
  gravityTds.setAref(3.3);
  gravityTds.setAdcRange(4096);
  gravityTds.setKvalueAddress(TDS_CALIBRATION_ADDR);
  gravityTds.begin();

  // Inisialisasi sensor
  dht.begin();
  ds18b20.begin();

  // Inisialisasi pin aktuator
  pinMode(POMPA_NUTRISI_A, OUTPUT);
  pinMode(POMPA_NUTRISI_B, OUTPUT);
  pinMode(POMPA_AIR_BERSIH, OUTPUT);
  pinMode(MOTOR_DC, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(POMPA_NUTRISI_A, LOW);
  digitalWrite(POMPA_NUTRISI_B, LOW);
  digitalWrite(POMPA_AIR_BERSIH, LOW);
  digitalWrite(MOTOR_DC, LOW);

  // Koneksi Wi-Fi
  Serial.println("Memulai koneksi Wi-Fi...");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("Terhubung ke Wi-Fi");

  timeClient.begin();
  timeClient.update();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("SignUp berhasil");
  } else {
    Serial.printf("Error SignUp: %s\n", config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);
  Serial.println("Firebase dimulai.");
}

void loop() {
  timeClient.update();
  String timestamp = timeClient.getFormattedTime();

  suhuUdara = dht.readTemperature();
  kelembapanUdara = dht.readHumidity();

  ds18b20.requestTemperatures();
  suhuAir = ds18b20.getTempCByIndex(0);

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distanceCm = duration * 0.0343 / 2;
  airTinggi = distanceCm;

  gravityTds.setTemperature(suhuAir);
  gravityTds.update();
  tdsValue = gravityTds.getTdsValue();

  if (airTinggi > 8) {
    Serial.println("Pompa air bersih diaktifkan.");
    digitalWrite(POMPA_AIR_BERSIH, HIGH);
    delay(5000);
    digitalWrite(POMPA_AIR_BERSIH, LOW);
  }

  if (tdsValue < 560) {
    Serial.println("Pompa nutrisi A dan B diaktifkan.");
    digitalWrite(POMPA_NUTRISI_A, HIGH);
    digitalWrite(POMPA_NUTRISI_B, HIGH);
    delay(5000);
    digitalWrite(POMPA_NUTRISI_A, LOW);
    digitalWrite(POMPA_NUTRISI_B, LOW);
  }

  Serial.println("===== Data Sensor =====");
  Serial.print("Waktu: "); Serial.println(timestamp);
  Serial.print("Suhu Udara: "); Serial.println(suhuUdara);
  Serial.print("Kelembapan Udara: "); Serial.println(kelembapanUdara);
  Serial.print("Suhu Air: "); Serial.println(suhuAir);
  Serial.print("Nilai TDS: "); Serial.println(tdsValue);
  Serial.print("Ketinggian Air: "); Serial.println(airTinggi);
  Serial.println("=======================");

  // Kirim data ke Firebase setiap 30 detik
  if (millis() - lastSendTime >= 30000) { // Interval 30 detik
    lastSendTime = millis();
    String path = "/DATA_SENSOR/" + timestamp;
    if (Firebase.RTDB.setFloat(&fbdo, path + "/temperature", suhuUdara) &&
        Firebase.RTDB.setFloat(&fbdo, path + "/humidity", kelembapanUdara) &&
        Firebase.RTDB.setFloat(&fbdo, path + "/WaterTemp", suhuAir) &&
        Firebase.RTDB.setFloat(&fbdo, path + "/TDS", tdsValue) &&
        Firebase.RTDB.setFloat(&fbdo, path + "/WaterLevel", airTinggi)) {
      Serial.println("Data berhasil dikirim ke Firebase.");
    } else {
      Serial.print("Gagal mengirim data ke Firebase: ");
      Serial.println(fbdo.errorReason());
    }
  }

  delay(2000);
}
