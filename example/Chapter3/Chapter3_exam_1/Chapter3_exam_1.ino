#include <Arduino.h> // เรียกใช้ไลบรารีพื้นฐานของ Arduino

void setup() {
  Serial.begin(115200); // เริ่มต้นการสื่อสาร Serial ที่ความเร็ว 115200 baud
}

void loop() {
  Serial.println("Hello World"); // ส่งข้อความ "Hello World" ไปยัง Serial Monitor
  delay(1000); // รอ 1 วินาทีก่อนส่งข้อความอีกครั้ง
}