/***********************************************************************
 * Project      :     tenergy32hub_sht30_lora_sensor
 * Description  :     Read temperature & humidity from SHT30 sensor and send as JSON via LoRa to Gateway Board
 * Hardware     :     tenergy32hub + SHT30
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
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>

/**************************************/
/*          Firmware Version          */
/**************************************/
String version = "0.1";

/**************************************/
/*          Header project            */
/**************************************/
void header_print(void)
{
    Serial.printf("\r\n***********************************************************************\r\n");
    Serial.printf("* Project      :     tenergy32hub_sht30_lora_sensor\r\n");
    Serial.printf("* Description  :     Read temperature & humidity from SHT30 sensor and send as JSON via LoRa to Gateway Board\r\n");
    Serial.printf("* Hardware     :     tenergy32hub + SHT30\r\n");
    Serial.printf("* Author       :     Tenergy Innovation Co., Ltd.\r\n");
    Serial.printf("* Date         :     04/07/2022\r\n");
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
Tenergy32Hub mcu;
Adafruit_SHT31 sht30 = Adafruit_SHT31(); // อ็อบเจกต์เซนเซอร์ SHT30

/**************************************/
/*            GPIO define             */
/**************************************/

/**************************************/
/*       Constand define value        */
/**************************************/
// 10 seconds WDT
#define WDT_TIMEOUT 10

/**************************************/
/*       eeprom address define        */
/**************************************/

/**************************************/
/*        define global variable      */
/**************************************/

// เพิ่มตัวแปรนับรอบไว้ด้านบน (global)
uint8_t loraCounter = 1;
String unitName = "";
String fwVersion = "0.1";  // Version ของโปรแกรมที่ใช้ในบอร์ดนี้
String topic = "esp32hub"; // Topic ที่ใช้ในการส่งข้อมูลไปยัง LoRa Gateway

/**************************************/
/*           define function          */
/**************************************/
String getUnitNameFromMac();

/***********************************************************************
 * FUNCTION:    getUnitNameFromMac
 * DESCRIPTION: สร้างชื่อ unitName จาก MAC Address (6 ตัวหลัง)
 * RETURNED:    String ชื่อบอร์ด tenergy32gateway-xxxxxx
 ***********************************************************************/
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
    // Initialize serial communication and print the header
    Serial.begin(115200);
    header_print();

    mcu.begin();
    mcu.displayOLEDInfo();

    unitName = getUnitNameFromMac();
    Serial.printf("unitName: %s\r\n", unitName.c_str());
    mcu.displayOLED(unitName.c_str());

    // เริ่มต้นเซนเซอร์ SHT30
    if (!sht30.begin(0x44))
    {
        Serial.println("Couldn't find SHT30 sensor!");
        mcu.displayOLED("SHT30 not found!");
        vTaskDelay(1000); // Wait for a second before halting
        mcu.beep(3, 100); // Beep 3 times to indicate error
        while (1)
            delay(1);
    }

    // Initialize and enable the watchdog with a 10-second timeout.
    esp_task_wdt_init(WDT_TIMEOUT, true); // true resets the CPU on WDT timeout
    esp_task_wdt_add(NULL);               // Add current task to watchdog monitoring
}

/***********************************************************************
 * FUNCTION:    loop
 * DESCRIPTION: loop process
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void loop()
{
    float temperature = sht30.readTemperature();
    float humidity = sht30.readHumidity();
    char line1[22], line2[22], line3[22], line4[22];
    snprintf(line1, sizeof(line1), "Unit: %s", unitName.c_str());
    snprintf(line2, sizeof(line2), "Temp: %.1f C", temperature);
    snprintf(line3, sizeof(line3), "Hum: %.1f %%", humidity);
    snprintf(line4, sizeof(line4), "Counter: %d", loraCounter);

    // Print to Serial Monitor
    Serial.printf("Unit: %s | Temp: %.1f C | Hum: %.1f %% | Counter: %d\r\n", unitName.c_str(), temperature, humidity, (int)loraCounter);
    // Display on OLED
    mcu.displayOLEDLines(line1, line2, line3, line4);

    // --- Send data via LoRa ---
    static long lastSendTime = 0;
    if (millis() - lastSendTime >= 10000) // Send every 10 seconds
    {
        // --- สร้าง JSON ข้อมูลที่จะส่ง ---
        StaticJsonDocument<256> doc;
        doc["id"] = unitName;
        doc["fw"] = fwVersion;
        doc["rssi"] = ""; // ถ้ายังไม่มีค่า rssi ให้ใส่ค่าว่างหรือค่าที่ต้องการ
        doc["topic"] = topic;
        doc["counter"] = loraCounter++;
        doc["param_1"] = temperature;
        doc["param_2"] = humidity;
        doc["param_3"] = NULL;
        doc["param_4"] = NULL;
        doc["param_5"] = NULL;
        doc["param_6"] = NULL;
        doc["param_7"] = NULL;
        doc["param_8"] = NULL;
        doc["param_9"] = NULL;
        doc["param_10"] = NULL;

        if (loraCounter > 255) // Reset counter if it exceeds 255
        {
            loraCounter = 1;
        }

        char jsonBuffer[256];
        size_t len = serializeJson(doc, jsonBuffer, sizeof(jsonBuffer));

        // --- ส่ง JSON ผ่าน LoRa ซ้ำ 5 รอบ ---
        bool sent = false;
        char line1[22];
        for (int i = 0; i < 5; ++i)
        {
            bool result = mcu.sendLoRa((uint8_t *)jsonBuffer, len);
            Serial.printf("Sending LoRa data[%d]: %s\n", i, jsonBuffer);
            snprintf(line1, sizeof(line1), "Sending LoRa...%d", i + 1);
            mcu.displayOLED(line1);

            if (result)
                sent = true; // ถ้าส่งสำเร็จอย่างน้อย 1 ครั้ง ให้ถือว่าสำเร็จ
            delay(100);      // หน่วงเล็กน้อยระหว่างแต่ละรอบ (เช่น 100ms)
        }
        if (sent)
        {
            Serial.printf("LoRa sent: %s\r\n", jsonBuffer);
            Serial.printf("Data size: %d bytes\n", (int)len); // แสดงขนาดข้อมูลที่ส่ง
            mcu.clearOLED();
            char sizeStr[32];
            snprintf(sizeStr, sizeof(sizeStr), "Size: %d bytes", (int)len);
            char counterStr[16];
            snprintf(counterStr, sizeof(counterStr), "Count: %d", loraCounter - 1); // -1 เพราะเพิ่มไปแล้ว
            mcu.displayOLEDLines("LoRa sent OK", sizeStr, counterStr);              // แสดงขนาดข้อมูลที่บรรทัด 2, counter ที่บรรทัด 3
        }
        else
        {
            Serial.println("LoRa send failed");
            mcu.displayOLED("LoRa send failed");
        }
        lastSendTime = millis();
    }

    esp_task_wdt_reset();
    vTaskDelay(1000);
}