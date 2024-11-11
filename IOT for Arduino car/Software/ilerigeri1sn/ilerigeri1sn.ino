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
const int ENA = 16; // Motor sürücü kartında ENA (PWM) - GPIO16 (OUT1 ve OUT2 için)
const int IN1 = 5;  // Motor sürücü kartında IN1 - GPIO5 (OUT1 için)
const int IN2 = 4;  // Motor sürücü kartında IN2 - GPIO4 (OUT2 için)
const int IN3 = 0;  // OUT3 motor kontrolü için IN3 - GPIO0 (Motor 2 için)
const int IN4 = 2;  // OUT4 motor kontrolü için IN4 - GPIO2 (Motor 2 için)
const int ENB = 14; // Motor sürücü kartında ENB (PWM) - GPIO14 (OUT3 ve OUT4 için)
const int SERVO_PIN = 15; // Servo motor (GPIO15)

Servo myServo; // Servo nesnesi
int motorSpeed = 1023; // Motor hızı maksimum (1023)
int servoAngle = 90; // Servo motorun başlangıç açısı (90 derece)

unsigned long forwardStartTime = 0;
unsigned long backwardStartTime = 0;
bool isForward = false;
bool isBackward = false;

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
    myServo.write(servoAngle);  // Servo motoru başlangıç pozisyonuna ayarla (90 derece)

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
    
    unsigned long currentTime = millis();

    // Motorlar ileri gidiyor mu kontrol et ve durdur
    if (isForward && (currentTime - forwardStartTime >= 1000)) {
        stopMotors();
        isForward = false;
        Serial.println("Motorlar durdu.");
    }

    // Motorlar geri gidiyor mu kontrol et ve durdur
    if (isBackward && (currentTime - backwardStartTime >= 1000)) {
        stopMotors();
        isBackward = false;
        Serial.println("Motorlar durdu.");
    }
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

    // Gelen mesajın kontrolü
    if (doc.containsKey("goforward")) {
        int goForward = doc["goforward"].as<int>();
        if (goForward == 1) {
            // İki DC motoru ileri sür
            digitalWrite(IN1, HIGH);
            digitalWrite(IN2, LOW);
            digitalWrite(IN3, HIGH);
            digitalWrite(IN4, LOW);
            analogWrite(ENA, motorSpeed); // OUT1 ve OUT2 motor hızı
            analogWrite(ENB, motorSpeed); // OUT3 ve OUT4 motor hızı
            Serial.println("Motorlar ileri gidiyor.");
            forwardStartTime = millis();  // Zamanı kaydet
            isForward = true;
        }
    }

    if (doc.containsKey("goback")) {
        int goBack = doc["goback"].as<int>();
        if (goBack == 1) {
            // İki DC motoru geri sür
            digitalWrite(IN1, LOW);
            digitalWrite(IN2, HIGH);
            digitalWrite(IN3, LOW);
            digitalWrite(IN4, HIGH);
            analogWrite(ENA, motorSpeed); // OUT1 ve OUT2 motor hızı
            analogWrite(ENB, motorSpeed); // OUT3 ve OUT4 motor hızı
            Serial.println("Motorlar geri gidiyor.");
            backwardStartTime = millis();  // Zamanı kaydet
            isBackward = true;
        }
    }

    if (doc.containsKey("steerright")) {
        int steerRight = doc["steerright"].as<int>();
        if (steerRight == 1) {
            // Servo motoru sağa 90 derece döndür
            servoAngle = 180; // Sağ maksimum 180 derece
            myServo.write(servoAngle);
            Serial.println("Servo motor sağa 90 derece döndü.");
        }
    }

    if (doc.containsKey("steerleft")) {
        int steerLeft = doc["steerleft"].as<int>();
        if (steerLeft == 1) {
            // Servo motoru sola 90 derece döndür
            servoAngle = 0; // Sol maksimum 0 derece
            myServo.write(servoAngle);
            Serial.println("Servo motor sola 90 derece döndü.");
        }
    }
}

void stopMotors() {
    // Motorları durdur
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
}
