#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Pin dan konfigurasi dasar
#define DHTPIN 4       // Pin DHT22
#define DHTTYPE DHT22
#define LDR_PIN 34
#define SDA_PIN 21     // SDA pin untuk I2C LCD
#define SCL_PIN 22     // SCL pin untuk I2C LCD

// Definisikan alamat LCD - untuk LCD 20x4
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Inisialisasi DHT
DHT dht(DHTPIN, DHTTYPE);

// Waktu terakhir pembaruan
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 1000;  // Perbarui setiap 1 detik

// Custom characters
byte thermometer[8] = {
  0b00100,
  0b01010,
  0b01010,
  0b01010,
  0b01110,
  0b11111,
  0b11111,
  0b01110
};

byte droplet[8] = {
  0b00100,
  0b00100,
  0b01010,
  0b01010,
  0b10001,
  0b10001,
  0b10001,
  0b01110
};

byte sunlight[8] = {
  0b00000,
  0b10101,
  0b01110,
  0b11011,
  0b01110,
  0b10101,
  0b00000,
  0b00000
};

byte barChar[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

byte degree[8] = {
  0b00110,
  0b01001,
  0b01001,
  0b00110,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

void setup() {
  Serial.begin(115200);
  Serial.println("Program mulai");

  Wire.begin(SDA_PIN, SCL_PIN);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.createChar(0, thermometer);
  lcd.createChar(1, droplet);
  lcd.createChar(2, sunlight);
  lcd.createChar(3, barChar);
  lcd.createChar(4, degree);

  displaySplashScreen();
  dht.begin();
}

void loop() {
  // Perbarui sensor setiap interval tertentu tanpa blocking delay
  if (millis() - lastUpdate >= updateInterval) {
    lastUpdate = millis();

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    int light = analogRead(LDR_PIN);
    int lightPercent = map(light, 0, 4095, 0, 100);

    // Validasi pembacaan sensor
    if (isnan(h) || isnan(t)) {
      Serial.println("Error: Gagal membaca dari sensor DHT!");
      return;
    }

    Serial.print("H: ");
    Serial.print(h);
    Serial.print("%, T: ");
    Serial.print(t);
    Serial.print("C, L: ");
    Serial.println(lightPercent);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MONITORING SENSOR");

    lcd.setCursor(0, 1);
    lcd.write(0);
    lcd.print(" Suhu: ");
    lcd.print(t, 1);
    lcd.write(4);
    lcd.print("C");
    displayBar(t, 12, 1);

    lcd.setCursor(0, 2);
    lcd.write(1);
    lcd.print(" Kelembapan: ");
    lcd.print(h, 1);
    lcd.print("%");
    displayBar(h, 12, 2);

    lcd.setCursor(0, 3);
    lcd.write(2);
    lcd.print(" Cahaya: ");
    lcd.print(lightPercent);
    lcd.print("%");
    displayBar(lightPercent, 12, 3);
  }
}

void displayBar(float value, int col, int row) {
  int bars = map(value, 0, 100, 0, 10);
  lcd.setCursor(col, row);
  for (int i = 0; i < 10; i++) {
    if (i < bars) {
      lcd.write(3);
    } else {
      lcd.print(" ");
    }
  }
}

void displaySplashScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" SISTEM MONITORING ");
  lcd.setCursor(0, 1);
  lcd.print("    LINGKUNGAN    ");
  lcd.setCursor(0, 3);
  lcd.print("Sugeng Aldi Widodo");
  lcd.setCursor(0, 2);
  lcd.print("  Initializing");
  for (int i = 0; i < 3; i++) {
    delay(2000);
    lcd.print(".");
    delay(2000);
  }
}
