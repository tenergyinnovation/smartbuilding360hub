/***********************************************************************
 * Project      :     smartbuilding360hub Temp & Humi & Relay Control
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
#include <esp_system.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_SHT31.h>

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
Tenergy32Hub mcu;                        // สร้างอ็อบเจกต์ mcu สำหรับควบคุมบอร์ด tenergy32hub
Adafruit_SHT31 sht30 = Adafruit_SHT31(); // อ็อบเจกต์ SHT30

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

// เพิ่มตัวแปรสถานะ relay
bool relayState = false;

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

// เพิ่มส่วนนี้ก่อน setup()
const char* ssid = "TENERGYINNOVATION";
const char* password = "L0vemel0vemydog";

#include <WebServer.h>
WebServer server(80);

float temp = 0, humid = 0;

// ฟังก์ชันสำหรับตอบสนองหน้าเว็บ
void handleRoot() {
    String html = "<!DOCTYPE html><html><head>";
    html += "<meta charset='utf-8'>";
    html += "<meta http-equiv='refresh' content='5'>"; // อัปเดตหน้าเว็บทุก 5 วินาที
    html += "<title>Sensor Data</title></head><body>";
    html += "<h2>Smart Building 360 Sensor</h2>";
    html += "<p>Temperature: <b>" + String(temp, 2) + " &deg;C</b></p>";
    html += "<p>Humidity: <b>" + String(humid, 2) + " %</b></p>";
    html += "<form action='/relay' method='POST'>";
    if (relayState) {
        html += "<p>Relay Status: <b>ON</b></p>";
        html += "<button type='submit' name='relay' value='off'>Turn OFF Relay</button>";
    } else {
        html += "<p>Relay Status: <b>OFF</b></p>";
        html += "<button type='submit' name='relay' value='on'>Turn ON Relay</button>";
    }
    html += "</form>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

// ฟังก์ชันจัดการการกดปุ่ม relay
void handleRelay() {
    if (server.method() == HTTP_POST) {
        if (server.hasArg("relay")) {
            String cmd = server.arg("relay");
            if (cmd == "on") {
                mcu.relayOn();
                relayState = true;
            } else if (cmd == "off") {
                mcu.relayOff();
                relayState = false;
            }
        }
    }
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
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
    Serial.printf("unitName: %s\r\n", unitName.c_str());
    mcu.displayOLED(unitName.c_str());

    if (!sht30.begin(0x44))
    { // เริ่มต้น SHT30 ที่ address 0x44
        Serial.println("Couldn't find SHT30 sensor!");
        mcu.displayOLED("SHT30 not found!");
        while (1)
            delay(10);
    }

    // เชื่อมต่อ WiFi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    mcu.displayOLED("Connecting WiFi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    mcu.displayOLED(WiFi.localIP().toString().c_str());

    // ตั้งค่า Web Server
    server.on("/", handleRoot);
    server.on("/relay", HTTP_POST, handleRelay);
    server.begin();

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

    char line1[22], line2[22], line3[22];
    snprintf(line1, sizeof(line1), "IP:%s", WiFi.localIP().toString().c_str());
    snprintf(line2, sizeof(line2), "Temp: %.2f C", temp);
    snprintf(line3, sizeof(line3), "Relay:%s H:%.2f%%", relayState ? "ON" : "OFF", humid);
    mcu.displayOLEDLines(line1, line2, line3); // แสดง 3 บรรทัดบน OLED

    server.handleClient();

    esp_task_wdt_reset();
    delay(1000); // อัปเดตทุก 1 วินาที
}