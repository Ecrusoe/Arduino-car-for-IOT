#include <Servo.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Wi-Fi bağlantı bilgileri
const char* ssid = "Redmi Note 9"; // Wi-Fi SSID
const char* password = "canatan25"; // Wi-Fi şifresi

// MQTT sunucu bilgileri
const char* mqttServer = "mqtt.prodiot.com"; // MQTT sunucu adresi
const int mqttPort = 1883; // MQTT sunucu portu
const char* mqttUser = "Crea"; // MQTT kullanıcı adı
const char* mqttPassword = "Crea!12"; // MQTT şifresi

// Cihazlar için pin tanımlamaları (ESP8266 pinleri)
const int ENA = 16; // Motor sürücü kartında ENA (PWM) - GPIO16 (OUT1 ve OUT2 için)
const int IN1 = 5;  // Motor sürücü kartında IN1 - GPIO5 (OUT1 için)
const int IN2 = 4;  // Motor sürücü kartında IN2 - GPIO4 (OUT2 için)
const int IN3 = 0;  // OUT3 motor kontrolü için IN3 - GPIO0 (Motor 2 için)
const int IN4 = 2;  // OUT4 motor kontrolü için IN4 - GPIO2 (Motor 2 için)
const int ENB = 14; // Motor sürücü kartında ENB (PWM) - GPIO14 (OUT3 ve OUT4 için)
const int SERVO_PIN = 15; // Servo motor pin (GPIO15)

Servo myServo; // Servo nesnesi
int motorSpeed = 1023; // Motor hızı maksimum (1023)
int servoAngle = 90; // Servo motorun başlangıç açısı (90 derece)

unsigned long forwardStartTime = 0; // Motor ileri giderken başlangıç zamanı
unsigned long backwardStartTime = 0; // Motor geri giderken başlangıç zamanı
bool isForward = false; // Motorun ileri hareket durumu
bool isBackward = false; // Motorun geri hareket durumu

unsigned long servoMoveStartTime = 0; // Servo hareket zamanı
bool servoMoved = false; // Servo hareket etti mi

WiFiClient espClient; // Wi-Fi istemcisi
PubSubClient client(espClient); // MQTT istemcisi

void setup() {
    // Seri haberleşmeyi başlat
    Serial.begin(115200);

    // Motor pinlerini başlat
    pinMode(IN1, OUTPUT); // IN1 pinini çıkış olarak ayarla
    pinMode(IN2, OUTPUT); // IN2 pinini çıkış olarak ayarla
    pinMode(ENA, OUTPUT); // ENA pinini çıkış olarak ayarla
    pinMode(IN3, OUTPUT); // IN3 pinini çıkış olarak ayarla
    pinMode(IN4, OUTPUT); // IN4 pinini çıkış olarak ayarla
    pinMode(ENB, OUTPUT); // ENB pinini çıkış olarak ayarla

    // Servo motoru başlat
    myServo.attach(SERVO_PIN); // Servo motoru belirtilen pine bağla
    myServo.write(servoAngle);  // Servo motoru başlangıç pozisyonuna ayarla (90 derece)

    // Wi-Fi'ye bağlan
    WiFi.begin(ssid, password); // Wi-Fi bağlantısını başlat
    while (WiFi.status() != WL_CONNECTED) { // Wi-Fi bağlantısı sağlanana kadar bekle
        delay(500); // 500 ms bekle
        Serial.println("Wi-Fi'ye Bağlanıyor..."); // Bağlantı durumu mesajı
    }
    Serial.println("Wi-Fi'ye Bağlandı!"); // Bağlantı başarılı

    // MQTT sunucusuna bağlan
    client.setServer(mqttServer, mqttPort); // MQTT sunucu bilgilerini ayarla
    client.setCallback(callback); // Callback fonksiyonunu ayarla
    while (!client.connected()) { // MQTT bağlantısı sağlanana kadar bekle
        Serial.println("MQTT'ye Bağlanıyor..."); // Bağlantı durumu mesajı
        if (client.connect("ESP8266Client", mqttUser, mqttPassword)) { // MQTT sunucusuna bağlan
            Serial.println("MQTT'ye Bağlandı!"); // Bağlantı başarılı
            // MQTT konusuna abone ol
            client.subscribe("pi/770fe854-9e21-42b7-8d42-f5e0651d98fc/CR03-DEVICE1"); // Belirtilen konuya abone ol
        } else {
            Serial.print("Bağlanamadı, Hata Kodu: "); // Bağlantı hatası mesajı
            Serial.println(client.state()); // Hata kodunu yazdır
            delay(2000); // 2 saniye bekle
        }
    }
}

