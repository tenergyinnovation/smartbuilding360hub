#include <Wire.h>             // ใช้ไลบรารี Wire สำหรับ I2C
#include <Adafruit_ADS1X15.h> // ใช้ไลบรารี ADS1115 สำหรับ ADC
#include <Arduino.h>          // ใช้ไลบรารี Arduino สำหรับฟังก์ชันพื้นฐาน

Adafruit_ADS1115 ads;       // สร้างออบเจ็กต์ ADC
const int threshold = 1000; // ค่าตัวอย่าง threshold

void setup()
{
    Serial.begin(115200);
    ads.begin(); // เริ่มต้น ADC
    Serial.println("Vibration Sensor Test");  // ข้อความเริ่มต้น  
}

void loop()
{
    int16_t raw = ads.readADC_SingleEnded(0); // อ่าน AIN0
    float voltage = raw * 0.1875 / 1000.0;    // แปลงเป็นโวลต์

    Serial.print("Raw: "); 
    Serial.print(raw); // แสดงค่าดิบ
    Serial.print("  V: ");
    Serial.println(voltage, 2); // แสดงค่าโวลต์

    // ตรวจสอบว่าค่าดิบเกิน threshold หรือไม่
    if (abs(raw) > threshold) // ใช้ abs() เพื่อเปรียบเทียบค่าบวกและลบ
    {
        Serial.println("*** Vibration detected! ***");
    }
    delay(200);
}
