#include <SoftwareSerial.h>

SoftwareSerial esp8266(2, 3); // RX, TX

const char* ssid = "Redmi Note 9";           // Wi-Fi ağınızın adı
const char* password = "emre25";   // Wi-Fi ağınızın şifresi

void setup() {
  Serial.begin(115200);
  esp8266.begin(115200);

  // ESP8266'yı resetleyin
  esp8266.println("AT+RST");
  delay(1000);
  printResponse();

  // WiFi ağına bağlanın
  esp8266.println("AT+CWMODE=1");
  delay(1000);
  printResponse();

  esp8266.print("AT+CWJAP=\"");
  esp8266.print(ssid);
  esp8266.print("\",\"");
  esp8266.print(password);
  esp8266.println("\"");
  delay(5000);
  printResponse();

  // Bağlantı durumu kontrolü
  esp8266.println("AT+CIFSR");
  delay(1000);
  printResponse();

  // Bağlantı başarılı ise mesaj yazdır
  if (esp8266.available()) {
    String ipAddress = "";
    while (esp8266.available()) {
      char c = esp8266.read();
      ipAddress += c;
    }
    Serial.println("Bağlandı! IP Adresi: " + ipAddress); // Bağlandı mesajı
  }
}

void loop() {
  if (esp8266.available()) {
    Serial.write(esp8266.read());
  }
  if (Serial.available()) {
    esp8266.write(Serial.read());
  }
}

// Yanıtları yazdırmak için yardımcı fonksiyon
void printResponse() {
  while (esp8266.available()) {
    Serial.write(esp8266.read());
  }
  Serial.println(); // Yeni satıra geç
}

