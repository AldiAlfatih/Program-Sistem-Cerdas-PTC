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
#define POMPA_NUTRISI 14
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
NTPClient timeClient(ntpUDP, "pool.ntp.org", 28800 + 3600); // WITA (GMT+8)

// Variabel untuk sensor ultrasonik
long duration;
float distanceCm;

// Variabel untuk logika kontrol
float suhuAir = 0;
float tdsValue = 0;
float suhuUdara = 0;
float kelembapanUdara = 0;
float airTinggi = 0;

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
  pinMode(POMPA_NUTRISI, OUTPUT);
  pinMode(POMPA_AIR_BERSIH, OUTPUT);
  pinMode(MOTOR_DC, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(POMPA_NUTRISI, LOW);
  digitalWrite(POMPA_AIR_BERSIH, LOW);
  digitalWrite(MOTOR_DC, LOW);

  // Koneksi Wi-Fi
  Serial.println("Memulai koneksi Wi-Fi...");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nTerhubung ke Wi-Fi");

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

  if (isnan(suhuUdara) || isnan(kelembapanUdara)) {
    Serial.println("Error membaca sensor DHT22");
  }

  ds18b20.requestTemperatures();
  suhuAir = ds18b20.getTempCByIndex(0);
  if (suhuAir == DEVICE_DISCONNECTED_C) {
    Serial.println("Sensor DS18B20 tidak terhubung");
    suhuAir = 0;
  }

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
    delay(500);
    digitalWrite(POMPA_AIR_BERSIH, LOW);
  }

  if (tdsValue < 560) {
    Serial.println("Pompa nutrisi diaktifkan.");
    digitalWrite(POMPA_NUTRISI, HIGH);
    delay(500);
    digitalWrite(POMPA_NUTRISI, LOW);
  }

  Serial.println("===== Data Sensor =====");
  Serial.print("Waktu: "); Serial.println(timestamp);
  Serial.print("Suhu Udara: "); Serial.println(suhuUdara);
  Serial.print("Kelembapan Udara: "); Serial.println(kelembapanUdara);
  Serial.print("Suhu Air: "); Serial.println(suhuAir);
  Serial.print("Nilai TDS: "); Serial.println(tdsValue);
  Serial.print("Ketinggian Air: "); Serial.println(airTinggi);
  Serial.println("=======================");

  if (millis() - lastSendTime >= interval) { // Interval 5 menit
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
