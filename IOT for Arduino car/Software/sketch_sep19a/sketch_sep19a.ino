#include <ESP8266WiFi.h>          // ESP8266 Wi-Fi kütüphanesi
#include <ESP8266HTTPClient.h>    // HTTP istemcisi kütüphanesi
#include <PubSubClient.h>         // MQTT kütüphanesi

// Wi-Fi ağ bilgileri
const char* ssid = "Redmi Note 9";       // Wi-Fi SSID (ağ adı)
const char* password = "canatan25";       // Wi-Fi şifresi

// MQTT sunucu bilgileri
const char* mqttServer = "mqtt.prodiot.com";  // MQTT sunucusunun adresi
const int mqttPort = 1883;                     // MQTT sunucusunun portu
const char* mqttUser = "Crea";                // MQTT kullanıcı adı
const char* mqttPassword = "Crea!12";         // MQTT şifresi

WiFiClient espClient;          // Wi-Fi istemcisi nesnesi
PubSubClient client(espClient);  // MQTT istemcisi, Wi-Fi istemcisine bağlı

void setup() {
  Serial.begin(115200);  // Seri haberleşme başlatılıyor, baud hızı 115200

  // Wi-Fi Bağlantısı kuruluyor
  WiFi.begin(ssid, password);  // Wi-Fi ağ adı ve şifre ile bağlantı başlatılıyor
  while (WiFi.status() != WL_CONNECTED) {  // Wi-Fi'ye bağlanana kadar bekleniyor
    delay(500);
    Serial.println("Wi-Fi'ye Bağlanıyor...");  // Bağlantı durumu seri port üzerinden yazdırılıyor
  }
  Serial.println("Wi-Fi'ye Bağlandı!");  // Bağlantı sağlandığında mesaj yazdırılıyor

  // MQTT Bağlantısı kuruluyor
  client.setServer(mqttServer, mqttPort);  // MQTT sunucu adresi ve portu ayarlanıyor
  while (!client.connected()) {  // MQTT'ye bağlanana kadar bekleniyor
    Serial.println("MQTT'ye Bağlanıyor...");  // Bağlantı durumu seri port üzerinden yazdırılıyor

    // MQTT bağlantısı kuruluyor
    if (client.connect("ESP8266Client", mqttUser, mqttPassword)) {
      Serial.println("MQTT'ye Bağlandı!");  // Bağlantı başarılı olursa mesaj yazdırılıyor
    } else {
      Serial.print("Bağlanamadı, Hata Kodu: ");  // Bağlantı başarısız olursa hata kodu yazdırılıyor
      Serial.print(client.state());
      delay(2000);  // Tekrar denemeden önce 2 saniye bekleniyor
    }
  }

  // MQTT bağlantısı başarılıysa bir mesaj gönderiliyor
  client.publish("pi/770fe854-9e21-42b7-8d42-f5e0651d98fc/CR03-DEVICE1", "EmreC.deneme");
  Serial.println("MQTT'ye Gönderildi!");  // Mesajın gönderildiği belirtiliyor

  // HTTP İsteği yapılıyor
  if (WiFi.status() == WL_CONNECTED) {  // Wi-Fi bağlantısı kontrol ediliyor
    WiFiClient client;  // Wi-Fi istemcisi nesnesi oluşturuluyor
    HTTPClient http;    // HTTP istemcisi nesnesi oluşturuluyor

    // GET İsteği Gönderiliyor
    http.begin(client, "http://httpbin.org/get"); // GET isteği için URL ayarlanıyor
    int httpCode = http.GET();  // GET isteği gönderiliyor

    if (httpCode > 0) {  // Sunucudan başarılı bir cevap alındı mı?
        String payload = http.getString();  // Sunucudan gelen cevap alınarak string'e çevriliyor
        Serial.println("GET isteği başarılı, cevap:");  // Başarı mesajı yazdırılıyor
        Serial.println(payload);  // Sunucunun cevabı yazdırılıyor
        Serial.println(httpCode);  // HTTP durumu (200, 404 vb.) yazdırılıyor

    } else {  // GET isteği başarısız olursa
        Serial.println("GET isteği başarısız");  // Başarısızlık mesajı yazdırılıyor
        Serial.print("Hata Kodu: ");
        Serial.println(httpCode);  // Hata kodu yazdırılıyor
    }

    http.end();  // HTTP isteği sonlandırılıyor
  }
}

void loop() {
  // Döngüde MQTT client'ı çalıştırın
  client.loop();  // MQTT istemcisini döngüde çalıştırarak bağlantıyı aktif tutun

  // İsterseniz burada tekrar HTTP isteği gönderme veya MQTT mesajları dinleme gibi işlemler yapabilirsiniz.
}
