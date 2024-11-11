#include <Servo.h>
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
const int IN3 = 0;  // OUT3 motor kontrolü için IN3 - GPIO0
const int IN4 = 2;  // OUT4 motor kontrolü için IN4 - GPIO2
const int ENB = 14; // Motor sürücü kartında ENB (PWM) - GPIO14 (OUT3 ve OUT4 için)
const int SERVO_PIN = 15; // Servo motor (GPIO15)

Servo myServo; // Servo nesnesi

int motorSpeed = 1023; // Motor hızı maksimum (1023)
bool motorActive = false; // Motorun durumu (başlangıçta kapalı)
bool motor2Active = false; // OUT3/OUT4 motor durumu (başlangıçta kapalı)
bool servoActive = false; // Servo motorun durumu (başlangıçta kapalı)
int pressCount = 0; // Butona basma sayacı

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
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENB, OUTPUT); // ENB pinini de çıkış olarak ayarla

    // Servo motoru başlat
    myServo.attach(SERVO_PIN);

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

    // Mesajın içeriğine göre motor ve servo durumunu güncelle
    if (doc.containsKey("motor1")) {
        int motor1Status = doc["motor1"].as<int>();

        if (motor1Status == 1) {
            pressCount++;  // Butona basma sayısını artır

            if (pressCount % 2 == 1) {
                // Servo motoru 90 dereceye döndür
                myServo.write(90);
                Serial.println("Servo motor 90 dereceye döndü.");

                // OUT3 ve OUT4'e bağlı motoru çalıştır
                digitalWrite(IN3, HIGH);
                digitalWrite(IN4, LOW);
               motor2Active = true;
                Serial.println("OUT3/OUT4'e bağlı motor çalıştırıldı.");

                //OUT1 ve OUT2'ye bağlı motoru çalıştır
                digitalWrite(IN1, HIGH);
                digitalWrite(IN2, LOW);
                analogWrite(ENA, motorSpeed);
                motorActive = true;
                Serial.println("OUT1/OUT2'ye bağlı motor çalıştırıldı.");

            } else {
                // Servo motoru 0 dereceye döndür
                myServo.write(0);
                Serial.println("Servo motor 0 dereceye döndü.");

                // OUT3 ve OUT4'e bağlı motoru durdur
                digitalWrite(IN3, LOW);
                digitalWrite(IN4, LOW);
                motor2Active = false;
                Serial.println("OUT3/OUT4'e bağlı motor durduruldu.");

                // OUT1 ve OUT2'ye bağlı motoru durdur
                digitalWrite(IN1, LOW);
                digitalWrite(IN2, LOW);
                analogWrite(ENA, 0);
                motorActive = false;
                Serial.println("OUT1/OUT2'ye bağlı motor durduruldu.");
            }
        }
    }
}
