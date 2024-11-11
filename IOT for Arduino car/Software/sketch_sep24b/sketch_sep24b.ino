#include <SoftwareSerial.h>

String agAdi = "Redmi NOte 9";  // Kablosuz ağ adı
String agSifresi = "canatan25"; // Kablosuz ağ şifresi

int rxPin = 10;  // ESP8266 TX pini
int txPin = 11;  // ESP8266 RX pini

SoftwareSerial esp(rxPin, txPin);  // Seri haberleşme için pin ayarları

void setup() {
  Serial.begin(9600);  // Seri port haberleşmesini başlat
  esp.begin(9600);  // ESP8266 ile seri haberleşmeyi başlat
  delay(1000);  // Stabilizasyon için kısa bir bekleme süresi

  Serial.println("AT Komutu Gönderiliyor...");
  esp.println("AT");  
  delay(1000);  // Yanıt alabilmek için kısa bir bekleme süresi

  while (esp.available()) {
    Serial.write(esp.read());  // ESP'den gelen yanıtı seri monitörde yazdır
  }

  if (!esp.find("OK")) {  
    Serial.println("ESP8266 ile iletişim kurulamadı.");  // Hata mesajı
    while (true);  // Döngüde bekle
  }

  Serial.println("OK Komutu Alındı");
  
  esp.println("AT+CWMODE=1");  
  delay(1000);  

  while (esp.available()) {
    Serial.write(esp.read());  // ESP'den gelen yanıtı yazdır
  }

  if (!esp.find("OK")) {  
    Serial.println("Modül ayarı yapılamadı.");  // Hata mesajı
    while (true);  // Döngüde bekle
  }

  Serial.println("Modül Client olarak ayarlandı");
  Serial.println("Ağa Bağlanılıyor...");
  
  esp.println("AT+CWJAP=\"" + agAdi + "\",\"" + agSifresi + "\"");  
  delay(5000);  // Ağa bağlanma süresi için bekle

  while (esp.available()) {
    Serial.write(esp.read());  // ESP'den gelen yanıtı yazdır
  }

  if (!esp.find("OK")) {  
    Serial.println("Ağa bağlanamadı.");  // Hata mesajı
    while (true);  // Döngüde bekle
  }

  Serial.println("Ağa Bağlanıldı.");
}

void loop() {
  String mqttServer = "MQTT_BAĞLANTI_ADRESİ";  // MQTT sunucusunun adresini buraya yazın.
  int mqttPort = 1883;  // MQTT sunucu portu, genellikle 1883'tür.

  esp.println("AT+CIPSTART=\"TCP\",\"" + mqttServer + "\"," + String(mqttPort));  // MQTT sunucusuna TCP ile bağlanıyoruz.
  
  delay(1000);  // Yanıt alabilmek için kısa bir bekleme süresi

  while (esp.available()) {
    Serial.write(esp.read());  // ESP'den gelen yanıtı yazdır
  }

  if(esp.find("Error")) {  
    Serial.println("AT+CIPSTART Error");
  } else {
    Serial.println("Bağlantı Kuruldu, MQTT sunucusuna bağlanıldı.");
    // Buraya MQTT ile ilgili gönderim veya alım işlemlerini ekleyebilirsiniz.
  }

  esp.println("AT+CIPCLOSE");  // Bağlantıyı kapatıyoruz.
  delay(10000);  // 10 saniye bekle
}

