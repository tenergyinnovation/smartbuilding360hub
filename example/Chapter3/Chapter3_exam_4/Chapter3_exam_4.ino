#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128    // กำหนดความกว้างหน้าจอ OLED
#define SCREEN_HEIGHT 32    // กำหนดความสูงหน้าจอ OLED
#define OLED_RESET -1       // ไม่ใช้ขารีเซ็ต
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int pinSW1 = 34; // กำหนดขา GPIO34 สำหรับปุ่ม SW1

void setup()
{
  Serial.begin(115200);                 // เริ่มต้น Serial Monitor ที่ baudrate 115200
  pinMode(pinSW1, INPUT_PULLUP);        // กำหนดขา SW1 เป็นอินพุตและเปิดใช้งานตัวต้านทาน pull-up ภายใน
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) // เริ่มต้นหน้าจอ OLED ที่ address 0x3C
  {
    Serial.println("OLED init failed"); // ถ้าเริ่มต้นไม่สำเร็จ ให้แสดงข้อความและหยุดทำงาน
    while (true)
      ;
  }
  display.clearDisplay();               // ล้างหน้าจอ OLED
  display.setTextSize(1);               // กำหนดขนาดตัวอักษร
  display.setTextColor(SSD1306_WHITE);  // กำหนดสีตัวอักษรเป็นขาว
}

void loop()
{
  display.clearDisplay();               // ล้างหน้าจอทุกครั้งก่อนแสดงผลใหม่
  display.setCursor(0, 0);              // กำหนดตำแหน่งเริ่มต้นของข้อความ
  display.println("Smart Building 360");// แสดงข้อความหัวข้อ
  display.print("SW1: ");               // แสดงข้อความ "SW1: "
  display.println(!digitalRead(pinSW1));// แสดงสถานะปุ่ม SW1 (ถ้ากดจะเป็น 1)
  display.display();                    // สั่งให้แสดงผลบนหน้าจอ OLED
  delay(1000);                          // หน่วงเวลา 1 วินาที
}