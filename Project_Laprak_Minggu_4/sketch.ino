#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* kota = "Nganjuk";
const char* apiKey = "dbad0ae804837e14b313844c73aaf5a0";

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Tombol navigasi
const int tombolNext = 12;
const int tombolPrev = 14;

int indeksData = 0;
unsigned long terakhirUpdate = 0;
const unsigned long intervalUpdate = 60000;

// Array nama-nama parameter
String labelCuaca[10] = {
  "Suhu", "Terasa", "Hujan", "Angin",
  "Arah", "Lembab", "Awan", "Tekanan",
  "Terbit", "Terbenam"
};

// Array isi data cuaca
String nilaiCuaca[10];

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  pinMode(tombolNext, INPUT_PULLUP);
  pinMode(tombolPrev, INPUT_PULLUP);

  lcd.setCursor(0, 0);
  lcd.print("Menghubungkan...");
  Serial.println("Mencoba menyambung ke WiFi...");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tersambung WiFi");
  Serial.println("\nWiFi Tersambung");
  delay(1000);
  lcd.clear();

  ambilCuaca();
  tampilkanData();
}

void loop() {
  // Pembaruan otomatis data setiap 1 menit
  if (millis() - terakhirUpdate > intervalUpdate) {
    terakhirUpdate = millis();
    ambilCuaca();
    tampilkanData(); // tampilkan setelah update
  }

  // Navigasi tombol
  if (digitalRead(tombolNext) == LOW) {
    Serial.println("Tombol Next ditekan");
    indeksData = (indeksData + 1) % 10;
    tampilkanData();
    delay(300);
  }
  if (digitalRead(tombolPrev) == LOW) {
    Serial.println("Tombol Prev ditekan");
    indeksData = (indeksData - 1 + 10) % 10;
    tampilkanData();
    delay(300);
  }
}

void ambilCuaca() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://api.openweathermap.org/data/2.5/weather?q=" + String(kota) + "&units=metric&appid=" + apiKey;
    http.begin(url);
    int kode = http.GET();

    if (kode > 0) {
      String respon = http.getString();
      StaticJsonDocument<1024> data;
      DeserializationError err = deserializeJson(data, respon);

      if (!err) {
        JsonObject main = data["main"];
        JsonObject wind = data["wind"];
        JsonObject sys = data["sys"];
        JsonObject clouds = data["clouds"];
        JsonObject rain = data["rain"];

        nilaiCuaca[0] = String(main["temp"].as<float>(), 1) + "C";
        nilaiCuaca[1] = String(main["feels_like"].as<float>(), 1) + "C";
        nilaiCuaca[2] = rain.containsKey("1h") ? String(rain["1h"].as<float>(), 1) + "mm" : "0mm";
        nilaiCuaca[3] = String(wind["speed"].as<float>(), 1) + "m/s";
        nilaiCuaca[4] = String(wind["deg"].as<int>()) + (char)223;
        nilaiCuaca[5] = String(main["humidity"].as<int>()) + "%";
        nilaiCuaca[6] = String(clouds["all"].as<int>()) + "%";
        nilaiCuaca[7] = String(main["pressure"].as<int>()) + "hPa";
        nilaiCuaca[8] = formatWaktu(sys["sunrise"]);
        nilaiCuaca[9] = formatWaktu(sys["sunset"]);

        // Tampilkan ke Serial Monitor semua data
        Serial.println("=== Data Cuaca di " + String(kota) + " ===");
        for (int i = 0; i < 10; i++) {
          Serial.print(labelCuaca[i]);
          Serial.print(": ");
          Serial.println(nilaiCuaca[i]);
        }
        Serial.println("==============================");
      } else {
        Serial.println("Gagal parsing JSON");
      }
    } else {
      Serial.println("Gagal HTTP, kode: " + String(kode));
    }
    http.end();
  } else {
    Serial.println("WiFi tidak terhubung");
  }
}

void tampilkanData() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print(labelCuaca[indeksData]);
  lcd.setCursor(0, 1); lcd.print(nilaiCuaca[indeksData]);

  // Serial Monitor Output
  Serial.print("[LCD] ");
  Serial.print(labelCuaca[indeksData]);
  Serial.print(": ");
  Serial.println(nilaiCuaca[indeksData]);
}

String formatWaktu(unsigned long unixTime) {
  unixTime += 7 * 3600; // UTC+7
  int jam = (unixTime % 86400L) / 3600;
  int menit = (unixTime % 3600) / 60;
  char waktu[6];
  sprintf(waktu, "%02d:%02d", jam, menit);
  return String(waktu);
}
