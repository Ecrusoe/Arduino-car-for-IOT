#include <ESP8266WiFi.h>  // ESP8266 WiFi kütüphanesi

// WiFi ayarları
const char* ssid = "CM_Staj";        // WiFi ağ adı (SSID)
const char* password = "patSo1#c3";          // WiFi şifresi

void setup() {
  // Seri haberleşmeyi başlat
  Serial.begin(115200);
  delay(100);

  // WiFi'yi başlat ve ağa bağlan
  Serial.println();
  Serial.print("WiFi'ye bağlanılıyor: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  // Bağlanana kadar bekle
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  // Bağlanınca IP adresini yazdır
  Serial.println();
  Serial.println("WiFi'ye başarıyla bağlanıldı!");
  Serial.print("IP Adresi: ");
  Serial.println(WiFi.localIP());  // Cihazın aldığı IP adresini gösterir
}

void loop() {
  // Ana döngüde herhangi bir işlem yapmanıza gerek yok
}
