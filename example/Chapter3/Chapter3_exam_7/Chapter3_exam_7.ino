/***********************************************************************
 * Project      :     smartbuilding360hub
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
// ฟังก์ชันสำหรับแสดงข้อมูล Header ของโปรเจกต์ผ่าน Serial Monitor
void header_print(void)
{
    Serial.printf("\r\n***********************************************************************\r\n");
    Serial.printf("* Project      :     smartbuilding360hub\r\n");
    Serial.printf("* Description  :     Template coding for tenergy32hub on vscode with platformIO\r\n");
    Serial.printf("* Hardware     :     tenergy32hub\r\n");
    Serial.printf("* Author       :     Tenergy Innovation Co., Ltd.\r\n");
    Serial.printf("* Date         :     04/07/2022\r\n");
    Serial.printf("* Revision     :     %s\r\n", version);
    Serial.printf("* Rev1.0       :     Origital\r\n");
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
// (ยังไม่ได้กำหนดขา GPIO เพิ่มเติม)

/**************************************/
/*       Constand define value        */
/**************************************/
// กำหนดค่า timeout สำหรับ Watchdog Timer เป็น 10 วินาที
#define WDT_TIMEOUT 10

/**************************************/
/*       eeprom address define        */
/**************************************/
// (ยังไม่ได้กำหนดตำแหน่ง eeprom)

/**************************************/
/*        define global variable      */
/**************************************/
// ตัวแปรสำหรับเก็บชื่อ unitName ที่สร้างจาก MAC Address
String unitName = "";

/**************************************/
/*           define function          */
/**************************************/

/***********************************************************************
 * FUNCTION:    getUnitNameFromMac
 * DESCRIPTION: สร้างชื่อ unitName จาก MAC Address (6 ตัวหลัง)
 * RETURNED:    String ชื่อบอร์ด tenergy32hub-xxxxxx
 ***********************************************************************/
// ฟังก์ชันนี้จะอ่าน MAC Address ของบอร์ด แล้วนำ 3 ไบต์สุดท้ายมาใช้สร้างชื่อ unitName
String getUnitNameFromMac()
{
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA); // อ่าน MAC Address ของ WiFi
    char macStr[7];
    snprintf(macStr, sizeof(macStr), "%02X%02X%02X", mac[3], mac[4], mac[5]); // แปลง 3 ไบต์สุดท้ายเป็นสตริง
    return "esp32hub-" + String(macStr); // รวมเป็นชื่อ unitName
}

/***********************************************************************
 * FUNCTION:    setup
 * DESCRIPTION: setup process
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
// ฟังก์ชัน setup() จะถูกเรียกเมื่อบอร์ดเริ่มทำงาน
void setup()
{
    Serial.begin(115200); // เริ่มต้น Serial ที่ baudrate 115200
    header_print();       // แสดงข้อมูล Header ของโปรเจกต์

    mcu.begin();          // เริ่มต้นการทำงานของอ็อบเจกต์ mcu
    mcu.displayOLEDInfo();// แสดงข้อมูลบนหน้าจอ OLED
    vTaskDelay(1000);     // หน่วงเวลา 1 วินาที

    unitName = getUnitNameFromMac(); // สร้างชื่อ unitName จาก MAC Address

    // แสดงชื่อ unitName บน Serial และ OLED
    Serial.printf("unitName: %s\r\n", unitName.c_str());
    mcu.displayOLED(unitName.c_str());

    // ตั้งค่าและเปิดใช้งาน Watchdog Timer (WDT) ที่ 10 วินาที
    esp_task_wdt_init(WDT_TIMEOUT, true); // true = รีเซ็ต CPU เมื่อ WDT timeout
    esp_task_wdt_add(NULL);               // เพิ่ม task ปัจจุบันเข้า WDT monitoring
}

/***********************************************************************
 * FUNCTION:    loop
 * DESCRIPTION: loop process
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
// ฟังก์ชัน loop() จะทำงานวนซ้ำตลอดเวลา
void loop()
{
    esp_task_wdt_reset(); // รีเซ็ต Watchdog Timer เพื่อป้องกันการรีเซ็ตบอร์ด
    vTaskDelay(1000);     // หน่วงเวลา 1 วินาที
}