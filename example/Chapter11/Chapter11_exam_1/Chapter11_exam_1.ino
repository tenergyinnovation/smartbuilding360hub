/***********************************************************************
 * Project      :     smartbuilding360hub read Dry-contact Example
 * Description  :     อ่านค่า Dry-contact ผ่าน GPIO
 *                    แสดงผลทาง Serial และ OLED
 * Hardware     :     tenergy32hub + Dry-contact sensor
 * Author       :     Tenergy Innovation Co., Ltd.
 * Date         :     13/07/2025
 * Revision     :     1.0
 ***********************************************************************/

#include <Arduino.h>      // ไลบรารีหลักสำหรับ Arduino
#include <tenergy32hub.h> // ไลบรารีสำหรับควบคุมบอร์ด Tenergy32Hub และ SDM120
#include <esp_task_wdt.h> // ไลบรารีสำหรับ watchdog timer
#include <esp_system.h>   // ไลบรารีสำหรับฟังก์ชันระบบ ESP32
#include <tiny32_v3.h>    // ไลบรารีสำหรับบอร์ด tiny32_v3 (ถ้ามี)

/**************************************/
/*          Firmware Version          */
/**************************************/

// กำหนดเวอร์ชันของเฟิร์มแวร์
String version = "1.0";

/**************************************/
/*        define global variable      */
/**************************************/
int sdm120SlaveAddress = 1; // กำหนด slave address ของ SDM120

/**************************************/
/*        define object variable      */
/**************************************/

// สร้างอ็อบเจกต์สำหรับควบคุมบอร์ด tenergy32hub
Tenergy32Hub mcu;

/**************************************/
/*       Constant define value        */
/**************************************/

#define WDT_TIMEOUT 10 // กำหนด timeout สำหรับ watchdog timer (วินาที)

/**************************************/
/*          Header project            */
/**************************************/
// ฟังก์ชันสำหรับแสดง header ข้อมูลโปรเจกต์ทาง Serial Monitor
void header_print(void)
{

    // แสดงข้อมูลโปรเจกต์ทาง Serial Monitor
    Serial.printf("\r\n***********************************************************************\r\n");
    Serial.printf("* Project      :     smartbuilding360hub read Dry-contact Example\r\n");
    Serial.printf("* Description  :     อ่านค่า Dry-contact ผ่าน GPIO\r\n");
    Serial.printf("* Hardware     :     tenergy32hub + Dry-contact sensor\r\n");
    Serial.printf("* Author       :     Tenergy Innovation Co., Ltd.\r\n");
    Serial.printf("* Date         :     13/07/2025\r\n");
    Serial.printf("* Revision     :     %s\r\n", version.c_str());
    Serial.printf("***********************************************************************/\r\n");
}

/***********************************************************************
 * FUNCTION:    setup
 * DESCRIPTION: กำหนดค่าเริ่มต้นระบบและเซนเซอร์
 * Parameter:    nothing
 * Returns:      nothing
 ***********************************************************************/
void setup()
{

    Serial.begin(115200); // เริ่มต้น Serial Monitor ที่ baudrate 115200
    header_print();       // แสดง header ข้อมูลโปรเจกต์

    mcu.begin();           // เริ่มต้นใช้งานบอร์ด tenergy32hub และอุปกรณ์ที่เกี่ยวข้อง
    mcu.displayOLEDInfo(); // แสดงข้อมูลเบื้องต้นบน OLED
    vTaskDelay(1000);      // หน่วงเวลา 1 วินาที
}

/***********************************************************************
 * FUNCTION:    loop
 * DESCRIPTION: Dry-contact sensor reading loop
 *              อ่านค่า Dry-contact sensor
 *              แสดงผลทาง Serial และ OLED
 * Parameter:    nothing
 * Returns:      nothing
 ***********************************************************************/
void loop()
{

    // ประกาศตัวแปรสำหรับเก็บข้อความแต่ละบรรทัดบน OLED
    char line1[40], line2[40], line3[40], line4[40];

    // --- ตรวจสอบสถานะ Dry-contact (เช่น เซนเซอร์/สวิตช์) ---
    // ใช้ขา GPIO32 (PIN_MOTION_SENSOR) สำหรับ Dry-contact
    // อ่านสถานะ Dry-contact (1 = ON/เปิดวงจร, 0 = OFF/ปิดวงจร)
    int dryContactState = !mcu.readMotionSensor(); // 1 = ON/เปิดวงจร, 0 = OFF/ปิดวงจร

    // แสดงผล Dry-contact ทาง Serial
    if (!dryContactState)
    {
        Serial.println("Dry-contact: ON");
        mcu.displayOLED("Dry-contact: ON"); // แสดงข้อความบน OLED
    }
    else
    {
        Serial.println("Dry-contact: OFF");
        mcu.displayOLED("Dry-contact: OFF"); // แสดงข้อความบน OLED
    }

    // แสดงผล Dry-contact บน OLED (บรรทัดที่ 1)
    snprintf(line1, sizeof(line1), "Dry-contact: %s", dryContactState ? "ON" : "OFF");

    // รีเซ็ต watchdog timer เพื่อป้องกันการรีเซ็ต MCU
    esp_task_wdt_reset();
    // หน่วงเวลา 2 วินาที (เพื่อให้ SDM120 ตอบทัน)
    delay(2000);
}