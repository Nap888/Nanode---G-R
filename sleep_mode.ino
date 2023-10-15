#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

const int analogPin = 2; // Аналоговый пин, на который подключена кнопка
const int ledPin = 13; // Пин для светодиода (встроенный на большинстве Arduino Nano)

void setup() {
  pinMode(ledPin, OUTPUT); // Устанавливаем пин для светодиода как выход
  mySwitch.enableTransmit(10); // Устанавливаем пин передатчика, например, D10
  mySwitch.setProtocol(1); // Устанавливаем протокол, может потребоваться настроить
  mySwitch.setRepeatTransmit(2); // Устанавливаем число повторений
  digitalWrite(ledPin, LOW); // Выключаем светодиод при запуске
}

void loop() {
  int buttonValue = analogRead(analogPin); // Считываем значение с аналогового пина

  if (buttonValue > 0) {
    // Значение аналогового пина отличается от 0, отправляем сигнал и включаем светодиод
    digitalWrite(ledPin, HIGH); // Включаем светодиод
    mySwitch.send(0x1, 24); // Измените адрес и длину битов по вашим требованиям
    delay(200); // Задержка для избежания многократной отправки при удержании кнопки
    digitalWrite(ledPin, LOW); // Выключаем светодиод
  }

  // Входим в режим сна, чтобы снизить потребление энергии
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
  sleep_disable();
}