void loop() {
    client.loop(); // MQTT bağlantısını canlı tut
    
    unsigned long currentTime = millis(); // Geçerli zamanı al

    // Motorlar ileri gidiyor mu kontrol et ve durdur
    if (isForward && (currentTime - forwardStartTime >= 1000)) {
        stopMotors(); // Motorları durdur
        isForward = false; // İleri hareketi durdur
        Serial.println("Motorlar durdu."); // Durum mesajı
    }

    // Motorlar geri gidiyor mu kontrol et ve durdur
    if (isBackward && (currentTime - backwardStartTime >= 1000)) {
        stopMotors(); // Motorları durdur
        isBackward = false; // Geri hareketi durdur
        Serial.println("Motorlar durdu."); // Durum mesajı
    }

    // Servo motor hareket ettikten sonra geri dönmesi için kontrol
    if (servoMoved && (currentTime - servoMoveStartTime >= 1000)) {
        servoAngle = 90; // Eski haline geri dön (90 derece)
        myServo.write(servoAngle); // Servo motoru geri döndür
        servoMoved = false; // Hareket tamamlandı
        Serial.println("Servo motor eski pozisyonuna döndü (90 derece)."); // Durum mesajı
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Mesaj alındı: "); // Mesaj alındı durumu
    Serial.print("Topic: "); // Gelen mesajın konusunu yazdır
    Serial.println(topic);

    // Gelen veriyi işleyelim
    StaticJsonDocument<200> doc; // JSON belgesi tanımla
    DeserializationError error = deserializeJson(doc, payload, length); // Gelen veriyi JSON'a dönüştür

    if (error) {
        Serial.print("JSON ayrıştırma hatası: "); // JSON ayrıştırma hatası durumu
        Serial.println(error.c_str()); // Hata mesajını yazdır
        return; // Hata durumunda çık
    }

    // Gelen mesajın kontrolü
    if (doc.containsKey("goforward")) {
        int goForward = doc["goforward"].as<int>(); // "goforward" anahtarına karşılık gelen değeri al
        if (goForward == 1) { // Eğer değer 1 ise
            // İki DC motoru ileri sür
            digitalWrite(IN1, HIGH); // IN1 pinini yüksek yap
            digitalWrite(IN2, LOW);  // IN2 pinini düşük yap
            digitalWrite(IN3, HIGH); // IN3 pinini yüksek yap
            digitalWrite(IN4, LOW);  // IN4 pinini düşük yap
            analogWrite(ENA, motorSpeed); // OUT1 ve OUT2 motor hızı
            analogWrite(ENB, motorSpeed); // OUT3 ve OUT4 motor hızı
            Serial.println("Motorlar ileri gidiyor."); // Durum mesajı
            forwardStartTime = millis();  // Zamanı kaydet
            isForward = true; // İleri hareket durumunu güncelle
        }
    }

    if (doc.containsKey("goback")) {
        int goBack = doc["goback"].as<int>(); // "goback" anahtarına karşılık gelen değeri al
        if (goBack == 1) { // Eğer değer 1 ise
            // İki DC motoru geri sür
            digitalWrite(IN1, LOW); // IN1 pinini düşük yap
            digitalWrite(IN2, HIGH); // IN2 pinini yüksek yap
            digitalWrite(IN3, LOW); // IN3 pinini düşük yap
            digitalWrite(IN4, HIGH); // IN4 pinini yüksek yap
            analogWrite(ENA, motorSpeed); // OUT1 ve OUT2 motor hızı
            analogWrite(ENB, motorSpeed); // OUT3 ve OUT4 motor hızı
            Serial.println("Motorlar geri gidiyor."); // Durum mesajı
            backwardStartTime = millis();  // Zamanı kaydet
            isBackward = true; // Geri hareket durumunu güncelle
        }
    }

    if (doc.containsKey("steerleft")) {
        int steerLeft = doc["steerleft"].as<int>(); // "steerleft" anahtarına karşılık gelen değeri al
        if (steerLeft == 1) { // Eğer değer 1 ise
            // Servo motoru sola döndür
            servoAngle = 80; // Yeni açı değeri
            myServo.write(servoAngle); // Servo motoru yeni açıya döndür
            Serial.println("Servo motor sola döndü (80 derece)."); // Durum mesajı
            servoMoved = true; // Servo hareketi tamamlandı
            servoMoveStartTime = millis(); // Zamanı kaydet
        }
    }

    if (doc.containsKey("steerright")) {
        int steerRight = doc["steerright"].as<int>(); // "steerright" anahtarına karşılık gelen değeri al
        if (steerRight == 1) { // Eğer değer 1 ise
            // Servo motoru sağa döndür
            servoAngle = 100; // Yeni açı değeri
            myServo.write(servoAngle); // Servo motoru yeni açıya döndür
            Serial.println("Servo motor sağa döndü (100 derece)."); // Durum mesajı
            servoMoved = true; // Servo hareketi tamamlandı
            servoMoveStartTime = millis(); // Zamanı kaydet
        }
    }
}

// Motorları durdurma fonksiyonu
void stopMotors() {
    // Motorları durdur
    digitalWrite(IN1, LOW); // IN1 pinini düşük yap
    digitalWrite(IN2, LOW); // IN2 pinini düşük yap
    digitalWrite(IN3, LOW); // IN3 pinini düşük yap
    digitalWrite(IN4, LOW); // IN4 pinini düşük yap
    analogWrite(ENA, 0); // Hızları sıfırla
    analogWrite(ENB, 0); // Hızları sıfırla
}
