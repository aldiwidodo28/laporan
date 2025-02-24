int Merah = 23;
int Kuning = 22;
int Hijau = 21;

void setup() {
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  
  pinMode(Merah, OUTPUT);
  pinMode(Kuning, OUTPUT);
  pinMode(Hijau, OUTPUT);
}

void loop() {
  digitalWrite(Merah, HIGH);
  digitalWrite(Kuning, LOW);
  digitalWrite(Hijau, LOW);
  delay(3000);
  
  digitalWrite(Merah, LOW);
  digitalWrite(Kuning, HIGH);
  digitalWrite(Hijau, LOW);
  delay(3000);
  
  digitalWrite(Merah, LOW);
  digitalWrite(Kuning, LOW);
  digitalWrite(Hijau, HIGH);
  delay(3000);
}