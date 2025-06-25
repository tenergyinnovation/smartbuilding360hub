#include <Arduino.h>

// กำหนดหมายเลขขา GPIO สำหรับปุ่มกดแต่ละตัว
const int pinSW1 = 34;
const int pinSW2 = 35;
const int pinSW3 = 36;

void setup()
{
  Serial.begin(115200);           // เริ่มต้น Serial Monitor ที่ baudrate 115200
  pinMode(pinSW1, INPUT_PULLUP);  // กำหนดขา SW1 เป็นอินพุตและเปิดใช้งานตัวต้านทาน pull-up ภายใน
  pinMode(pinSW2, INPUT_PULLUP);  // กำหนดขา SW2 เป็นอินพุตและเปิดใช้งานตัวต้านทาน pull-up ภายใน
  pinMode(pinSW3, INPUT_PULLUP);  // กำหนดขา SW3 เป็นอินพุตและเปิดใช้งานตัวต้านทาน pull-up ภายใน
}

void loop()
{
  // อ่านสถานะปุ่ม ถ้ากดจะได้ค่า LOW (จึงต้อง ! เพื่อให้ true เมื่อกด)
  bool sw1 = !digitalRead(pinSW1); // ถ้ากด SW1 จะเป็น true
  bool sw2 = !digitalRead(pinSW2); // ถ้ากด SW2 จะเป็น true
  bool sw3 = !digitalRead(pinSW3); // ถ้ากด SW3 จะเป็น true

  // แสดงผลสถานะปุ่มแต่ละตัวทาง Serial Monitor
  Serial.print("SW1="); Serial.print(sw1);
  Serial.print(" SW2="); Serial.print(sw2);
  Serial.print(" SW3="); Serial.println(sw3);

  delay(500); // หน่วงเวลา 0.5 วินาที ก่อนอ่านค่ารอบถัดไป
}