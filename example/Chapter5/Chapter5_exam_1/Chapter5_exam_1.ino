/***********************************************************************
 * Project      :     smartbuilding360hub Sound Sensor
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
    Serial.printf("* Project      :     smartbuilding360hub Sound Sensor\r\n");
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


/**************************************/
/*           define function          */
/**************************************/


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

    mcu.begin();           // เริ่มต้นการทำงานของอ็อบเจกต์ mcu
    mcu.displayOLEDInfo(); // แสดงข้อมูลบนหน้าจอ OLED
    vTaskDelay(1000);      // หน่วงเวลา 1 วินาที


    if (mcu.initADC()) //    ตรวจสอบการเริ่มต้น ADC
    {
        Serial.println("ADC initialized successfully.");
    }
    else
    {
        Serial.println("Failed to initialize ADC.");
        mcu.beep(3, 100); // บี๊บ 3 ครั้งถ้าไม่สำเร็จ
    }

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
    // อ่านค่า Microphone Sensor จาก AIN1 จำนวน 10 ครั้ง แล้วหาค่าเฉลี่ย
    long sum_raw_mic = 0; // ประกาศตัวแปรสำหรับรวมค่าที่อ่านได้
    for (int i = 0; i < 10; i++)
    {                                         // วนลูปอ่านค่า 10 ครั้ง
        sum_raw_mic += mcu.readADCChannel(1); // อ่านค่าดิบจาก AIN1 แล้วบวกสะสม
        delay(2);                             // หน่วงเวลาสั้น ๆ เพื่อความแม่นยำในการอ่าน
    }
    int16_t raw_mic = sum_raw_mic / 10;            // หาค่าเฉลี่ยของค่าดิบที่อ่านได้ 10 ครั้ง
    float voltage_mic = raw_mic * 0.1875 / 1000.0; // แปลงค่าดิบเป็นแรงดันไฟฟ้า (โวลต์)

    // แสดงค่าดิบและแรงดันไฟฟ้าทาง Serial Monitor
    Serial.printf("Mic Raw(avg): %d  V: %.2fV\n", raw_mic, voltage_mic);

    // แสดงค่าดิบและแรงดันไฟฟ้าบน OLED
    char line1[22], line2[22];                                // ประกาศตัวแปรสำหรับเก็บข้อความที่จะแสดงบน OLED
    snprintf(line1, sizeof(line1), "Sound Raw:%d ", raw_mic); // เตรียมข้อความแสดงบน OLED
    snprintf(line2, sizeof(line2), "V: %.2f V", voltage_mic); // แปลงแรงดันไฟฟ้าเป็นสตริง
    mcu.displayOLEDLines(line1, line2);                       // แสดงข้อความบน OLED

    delay(100); // หน่วงเวลาเล็กน้อยก่อนวนลูปรอบถัดไป

    esp_task_wdt_reset(); // รีเซ็ต Watchdog Timer เพื่อป้องกันบอร์ดรีเซ็ตตัวเอง
}