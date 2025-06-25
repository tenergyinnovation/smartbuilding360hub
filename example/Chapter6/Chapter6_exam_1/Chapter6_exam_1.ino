/***********************************************************************
 * Project      :     smartbuilding360hub Motion Sensor
 * Description  :     Template coding for tenergy32hub on vscode with platformIO
 * Hardware     :     tenergy32hub
 * Author       :     Tenergy Innovation Co., Ltd.
 * Date         :     27/04/2025
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
    Serial.printf("* Project      :     smartbuilding360hub Motion Sensor\r\n");
    Serial.printf("* Description  :     Template coding for tenergy32hub on vscode with platformIO\r\n");
    Serial.printf("* Hardware     :     tenergy32hub\r\n");
    Serial.printf("* Author       :     Tenergy Innovation Co., Ltd.\r\n");
    Serial.printf("* Date         :     04/07/2022\r\n");
    Serial.printf("* Revision     :     %s\r\n", version);
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
#define PIR_PIN 32 // กำหนดขา PIR sensor

/**************************************/
/*       Constand define value        */
/**************************************/
#define WDT_TIMEOUT 10

/**************************************/
/*        define global variable      */
/**************************************/
String unitName = "";

/**************************************/
/*           define function          */
/**************************************/
String getUnitNameFromMac()
{
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    char macStr[7];
    snprintf(macStr, sizeof(macStr), "%02X%02X%02X", mac[3], mac[4], mac[5]);
    return "esp32hub-" + String(macStr);
}

/***********************************************************************
 * FUNCTION:    setup
 * DESCRIPTION: setup process
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void setup()
{
    Serial.begin(115200);           // เริ่มต้น Serial Monitor ที่ baudrate 115200
    header_print();                 // แสดงข้อมูลโปรเจกต์

    mcu.begin();                    // เริ่มต้นใช้งานบอร์ด tenergy32hub
    mcu.displayOLEDInfo();          // แสดงข้อมูลบน OLED
    vTaskDelay(1000);               // หน่วงเวลา 1 วินาที

    unitName = getUnitNameFromMac();// สร้างชื่อ unitName จาก MAC Address
    Serial.printf("unitName: %s\r\n", unitName.c_str());
    mcu.displayOLED(unitName.c_str());

    pinMode(PIR_PIN, INPUT);        // กำหนดขา PIR เป็นอินพุต

    esp_task_wdt_init(WDT_TIMEOUT, true); // ตั้งค่า Watchdog Timer
    esp_task_wdt_add(NULL);               // เพิ่ม task ปัจจุบันเข้า WDT monitoring
}

/***********************************************************************
 * FUNCTION:    loop
 * DESCRIPTION: loop process
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void loop()
{
    int pirState = digitalRead(PIR_PIN); // อ่านสถานะ PIR sensor

    char line1[22], line2[22];

    if (pirState == HIGH) // มีการตรวจจับการเคลื่อนไหว
    {
        Serial.println("Motion Detected!"); // แสดงข้อความทาง Serial
        snprintf(line1, sizeof(line1), "PIR: Motion!"); // เตรียมข้อความแสดงบน OLED
        snprintf(line2, sizeof(line2), "Status: ACTIVE");
        mcu.displayOLEDLines(line1, line2); // แสดงข้อความบน OLED
    }
    else // ไม่มีการเคลื่อนไหว
    {
        Serial.println("No Motion"); // แสดงข้อความทาง Serial
        snprintf(line1, sizeof(line1), "PIR: No Motion"); // เตรียมข้อความแสดงบน OLED
        snprintf(line2, sizeof(line2), "Status: IDLE");
        mcu.displayOLEDLines(line1, line2); // แสดงข้อความบน OLED
    }

    esp_task_wdt_reset(); // รีเซ็ต Watchdog Timer
    delay(200); // หน่วงเวลาเล็กน้อยเพื่อความเสถียร
}