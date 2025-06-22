/***********************************************************************
 * Project      :     smartbuilding360hub Temp & Humi & Relay Control
 * Description  :     Template coding for tenergy32hub on vscode with platformIO
 * Hardware     :     tenergy32hub
 * Author       :     Tenergy Innovation Co., Ltd.
 * Date         :     22/06/2025
 * Revision     :     1.2
 * Rev1.0       :     Original
 * Rev1.1       :     Update for new features [2025-06-22]
 * Rev1.2       :     Add EEPROM save/load for temp threshold [2025-06-23]   
 * website      :     http://www.tenergyinnovation.co.th
 * Email        :     uten.boonliam@tenergyinnovation.co.th
 * TEL          :     +66 89-140-7205
 ***********************************************************************/
#include <Arduino.h>                  // ไลบรารีหลักของ Arduino
#include <tenergy32hub.h>             // ไลบรารีสำหรับบอร์ด tenergy32hub
#include <esp_task_wdt.h>             // ไลบรารีสำหรับ Watchdog Timer
#include <esp_system.h>               // ไลบรารีสำหรับระบบ ESP32
#include <WiFi.h>                     // ไลบรารีสำหรับเชื่อมต่อ WiFi
#include <WebServer.h>                // ไลบรารีสำหรับ Web Server
#include <Adafruit_SHT31.h>           // ไลบรารีสำหรับ SHT30
#include <EEPROM.h>                   // ไลบรารีสำหรับ EEPROM

/**************************************/
/*          Firmware Version          */
/**************************************/
String version = "1.2"; // กำหนดเวอร์ชันของเฟิร์มแวร์

/**************************************/
/*          Header project            */
/**************************************/
void header_print(void)
{
    Serial.printf("\r\n***********************************************************************\r\n"); // แสดงเส้นแบ่ง
    Serial.printf("* Project      :     smartbuilding360hub Motion Sensor\r\n"); // แสดงชื่อโปรเจกต์
    Serial.printf("* Description  :     Template coding for tenergy32hub on vscode with platformIO\r\n"); // แสดงรายละเอียด
    Serial.printf("* Hardware     :     tenergy32hub\r\n"); // แสดงชื่อฮาร์ดแวร์
    Serial.printf("* Author       :     Tenergy Innovation Co., Ltd.\r\n"); // แสดงชื่อผู้เขียน
    Serial.printf("* Date         :     04/07/2022\r\n"); // แสดงวันที่
    Serial.printf("* Revision     :     %s\r\n", version); // แสดงเวอร์ชัน
    Serial.printf("* Rev1.0       :     Original\r\n"); // แสดงหมายเหตุเวอร์ชัน
    Serial.printf("* website      :     http://www.tenergyinnovation.co.th\r\n"); // แสดงเว็บไซต์
    Serial.printf("* Email        :     uten.boonliam@tenergyinnovation.co.th\r\n"); // แสดงอีเมล
    Serial.printf("* TEL          :     +66 89-140-7205\r\n"); // แสดงเบอร์โทร
    Serial.printf("***********************************************************************/\r\n"); // แสดงเส้นแบ่ง
}

/**************************************/
/*        define object variable      */
/**************************************/
Tenergy32Hub mcu;                        // สร้างอ็อบเจกต์ mcu สำหรับควบคุมบอร์ด tenergy32hub
Adafruit_SHT31 sht30 = Adafruit_SHT31(); // อ็อบเจกต์ SHT30

/**************************************/
/*            GPIO define             */
/**************************************/
#define PIR_PIN 32 // กำหนดขา PIR sensor

/**************************************/
/*       Constand define value        */
/**************************************/
#define WDT_TIMEOUT 10 // กำหนดค่า timeout สำหรับ Watchdog Timer

/**************************************/
/*        define global variable      */
/**************************************/
String unitName = ""; // ตัวแปรเก็บชื่อ unit

// เพิ่มตัวแปรสถานะ relay
bool relayState = false; // ตัวแปรเก็บสถานะ relay

