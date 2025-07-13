/***********************************************************************
 * Project      :     smartbuilding360hub SDM120 Modbus Example
 * Description  :     อ่านค่า SDM120 Modbus ผ่าน RS485
 *                    แสดงผลทาง Serial และ OLED
 *                    สามารถอ่านค่าแรงดันไฟฟ้า, กระแสไฟฟ้า, กำลังไฟฟ้า และพลังงานที่ใช้
 * Hardware     :     tenergy32hub + SDM120 Modbus + CT sensor
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
    Serial.printf("* Project      :     smartbuilding360hub SDM120 Modbus Example\r\n");
    Serial.printf("* Description  :     อ่านค่า SDM120 Modbus ผ่าน RS485\r\n");
    Serial.printf("* Hardware     :     tenergy32hub + SDM120 Modbus + CT sensor\r\n");
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
 * DESCRIPTION: อ่านค่า SDM120CT ทีละรีจิสเตอร์ (ผ่าน RS485/Serial2, slave address=1, baudrate=9600)
 *              แสดงผลทาง Serial และ OLED
 * Parameter:    nothing
 * Returns:      nothing
 ***********************************************************************/
void loop()
{

    // ประกาศตัวแปรสำหรับเก็บข้อความแต่ละบรรทัดบน OLED
    char line1[40], line2[40], line3[40], line4[40];

    // ประกาศตัวแปรสำหรับเก็บค่าที่อ่านได้จาก SDM120CT
    float voltage = 0, current = 0, activePower = 0, importEnergy = 0, powerFactor = 0, frequency = 0;

    // อ่านค่าจาก SDM120CT ทีละรีจิสเตอร์ (ผ่าน RS485/Serial2, slave address=1, baudrate=9600)
    // ฟังก์ชัน readSDM120... จะคืนค่า true หากอ่านสำเร็จ
    if (mcu.readSDM120All(sdm120SlaveAddress, voltage, current, activePower, importEnergy, powerFactor, frequency, Serial2, 9600))
    {
        // สร้างข้อความแต่ละบรรทัดสำหรับแสดงบน OLED
        // line1: แรงดันและกระแส
        snprintf(line1, sizeof(line1), "V:%.1fV I:%.1fA", voltage, current);
        // line2: กำลังไฟฟ้าและพลังงานสะสม
        snprintf(line2, sizeof(line2), "P:%.1fW E:%.1fkWh", activePower, importEnergy / 1000.0);
        // line3: Power Factor และความถี่
        snprintf(line3, sizeof(line3), "PF:%.1f F:%.1fHz", powerFactor, frequency);
        // line4: พลังงานสะสม (แสดงซ้ำเพื่อเน้น)
        snprintf(line4, sizeof(line4), "importEnergy:%.1fkWh", importEnergy / 1000.0);

        mcu.displayOLEDLines(line1, line2, line3, line4); // แสดงผลบน OLED

        // แสดงผลค่าที่อ่านได้ทาง Serial Monitor
        Serial.printf("Voltage: %.1f V, Current: %.1f A, Power: %.1f W, Energy: %.1f kWh, PF: %.1f, Freq: %.1f Hz\r\n",
                      voltage, current, activePower, importEnergy / 1000.0, powerFactor, frequency);
    }
    else
    {
        // ถ้าอ่านค่า SDM120CT ไม่สำเร็จ
        Serial.println("Failed to read SDM120CT data."); // แจ้งเตือนทาง Serial
        // สร้างข้อความผิดพลาดสำหรับแสดงบน OLED
        snprintf(line1, sizeof(line1), "Error reading SDM120");
        snprintf(line2, sizeof(line2), "Check connection");
        snprintf(line3, sizeof(line3), "or settings");
        snprintf(line4, sizeof(line4), "Slave Addr: 1");
        mcu.displayOLEDLines(line1, line2, line3, line4); // แสดงข้อความผิดพลาดบน OLED
        delay(5000);                                      // แสดงข้อความผิดพลาดบน OLED ประมาณ 5 วินาที
    }

    // รีเซ็ต watchdog timer เพื่อป้องกันการรีเซ็ต MCU
    esp_task_wdt_reset();
    // หน่วงเวลา 2 วินาที (เพื่อให้ SDM120 ตอบทัน)
    delay(2000);
}