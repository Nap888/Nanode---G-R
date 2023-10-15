#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // TX, RX

void setup() {
  Serial.begin(9600); // Инициализируем встроенный Serial Monitor
  mySerial.begin(9600); // Инициализируем SoftwareSerial
}

void loop() {
  // Чтение данных с геофона (аналоговый пин A0)
  int geophoneValue = analogRead(A0);
  
  // Чтение данных с сейсмографа (аналоговый пин A2)
  int seismicValue = analogRead(A2);
  
  // Отправка данных на SoftwareSerial порт
  mySerial.print("Geophone: ");
  mySerial.println(geophoneValue);
  mySerial.print("Seismic: ");
  mySerial.println(seismicValue);

  // Задержка для стабилизации данных (по вашему выбору)
  delay(1500);
}
