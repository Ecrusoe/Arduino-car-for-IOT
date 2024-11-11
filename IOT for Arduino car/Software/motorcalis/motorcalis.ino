#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
#include <ArduinoJson.h> // ArduinoJson kütüphanesini eklemeyi unutma

// Wi-Fi bağlantı bilgileri
const char* ssid = "Redmi Note 9";
const char* password = "canatan25";

// MQTT sunucu bilgileri
const char* mqttServer = "mqtt.prodiot.com";
const int mqttPort = 1883;
const char* mqttUser = "Crea";
const char* mqttPassword = "Crea!12";

// Cihazlar için pin tanımlamaları (ESP8266 pinleri)
const int ENA = 16; // Motor sürücü kartında ENA (PWM) - GPIO16
const int IN1 = 5;  // Motor sürücü kartında IN1 - GPIO5
const int IN2 = 4;  // Motor sürücü kartında IN2 - GPIO4
const int IN3 = 0;  // Motor sürücü kartında IN3 - GPIO0
const int IN4 = 2;  // Motor sürücü kartında IN4 - GPIO2
const int ENB = 14; // Motor sürücü kartında ENB (PWM) - GPIO14

int motorSpeed = 50; // Motor hızı başlangıçta 50

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long previousMillis = 0; // Önceki milisaniye zamanını saklamak için
const long interval = 5000;       // Mesajın 5 saniyede bir gösterilmesi

void setup() {
    // Seri haberleşmeyi başlat
    Serial.begin(115200);

    // Motor pinlerini başlat
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENB, OUTPUT);

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
            // Abone ol
            if (client.subscribe("pi/770fe854-9e21-42b7-8d42-f5e0651d98fc/CR03-DEVICE1")) {
                Serial.println("Abone olundu!");
            } else {
                Serial.println("Abone olunamadı.");
            }
        } else {
            Serial.print("Bağlanamadı, Hata Kodu: ");
            Serial.print(client.state());
            delay(2000);
        }
    }

    // Test mesajı gönder
    client.publish("pi/770fe854-9e21-42b7-8d42-f5e0651d98fc/CR03-DEVICE1", "{\"motor1\":\"1\"}");
    Serial.println("MQTT'ye Gönderildi!");
}

void loop() {
    // Zamanı al
    unsigned long currentMillis = millis();

    // 5 saniyelik aralığı kontrol et
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis; // Önceki zamanı güncelle
        Serial.println("Loop çalışıyor..."); // Debugging için
    }

    client.loop(); // MQTT bağlantısını canlı tut
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Mesaj alındı: ");
    Serial.print("Topic: ");
    Serial.print(topic);
    Serial.print(" Mesaj: ");
    
    // Payload'u string olarak yazdırma
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
    
    // Gelen mesajı işleme
    if (strcmp(topic, "pi/770fe854-9e21-42b7-8d42-f5e0651d98fc/CR03-DEVICE1") == 0) {
        StaticJsonDocument<200> doc;  // JSON dökümanı oluştur
        DeserializationError error = deserializeJson(doc, payload, length);

        if (error) {
            Serial.print("JSON ayrıştırma hatası: ");
            Serial.println(error.c_str());
            return;
        }

        // motor1 değeri varsa işle
        if (doc.containsKey("motor1")) {
            int motor1Status = doc["motor1"];

            if (motor1Status == 1) {
                // Motoru çalıştır (OUT2 kullanarak)
                digitalWrite(IN1, HIGH);
                digitalWrite(IN2, LOW);
                analogWrite(ENA, motorSpeed); // Motor hızını PWM sinyaliyle ayarla
                Serial.println("Motor çalışıyor...");
            } else {
                // Motoru durdur
                digitalWrite(IN1, LOW);
                digitalWrite(IN2, LOW);
                Serial.println("Motor durdu.");
            }
        }
    }
}
