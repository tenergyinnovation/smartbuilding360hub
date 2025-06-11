#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads; // สร้างอ็อบเจกต์สำหรับใช้งาน ADC ADS1115

void setup()
{
  Serial.begin(115200); // เริ่มต้น Serial Monitor ที่ baudrate 115200
  ads.begin();          // เริ่มต้นใช้งานโมดูล ADS1115
}

void loop()
{
  int16_t raw0 = ads.readADC_SingleEnded(2); // อ่านค่าดิจิทัลจากช่อง AIN2 ของ ADS1115
  float voltage = raw0 * 0.1875 / 1000.0;    // แปลงค่าดิจิทัลเป็นแรงดันไฟฟ้า (0.1875 mV ต่อ 1 count)

  Serial.print("Potentiometer: ");           // แสดงข้อความ "Potentiometer: "
  Serial.print(voltage, 3);                  // แสดงค่าแรงดันไฟฟ้า 3 ตำแหน่งทศนิยม
  Serial.println(" V");                      // แสดงหน่วยเป็นโวลต์

  delay(500); // หน่วงเวลา 0.5 วินาที ก่อนอ่านค่ารอบถัดไป
}