#define EEPROM_ADDR 0 // ตำแหน่งเก็บค่า threshold ใน EEPROM

float tempThreshold = 30.0; // ค่าเริ่มต้นของ temp threshold

/***********************************************************************
 * FUNCTION:    loadThresholdFromEEPROM
 * DESCRIPTION: Load temperature threshold from EEPROM
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void loadThresholdFromEEPROM()
{
    EEPROM.begin(32); // เริ่มต้นใช้งาน EEPROM
    float val; // ตัวแปรชั่วคราวสำหรับอ่านค่า
    EEPROM.get(EEPROM_ADDR, val); // อ่านค่าจาก EEPROM
    if (!isnan(val) && val > 0 && val < 100) // ตรวจสอบค่าที่อ่านได้
        tempThreshold = val; // ถ้าค่าถูกต้องให้นำมาใช้
    EEPROM.end(); // จบการใช้งาน EEPROM
}

void saveThresholdToEEPROM()
{
    EEPROM.begin(32); // เริ่มต้นใช้งาน EEPROM
    EEPROM.put(EEPROM_ADDR, tempThreshold); // เขียนค่า tempThreshold ลง EEPROM
    EEPROM.commit(); // ยืนยันการเขียน
    EEPROM.end(); // จบการใช้งาน EEPROM
}

/***********************************************************************
 * FUNCTION:    getUnitNameFromMac
 * DESCRIPTION: Generate a unit name based on the MAC address of the ESP32
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
String getUnitNameFromMac()
{
    uint8_t mac[6]; // ตัวแปรเก็บ MAC address
    esp_read_mac(mac, ESP_MAC_WIFI_STA); // อ่าน MAC address
    char macStr[7]; // ตัวแปรเก็บ MAC address แบบ string
    snprintf(macStr, sizeof(macStr), "%02X%02X%02X", mac[3], mac[4], mac[5]); // แปลงเป็น string
    return "esp32hub-" + String(macStr); // คืนค่าชื่อ unit
}

// เพิ่มส่วนนี้ก่อน setup()
const char *ssid = "TENERGYINNOVATION"; // SSID สำหรับเชื่อมต่อ WiFi
const char *password = "L0vemel0vemydog"; // Password สำหรับเชื่อมต่อ WiFi

#include <WebServer.h> // ไลบรารี WebServer
WebServer server(80); // สร้าง WebServer ที่ port 80

float temp = 0, humid = 0; // ตัวแปรเก็บค่าอุณหภูมิและความชื้น

// ฟังก์ชันสำหรับตอบสนองหน้าเว็บ
void handleRoot()
{
    String html = "<!DOCTYPE html><html><head>"; // เริ่มต้น HTML
    html += "<meta charset='utf-8'>"; // กำหนด charset
    html += "<meta http-equiv='refresh' content='5'>"; // อัปเดตหน้าเว็บทุก 5 วินาที
    html += "<title>Sensor Data</title></head><body>"; // กำหนด title
    html += "<h2>Smart Building 360 Sensor</h2>"; // หัวข้อ

    // Temperature: สีแดง
    html += "<p><span style='color:red;'>Temperature:</span> <b style='color:red;'>" + String(temp, 2) + " &deg;C</b></p>";

    // Humidity: สีน้ำเงิน
    html += "<p><span style='color:blue;'>Humidity:</span> <b style='color:blue;'>" + String(humid, 2) + " %</b></p>";

    // Threshold: สีดำ
    html += "<p><span style='color:black;'>Threshold:</span> <b style='color:black;'>" + String(tempThreshold, 1) + " &deg;C</b></p>";

    // Relay Status: สีดำ, สถานะ ON เป็นสีเขียว, OFF เป็นเทา
    html += "<p><span style='color:black;'>Relay Status:</span> <b style='color:" 
        + String(relayState ? "green" : "gray") + ";'>" 
        + (relayState ? "ON" : "OFF") + "</b></p>";

    html += "<form action='/relay' method='POST'>"; // ฟอร์มสำหรับควบคุม relay
    if (relayState)
    {
        html += "<button type='submit' name='relay' value='off'>Turn OFF Relay</button>";
    }
    else
    {
        html += "<button type='submit' name='relay' value='on'>Turn ON Relay</button>";
    }
    html += "</form>"; // จบฟอร์ม
    html += "</body></html>"; // จบ HTML
    server.send(200, "text/html", html); // ส่ง HTML ไปยัง browser
}

/***********************************************************************
 * FUNCTION:    handleRelay
 * DESCRIPTION: Handle relay control via web interface
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
// ฟังก์ชันจัดการการกดปุ่ม relay
void handleRelay()
{
    if (server.method() == HTTP_POST) // ถ้าเป็น POST
    {
        if (server.hasArg("relay")) // ถ้ามี argument relay
        {
            String cmd = server.arg("relay"); // อ่านค่า argument
            if (cmd == "on") // ถ้าคือ on
            {
                mcu.relayOn(); // เปิด relay
                relayState = true; // อัปเดตสถานะ
            }
            else if (cmd == "off") // ถ้าคือ off
            {
                mcu.relayOff(); // ปิด relay
                relayState = false; // อัปเดตสถานะ
            }
        }
    }
    server.sendHeader("Location", "/", true); // redirect กลับไปหน้าแรก
    server.send(302, "text/plain", ""); // ส่ง response
}

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

    unitName = getUnitNameFromMac(); // สร้างชื่อ unitName จาก MAC Address
    Serial.printf("unitName: %s\r\n", unitName.c_str()); // แสดงชื่อ unitName
    mcu.displayOLED(unitName.c_str()); // แสดงชื่อ unitName บน OLED

    if (!sht30.begin(0x44)) // เริ่มต้น SHT30 ที่ address 0x44
    {
        Serial.println("Couldn't find SHT30 sensor!"); // ถ้าไม่เจอ SHT30
        mcu.displayOLED("SHT30 not found!"); // แสดงข้อความบน OLED
        while (1)
            delay(10); // ค้างไว้
    }

    // เชื่อมต่อ WiFi
    WiFi.begin(ssid, password); // เริ่มเชื่อมต่อ WiFi
    Serial.print("Connecting to WiFi"); // แสดงข้อความ
    mcu.displayOLED("Connecting WiFi..."); // แสดงข้อความบน OLED
    while (WiFi.status() != WL_CONNECTED) // รอจนกว่าจะเชื่อมต่อสำเร็จ
    {
        delay(500); // หน่วงเวลา
        Serial.print("."); // แสดงจุด
    }
    Serial.println("\nWiFi connected!"); // แสดงข้อความเมื่อเชื่อมต่อสำเร็จ
    Serial.print("IP address: "); // แสดงข้อความ
    Serial.println(WiFi.localIP()); // แสดง IP address
    mcu.displayOLED(WiFi.localIP().toString().c_str()); // แสดง IP บน OLED

    // ตั้งค่า Web Server
    server.on("/", handleRoot); // ตั้ง handler สำหรับหน้าแรก
    server.on("/relay", HTTP_POST, handleRelay); // ตั้ง handler สำหรับ relay
    server.begin(); // เริ่มต้น Web Server

    loadThresholdFromEEPROM(); // โหลดค่า threshold จาก EEPROM

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
    temp = sht30.readTemperature(); // อ่านค่าอุณหภูมิ (องศาเซลเซียส)
    humid = sht30.readHumidity();   // อ่านค่าความชื้นสัมพัทธ์ (%)

    // ใช้ library object สำหรับอ่าน switch
    bool sw1 = mcu.readSW1(); // อ่านสถานะ SW1
    bool sw2 = mcu.readSW2(); // อ่านสถานะ SW2

    static unsigned long lastBtn = 0; // ตัวแปรเก็บเวลาปุ่มล่าสุด
    static bool saved = false; // ตัวแปรกัน save ซ้ำ

    // ปรับค่า threshold
    if (sw1 && !sw2) // ถ้ากด SW1 เพียงปุ่มเดียว
    {
        if (millis() - lastBtn > 200) // หน่วงป้องกันการ repeat
        {
            tempThreshold += 0.5; // เพิ่มค่า threshold
            lastBtn = millis(); // อัปเดตเวลา
            saved = false; // ยังไม่ save
            mcu.beep(1, 80); // เสียง beep เมื่อเพิ่มค่า
            char msg[22];
            snprintf(msg, sizeof(msg), "Inc Th: %.1f", tempThreshold); // ข้อความแสดงค่าใหม่
            mcu.displayOLED(msg); // แสดงบน OLED
            delay(300); // หน่วงเวลา
        }
    }
    else if (!sw1 && sw2) // ถ้ากด SW2 เพียงปุ่มเดียว
    {
        if (millis() - lastBtn > 200) // หน่วงป้องกันการ repeat
        {
            tempThreshold -= 0.5; // ลดค่า threshold
            lastBtn = millis(); // อัปเดตเวลา
            saved = false; // ยังไม่ save
            mcu.beep(1, 80); // เสียง beep เมื่อ ลดค่า
            char msg[22];
            snprintf(msg, sizeof(msg), "Dec Th: %.1f", tempThreshold); // ข้อความแสดงค่าใหม่
            mcu.displayOLED(msg); // แสดงบน OLED
            delay(300); // หน่วงเวลา
        }
    }
    else if (sw1 && sw2) // ถ้ากดทั้งสองปุ่มพร้อมกัน
    {
        if (!saved) // ถ้ายังไม่ได้ save
        {
            saveThresholdToEEPROM(); // save ค่า threshold ลง EEPROM
            Serial.println("Threshold saved to EEPROM!"); // แสดงข้อความ
            mcu.beep(2, 100); // เสียง beep 2 ครั้งเมื่อ save
            mcu.displayOLED("Threshold Saved!"); // แสดงข้อความบน OLED
            delay(500); // หน่วงเวลา
            saved = true; // กัน save ซ้ำ
        }
    }
    else
    {
        saved = false; // reset flag เมื่อปล่อยปุ่ม
    }

    char line1[22], line2[22], line3[22]; // ตัวแปรเก็บข้อความแต่ละบรรทัด
    snprintf(line1, sizeof(line1), "IP:%s", WiFi.localIP().toString().c_str()); // แสดง IP
    snprintf(line2, sizeof(line2), "Temp: %.2fC Th:%.1f", temp, tempThreshold); // แสดง temp และ threshold
    snprintf(line3, sizeof(line3), "Relay:%s H:%.2f%%", relayState ? "ON" : "OFF", humid); // แสดงสถานะ relay และ humid
    mcu.displayOLEDLines(line1, line2, line3); // แสดงข้อความทั้ง 3 บรรทัดบน OLED

    server.handleClient(); // รอรับ request จาก web browser

    esp_task_wdt_reset(); // รีเซ็ต Watchdog Timer
    delay(1000); // หน่วงเวลา 1 วินาที

    static long lastalert = 0; // ตัวแปรเก็บเวลาการแจ้งเตือนล่าสุด
    if (millis() - lastalert > 60000) // ทุก 60 วินาที
    {
        lastalert = millis(); // อัปเดตเวลา
        if (temp > tempThreshold) // ถ้าอุณหภูมิสูงกว่า threshold
        {
            Serial.println("Alert: High temperature detected!"); // แจ้งเตือนผ่าน Serial
            mcu.displayOLED("Alert: High Temp!"); // แจ้งเตือนบน OLED
            mcu.beep(3, 200); // ส่งเสียงเตือน
        }
        else
        {
            Serial.println("Temperature and humidity are normal."); // แจ้งเตือนผ่าน Serial
            mcu.displayOLED("Temp/Humid Normal"); // แจ้งเตือนบน OLED
        }
    }
}