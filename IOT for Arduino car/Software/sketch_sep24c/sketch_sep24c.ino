#include <SoftwareSerial.h>

SoftwareSerial ESP8266(10, 11); // RX, TX

void setup() {
  Serial.begin(9600); // Arduino için seri monitor
  ESP8266.begin(9600);  // Deneme baud hızı
  Serial.println("ESP8266 ile iletişim kurmaya hazır.");
}

void loop() {
  if (ESP8266.available()) {
    Serial.write(ESP8266.read()); // ESP8266'dan gelen veriyi yazdır
  }

  if (Serial.available()) {
    ESP8266.write(Serial.read()); // Seri monitörden gelen veriyi ESP8266'ya gönder
  }
}
