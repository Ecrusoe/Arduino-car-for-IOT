#include <Servo.h> // Servo kütüphanesini ekleyin

// Pin tanımlamaları
#define ENA 16  // GPIO16 (ENA)
#define IN1 5   // GPIO5 (IN1)
#define IN2 4   // GPIO4 (IN2)
#define IN3 0   // GPIO0 (IN3) - İkinci motor için
#define IN4 2   // GPIO2 (IN4) - İkinci motor için
#define ENB 14  // GPIO14 (ENB)
#define SERVO_PIN 15 // GPIO15 (Servo motor)

Servo myServo; // Servo nesnesini oluştur
int servoMoveCount = 0;  // Servo motorun hareket sayacı
bool systemStopped = false; // Sistem durduruldu mu?

void setup() {
  // Pin modlarını OUTPUT olarak ayarlıyoruz
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Seri haberleşmeyi başlat
  Serial.begin(115200);

  // Motoru ileri döndür
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 1023);  // Hız kontrolü (maksimum hız)
  Serial.println("Motor 1 ileri hareket ediyor");

  // İkinci motoru da ileri döndür
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 1023);  // Hız kontrolü (maksimum hız)
  Serial.println("Motor 2 ileri hareket ediyor");

  // Servo motoru başlat
  myServo.attach(SERVO_PIN); // Servo motorun pinini ayarla
}

void loop() {
  if (systemStopped) {
    return; // Sistem durduktan sonra hiçbir şey yapma
  }

  if (servoMoveCount < 5) {  // Servo motor 5 kez döndürülmediyse
    // Servo motoru 0 dereceye döndür
    myServo.write(0);
    Serial.print("Servo motor 0 dereceye döndü. Hareket sayısı: ");
    Serial.println(servoMoveCount + 1);  // Hareket sayısını yazdır

    delay(3000); // 3 saniye bekle

    // Servo motoru 90 dereceye döndür
    myServo.write(90);
    Serial.print("Servo motor 90 dereceye döndü. Hareket sayısı: ");
    Serial.println(servoMoveCount + 1);  // Hareket sayısını yazdır

    delay(3000); // 3 saniye bekle

    // Hareket sayacını artır
    servoMoveCount++;
  } else {
    // Servo motor hareketini durdur
    myServo.detach();
    Serial.println("Servo motor hareketi durduruldu.");

    // DC motorları durdur
    analogWrite(ENA, 0);  // Motor 1'i durdur
    analogWrite(ENB, 0);  // Motor 2'yi durdur
    Serial.println("DC motorlar durduruldu.");

    // Sistemi durdur
    systemStopped = true;

    // Sonsuz döngüye girerek servo ve DC motor hareketini tamamen durdur
    while (true) {
      // Sistem burada durur ve loop() tekrar etmez
    }
  }
}

