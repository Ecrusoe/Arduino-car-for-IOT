#include <ESP8266WiFi.h>  // ESP8266 Wi-Fi kütüphanesi

// Wi-Fi ayarları
const char* ssid = "WiFi_ADI";       // Wi-Fi ağ adını buraya yaz
const char* password = "WiFi_SIFRESI";  // Wi-Fi şifresini buraya yaz

void setup() {
  Serial.begin(115200);  // Seri haberleşmeyi başlat
  delay(10);
  
  // Wi-Fi bağlantısı başlatılıyor
  Serial.println();
  Serial.println("Wi-Fi'ye bağlanılıyor...");
  WiFi.begin(ssid, password);

  // Wi-Fi'ye bağlanmayı dene
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Wi-Fi'ye bağlanıldı
  Serial.println("");
  Serial.println("Wi-Fi'ye başarıyla bağlandı!");
  Serial.print("IP Adresi: ");
  Serial.println(WiFi.localIP());  // Cihazın IP adresini yazdır
}

void loop() {
  // Buraya ana kodlarını yazabilirsin
}
