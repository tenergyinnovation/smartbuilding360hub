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

    // อ่านสถานะปุ่ม SW1 และ SW2
    bool sw1 = mcu.readSW1();                 // อ่านสถานะปุ่ม SW1
    bool sw2 = mcu.readSW2();                 // อ่านสถานะปุ่ม SW2
    bool slideSwitch = mcu.readSlideSwitch(); // อ่านสถานะสไลด์สวิตช์

    // อ่านสถานะรีเลย์
    bool relayState = mcu.readRelayState();   // อ่านสถานะรีเลย์ปัจจุบัน
    static bool slideSwitch_prevState = true; // ตัวแปรเก็บสถานะก่อนหน้าเพื่อเปรียบเทียบการเปลี่ยนแปลง

    // ถ้ากด SW1
    if (sw1)
    {
        // แสดงสถานะบน OLED
        mcu.displayOLEDLines("SW1: ON", "Relay: ON"); // แสดงข้อความบน OLED ว่า SW1 ถูกกด
        mcu.angryBirdSound();                         // เล่นเสียง angry bird
        mcu.relayOn();                                // เปิดรีเลย์
        relayState = true;                            // อัปเดตสถานะรีเลย์
        vTaskDelay(500);                              // ป้องกันการ repeat
    }

    // ถ้ากด SW2
    if (sw2)
    {
        // แสดงสถานะบน OLED
        mcu.displayOLEDLines("SW1: OFF", "Relay: OFF"); // แสดงข้อความบน OLED ว่า SW2 ถูกกด
        mcu.marioSound();                               // เล่นเสียง mario
        mcu.relayOff();                                 // ปิดรีเลย์
        relayState = false;                             // อัปเดตสถานะรีเลย์
        vTaskDelay(500);                                // ป้องกันการ repeat
    }

    // ตรวจสอบสถานะสไลด์สวิตช์
    if (slideSwitch != slideSwitch_prevState) // ตรวจสอบว่ามีการเปลี่ยนแปลงสถานะของสไลด์สวิตช์หรือไม่
    {
        if (slideSwitch)
        {
            mcu.blinkbuildingLED(500); // เริ่มกระพริบ LED สีเขียวทุก 0.5 วินาที
            mcu.blinkRedLED(1000);     // เริ่มกระพริบ LED สีแดงทุก 1 วินาที
            mcu.blinkBlueLED(1500);    // เริ่มกระพริบ LED สีน้ำเงินทุก 1.5 วินาที
        }
        else
        {
            mcu.blinkbuildingLED(0); // หยุดกระพริบ LED สีเขียว
            mcu.blinkRedLED(0);      // หยุดกระพริบ LED สีแดง
            mcu.blinkBlueLED(0);     // หยุดกระพริบ LED สีน้ำเงิน
        }
        slideSwitch_prevState = slideSwitch; // อัปเดตสถานะก่อนหน้า
    }

    // อ่านค่า Potentiometer (VR) จาก ADC ช่อง 2
    int16_t raw0 = mcu.readPotentiometer(); // อ่านค่าดิจิทัลจากช่อง AIN2 ของ ADS1115
    float voltage = raw0 * 0.1875 / 1000.0; // แปลงค่าดิจิทัลเป็นแรงดันไฟฟ้า (0.1875 mV ต่อ 1 count)

    Serial.print("Potentiometer: "); // แสดงข้อความ "Potentiometer: "
    Serial.print(voltage, 2);        // แสดงค่าแรงดันไฟฟ้า 3 ตำแหน่งทศนิยม
    Serial.println(" V");            // แสดงหน่วยเป็นโวลต์

    char line1[32], line2[32], line3[32];                                             // สร้างตัวแปรสำหรับเก็บข้อความที่จะแสดงบน OLED
    snprintf(line1, sizeof(line1), "VR: %.2f V", voltage);                            // สร้างสตริงแสดงค่า Potentiometer
    snprintf(line2, sizeof(line2), "Relay: %s", mcu.readRelayState() ? "ON" : "OFF"); // สร้างสตริงแสดงสถานะรีเลย
    sniprintf(line3, sizeof(line3), "BlinkLED: %s", slideSwitch ? "ON" : "OFF");      // สร้างสตริงแสดงสถานะปุ่ม SW1
    mcu.displayOLEDLines(line1, line2, line3);                                        // แสดงข้อมูลบน OLED

    vTaskDelay(200); // หน่วงเวลาเล็กน้อยเพื่อให้การแสดงผลไม่ถี่เกินไป
}
