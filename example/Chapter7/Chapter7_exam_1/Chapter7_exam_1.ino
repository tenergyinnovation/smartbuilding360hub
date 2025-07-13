/***********************************************************************
 * Project      :     smartbuilding360hub Water Leak Sensor
 * Description  :     Template coding for tenergy32hub on vscode with platformIO
 * Hardware     :     tenergy32hub
 * Author       :     Tenergy Innovation Co., Ltd.
 * Date         :     26/06/2025
 * Revision     :     1.0
 * Rev1.0       :     Original
 * website      :     http://www.tenergyinnovation.co.th
 * Email        :     uten.boonliam@tenergyinnovation.co.th
 * TEL          :     +66 89-140-7205
 ***********************************************************************/
#include <Arduino.h>
#include <tenergy32hub.h>
#include <esp_task_wdt.h>
#include <esp_system.h> // สำหรับ esp_read_mac

/**************************************/
/*          Firmware Version          */
/**************************************/
String version = "1.0"; // กำหนดเวอร์ชันของเฟิร์มแวร์

/**************************************/
/*          Header project            */
/**************************************/
void header_print(void)
{
    Serial.printf("\r\n***********************************************************************\r\n");
    Serial.printf("* Project      :     smartbuilding360hub Water Leak Sensor\r\n");
    Serial.printf("* Description  :     Template coding for tenergy32hub on vscode with platformIO\r\n");
    Serial.printf("* Hardware     :     tenergy32hub\r\n");
    Serial.printf("* Author       :     Tenergy Innovation Co., Ltd.\r\n");
    Serial.printf("* Date         :     26/06/2025\r\n");
    Serial.printf("* Revision     :     %s\r\n", version.c_str());
    Serial.printf("* Rev1.0       :     Original\r\n");
    Serial.printf("* website      :     http://www.tenergyinnovation.co.th\r\n");
    Serial.printf("* Email        :     uten.boonliam@tenergyinnovation.co.th\r\n");
    Serial.printf("* TEL          :     +66 89-140-7205\r\n");
    Serial.printf("***********************************************************************/\r\n");
}

/**************************************/
/*        define object variable      */
/**************************************/
Tenergy32Hub mcu; // สร้างอ็อบเจกต์ mcu สำหรับควบคุมบอร์ด tenergy32hub

/**************************************/
/*            GPIO define             */
/**************************************/
#define WATER_LEAK_PIN 33 // กำหนดขา Water Leak sensor

/**************************************/
/*       Constand define value        */
/**************************************/
#define WDT_TIMEOUT 10
#define RELAY_ON_TIME 30000 // 30 วินาที

/**************************************/
/*        define global variable      */
/**************************************/
unsigned long lastLeakTime = 0; // เวลาที่ตรวจจับน้ำรั่วล่าสุด
bool relayState = false;        // สถานะรีเลย์

/**************************************/
/*           define function          */
/**************************************/

/***********************************************************************
 * FUNCTION:    setup
 * DESCRIPTION: setup process
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void setup()
{
    Serial.begin(115200); // เริ่มต้น Serial Monitor ที่ baudrate 115200
    header_print();       // แสดงข้อมูลโปรเจกต์

    mcu.begin();           // เริ่มต้นใช้งานบอร์ด tenergy32hub
    mcu.displayOLEDInfo(); // แสดงข้อมูลบน OLED
    vTaskDelay(1000);      // หน่วงเวลา 1 วินาที

    pinMode(WATER_LEAK_PIN, INPUT); // กำหนดขา Water Leak Sensor เป็นอินพุต

    mcu.relayOff(); // ปิดรีเลย์เริ่มต้น
    relayState = false;

    esp_task_wdt_init(WDT_TIMEOUT, true); // ตั้งค่า Watchdog Timer
    esp_task_wdt_add(NULL);               // เพิ่ม task ปัจจุบันเข้า WDT monitoring

    Serial.println("System Ready");
}

/***********************************************************************
 * FUNCTION:    loop
 * DESCRIPTION: loop process
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void loop()
{
    int waterLeakState = digitalRead(WATER_LEAK_PIN); // อ่านสถานะ Water Leak sensor

    // แสดงสถานะเซนเซอร์น้ำรั่วทาง Serial Monitor เท่านั้น
    if (waterLeakState == HIGH) // ตรวจจับน้ำรั่ว
    {
        Serial.println("Water Leak Detected!"); // แสดงข้อความทาง Serial
    }
    else // ไม่พบการรั่วของน้ำ
    {
        Serial.println("Water: No Leak"); // แสดงข้อความทาง Serial
    }

    esp_task_wdt_reset(); // รีเซ็ต Watchdog Timer
    delay(200);           // หน่วงเวลาเล็กน้อยเพื่อความเสถียร
}