#include <Arduino.h>

// ฟังก์ชัน setup() จะทำงานเพียงครั้งเดียวเมื่อเริ่มต้นโปรแกรม
void setup() {
    pinMode(2, OUTPUT); // กำหนดขา 2 ให้เป็นขาเอาต์พุต
}

// ฟังก์ชัน loop() จะทำงานซ้ำไปเรื่อย ๆ
void loop() {
    digitalWrite(2, HIGH); // เปิดไฟ LED (จ่ายไฟ HIGH ที่ขา 2)
    delay(1000);           // หน่วงเวลา 1 วินาที
    digitalWrite(2, LOW);  // ปิดไฟ LED (จ่ายไฟ LOW ที่ขา 2)
    delay(1000);           // หน่วงเวลา 1 วินาที
}