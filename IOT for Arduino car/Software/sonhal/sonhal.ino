#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

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

int motorSpeed = 1023; // Motor hızı maksimum (1023)
bool motorActive = false; // Motorun durumu (başlangıçta kapalı)

// Wi-Fi ve MQTT nesneleri
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
    // Seri haberleşmeyi başlat
    Serial.begin(115200);

    // Motor pinlerini başlat
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(ENA, OUTPUT);

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
            // MQTT konusuna abone ol
            client.subscribe("pi/770fe854-9e21-42b7-8d42-f5e0651d98fc/CR03-DEVICE1");
        } else {
            Serial.print("Bağlanamadı, Hata Kodu: ");
            Serial.println(client.state());
            delay(2000);
        }
    }
}

void loop() {
    client.loop(); // MQTT bağlantısını canlı tut
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Mesaj alındı: ");
    Serial.print("Topic: ");
    Serial.println(topic);

    // Gelen veriyi işleyelim
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payload, length);

    if (error) {
        Serial.print("JSON ayrıştırma hatası: ");
        Serial.println(error.c_str());
        return;
    }

    // Mesajın içeriğine göre motor durumunu güncelle
    if (doc.containsKey("motor1")) {
        int motor1Status = doc["motor1"].as<int>();

        if (motor1Status == 1) {
            // Motorun mevcut durumunu tersine çevir
            motorActive = !motorActive;

            if (motorActive) {
                // Motoru çalıştır
                digitalWrite(IN1, HIGH);
                digitalWrite(IN2, LOW);
                analogWrite(ENA, motorSpeed);
                Serial.println("Motor çalıştırıldı.");
            } else {
                // Motoru durdur
                digitalWrite(IN1, LOW);
                digitalWrite(IN2, LOW);
                Serial.println("Motor durduruldu.");
            }
        }
    }
}

