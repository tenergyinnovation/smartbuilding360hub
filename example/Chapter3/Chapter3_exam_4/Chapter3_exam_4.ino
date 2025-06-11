#include <Arduino.h>

const int Relay = 25; // กำหนดหมายเลขขา GPIO สำหรับควบคุมรีเลย์

void setup() {
  Serial.begin(115200);      // เริ่มต้น Serial Monitor ที่ baudrate 115200
  pinMode(Relay, OUTPUT);    // กำหนดขารีเลย์เป็นขาเอาต์พุต
  digitalWrite(Relay, LOW);  // เริ่มต้นให้รีเลย์อยู่ในสถานะปิด (LOW)
}

void loop() {
  digitalWrite(Relay, HIGH); // สั่งให้รีเลย์ทำงาน (จ่ายไฟ HIGH)
  delay(1000);               // หน่วงเวลา 1 วินาที
  digitalWrite(Relay, LOW);  // สั่งให้รีเลย์หยุดทำงาน (จ่ายไฟ LOW)
  delay(1000);               // หน่วงเวลา 1 วินาที
}