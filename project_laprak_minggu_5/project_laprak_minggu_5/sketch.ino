#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

// WiFi Wokwi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// MQTT HiveMQ
const char* mqtt_server = "broker.hivemq.com";
const char* topic_servo = "iot/parking/servo";

WiFiClient espClient;
PubSubClient client(espClient);
Servo myServo;

const int servoPin = 33;
bool isOpen = false;
unsigned long lastToggleTime = 0;
unsigned long toggleInterval = 5000;  // 5 detik

void reconnect() {
  while (!client.connected()) {
    Serial.print("🔄 Menghubungkan MQTT...");
    if (client.connect("esp32ClientAuto")) {
      Serial.println("✅ MQTT Terhubung!");
    } else {
      Serial.print("❌ Gagal, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Servo Setup
  myServo.attach(servoPin);
  myServo.write(0);  // mulai dari TERTUTUP
  Serial.println("✅ Servo terpasang di pin 33");

  // WiFi Setup
  WiFi.begin(ssid, password);
  Serial.print("🌐 Menghubungkan WiFi");
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - wifiStart > 10000) {
      Serial.println("\n⚠️ Gagal konek WiFi. Lanjut tanpa internet.");
      break;
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi Tersambung");
  }

  // MQTT Setup
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) reconnect();
    client.loop();
  }

  unsigned long currentTime = millis();
  if (currentTime - lastToggleTime >= toggleInterval) {
    lastToggleTime = currentTime;

    if (isOpen) {
      myServo.write(0); // TUTUP
      if (client.connected()) client.publish(topic_servo, "TUTUP");
      Serial.println("🔒 Servo Tertutup");
    } else {
      myServo.write(90); // BUKA
      if (client.connected()) client.publish(topic_servo, "BUKA");
      Serial.println("🔓 Servo Terbuka");
    }

    isOpen = !isOpen; // toggle status
  }

  delay(50);
}
