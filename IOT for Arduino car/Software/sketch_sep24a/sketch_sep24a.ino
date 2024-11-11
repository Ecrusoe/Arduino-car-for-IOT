#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Wi-Fi bağlantı bilgileri
const char* ssid = "Redmi Note 9";
const char* password = "canatan25";

// MQTT sunucu bilgileri
const char* mqttServer = "mqtt.prodiot.com";
const int mqttPort = 1883;
const char* mqttUser = "Crea";
const char* mqttPassword = "Crea!12";

// Motor için pin tanımlaması
int motorPin = 5;  // DC motoru kontrol eden pin

int motorSpeed = 0;  // Motor hızı başlangıçta 0

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // Seri haberleşmeyi başlat
  Serial.begin(115200);

  // DC motor pinini başlat
  pinMode(motorPin, OUTPUT);
  analogWrite(motorPin, motorSpeed);  // Motor hızını başlangıçta sıfırla

  // Wi-Fi'ye bağlan
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Wi-Fi'ye Bağlanıyor...");
  }
  Serial.println("Wi-Fi'ye Bağlandı!");

  // MQTT sunucusuna bağlan
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("MQTT'ye Bağlanıyor...");
    if (client.connect("ESP8266Client", mqttUser, mqttPassword)) {
      Serial.println("MQTT'ye Bağlandı!");
    } else {
      Serial.print("Bağlanamadı, Hata Kodu: ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  // Komutları almak için bir konuya abone ol
  client.subscribe("pi/770fe854-9e21-42b7-8d42-f5e0651d98fc/CR03-DEVICE1");

  // Test mesajı gönder
  client.publish("pi/770fe854-9e21-42b7-8d42-f5e0651d98fc/CR03-DEVICE1", "Motor Kontrolü Başladı");
  Serial.println("MQTT'ye Gönderildi!");
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  Serial.print("Mesaj geldi, konu: ");
  Serial.println(topic);
  Serial.print("Mesaj:");

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    message += (char)payload[i];
  }
  Serial.println();

  // Gelen mesajı işleme
  handleCommand(message);
}

void handleCommand(String command) {
  if (command == "MOTOR_ON") {
    motorSpeed = 255;  // Motoru maksimum hızda çalıştır
    analogWrite(motorPin, motorSpeed);
    Serial.println("Motor çalıştırıldı");
  } else if (command == "MOTOR_OFF") {
    motorSpeed = 0;  // Motoru durdur
    analogWrite(motorPin, motorSpeed);
    Serial.println("Motor durduruldu");
  } else {
    Serial.println("Bilinmeyen komut");
  }
}

void loop() {
  client.loop();  // MQTT bağlantısını canlı tut
}
