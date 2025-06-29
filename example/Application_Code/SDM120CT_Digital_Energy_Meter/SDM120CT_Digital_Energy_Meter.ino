/***********************************************************************
 * Project      :     smartbuilding360hub Read SDM120CT Digital Energy Meter
 * Description  :     อ่านค่าพลังงานจาก SDM120CT แสดงผลทาง Serial & OLED
 * Hardware     :     tenergy32hub + SDM120CT + Buzzer + SW1 + SW2 + Relay
 * Author       :     Tenergy Innovation Co., Ltd.
 * Date         :     29/06/2025
 * Revision     :     1.0
 ***********************************************************************/
#include <Arduino.h>
#include <tenergy32hub.h>
#include <Wire.h>
#include <EEPROM.h>
#include <esp_task_wdt.h>
#include <esp_system.h> // สำหรับ esp_read_mac

/**************************************/
/*          Firmware Version          */
/**************************************/
String version = "1.0";

/**************************************/
/*        define global variable      */
/**************************************/
String unitName = "";

/**************************************/
/*        define object variable      */
/**************************************/
Tenergy32Hub mcu; // สร้างอ็อบเจกต์สำหรับควบคุมบอร์ด tenergy32hub

/**************************************/
/*       Constant define value        */
/**************************************/
#define WDT_TIMEOUT 10 // กำหนด timeout สำหรับ watchdog timer

/**************************************/
/*          Header project            */
/**************************************/
// ฟังก์ชันสำหรับแสดง header ข้อมูลโปรเจกต์ทาง Serial Monitor
void header_print(void)
{
    Serial.printf("\r\n***********************************************************************\r\n");
    Serial.printf("* Project      :     smartbuilding360hub Read SDM120CT Digital Energy Meter\r\n");
    Serial.printf("* Description  :     Read energy from SDM120CT, show on Serial & OLED\r\n");
    Serial.printf("* Hardware     :     tenergy32hub + SDM120CT + Buzzer + SW1 + SW2 + Relay\r\n");
    Serial.printf("* Author       :     Tenergy Innovation Co., Ltd.\r\n");
    Serial.printf("* Date         :     26/06/2025\r\n");
    Serial.printf("* Revision     :     %s\r\n", version.c_str());
    Serial.printf("***********************************************************************/\r\n");
}

/***********************************************************************
 * FUNCTION:    getUnitNameFromMac
 * DESCRIPTION: สร้างชื่อ unitName จาก MAC Address ของ ESP32
 ***********************************************************************/
String getUnitNameFromMac()
{
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA); // อ่าน MAC Address ของ ESP32
    char macStr[7];
    snprintf(macStr, sizeof(macStr), "%02X%02X%02X", mac[3], mac[4], mac[5]);
    return "esp32hub-" + String(macStr); // สร้างชื่อ unitName จาก MAC
}

/***********************************************************************
 * FUNCTION:    setup
 * DESCRIPTION: กำหนดค่าเริ่มต้นระบบและเซนเซอร์
 ***********************************************************************/
void setup()
{
    Serial.begin(115200); // เริ่มต้น Serial Monitor
    header_print();       // แสดง header ข้อมูลโปรเจกต์

    mcu.begin();          // เริ่มต้นใช้งานบอร์ด tenergy32hub (I2C, OLED, LoRa, ฯลฯ)
    mcu.displayOLEDInfo();// แสดงข้อมูลเบื้องต้นบน OLED
    vTaskDelay(1000);

    mcu.showLibraryVersion(); // แสดงเวอร์ชันไลบรารีทาง Serial
    mcu.displayOLED("SMART BUILDING 360 HUB"); // แสดงชื่อโปรเจกต์บน OLED

    unitName = getUnitNameFromMac(); // สร้างชื่อ unitName จาก MAC Address
    Serial.printf("unitName: %s\r\n", unitName.c_str());

    esp_task_wdt_init(WDT_TIMEOUT, true); // เริ่มต้น watchdog timer
    esp_task_wdt_add(NULL);

    Serial.println("System Ready"); // แจ้งเตรียมพร้อมระบบ
}

/***********************************************************************
 * FUNCTION:    loop
 * DESCRIPTION: อ่านค่าพลังงานจาก SDM120CT แสดงผลทาง Serial และ OLED
 ***********************************************************************/
void loop()
{
    char line1[40], line2[40], line3[40], line4[40]; // ตัวแปรสำหรับเก็บข้อความแต่ละบรรทัดบน OLED

    // ประกาศตัวแปรสำหรับเก็บค่าที่อ่านได้จาก SDM120CT
    float voltage = 0, current = 0, activePower = 0, importEnergy = 0, powerFactor = 0, frequency = 0;

    // อ่านค่าจาก SDM120CT ทีละรีจิสเตอร์ (ผ่าน RS485/Serial2, slave address=1, baudrate=9600)
    // ถ้าอ่านค่าทุกรีจิสเตอร์สำเร็จ
    if (mcu.readSDM120Voltage(1, voltage, Serial2, 9600) &&
        mcu.readSDM120Current(1, current, Serial2, 9600) &&
        mcu.readSDM120ActivePower(1, activePower, Serial2, 9600) &&
        mcu.readSDM120ImportEnergy(1, importEnergy, Serial2, 9600) &&
        mcu.readSDM120PowerFactor(1, powerFactor, Serial2, 9600) &&
        mcu.readSDM120Frequency(1, frequency, Serial2, 9600))
    {
        // สร้างข้อความแต่ละบรรทัดสำหรับแสดงบน OLED
        snprintf(line1, sizeof(line1), "V:%.1fV I:%.1fA", voltage, current);
        snprintf(line2, sizeof(line2), "P:%.1fW E:%.1fkWh", activePower, importEnergy / 1000.0);
        snprintf(line3, sizeof(line3), "PF:%.1f F:%.1fHz", powerFactor, frequency);
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
        // แสดงข้อความผิดพลาดบน OLED
        snprintf(line1, sizeof(line1), "Error reading SDM120");
        snprintf(line2, sizeof(line2), "Check connection");
        snprintf(line3, sizeof(line3), "or settings");
        snprintf(line4, sizeof(line4), "Slave Addr: 1");
        mcu.displayOLEDLines(line1, line2, line3, line4);
        delay(5000); // แสดงข้อความผิดพลาดบน OLED ประมาณ 5 วินาที
    }

    esp_task_wdt_reset(); // รีเซ็ต watchdog timer
    delay(2000); // หน่วงเวลา 2 วินาที (เพื่อให้ SDM120 ตอบทัน)
}