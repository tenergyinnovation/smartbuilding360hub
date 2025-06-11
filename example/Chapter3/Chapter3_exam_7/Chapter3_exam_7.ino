#include <Arduino.h>

const int pinBuzzer = 13;  // GPIO13 ต่อกับบัซเซอร์

void setup() {
  pinMode(pinBuzzer, OUTPUT);      // ตั้งพินเป็นเอาต์พุต
  digitalWrite(pinBuzzer, LOW);    // ปิดเสียงบัซเซอร์เริ่มต้น
}

void loop() {
  // ทดสอบเสียงเตือน 3 ครั้ง
  for (int i = 0; i < 3; i++) {
    tone(pinBuzzer, 2000);   // ปล่อยสัญญาณความถี่ 2 kHz
    delay(200);              // ดัง 200 ms
    noTone(pinBuzzer);       // หยุดเสียง
    delay(200);              // หยุด 200 ms ก่อนครั้งถัดไป
  }

  delay(2000);               // หน่วง 2 วินาที ก่อนวนลูปใหม่
}