/***********************************************************************
 * Project      :     tenergy32hub_bh1750_lora_sensor
 * Description  :     Read illuminance (lux) from BH1750 sensor and send as JSON via LoRa to Gateway Board
 * Hardware     :     tenergy32hub + BH1750
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
#include <ErriezBH1750.h> // Include the ErriezBH1750 library for BH1750 sensor

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
    Serial.printf("* Project      :     tenergy32hub_bh1750_lora_sensor\r\n");
    Serial.printf("* Description  :     Read illuminance (lux) from BH1750 sensor and send as JSON via LoRa to Gateway Board\r\n");
    Serial.printf("* Hardware     :     tenergy32hub + BH1750\r\n");
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
BH1750 bh1750(LOW); // อ็อบเจกต์เซนเซอร์ BH1750 (I2C address 0x23)

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
uint8_t loraCounter = 1;
String unitName = "";
String fwVersion = "0.1";
String topic = "esp32hub";

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
    Serial.begin(115200);
    header_print();

    mcu.begin();
    mcu.displayOLEDInfo();

    unitName = getUnitNameFromMac();
    Serial.printf("unitName: %s\r\n", unitName.c_str());
    mcu.displayOLED(unitName.c_str());

    // เริ่มต้นเซนเซอร์ BH1750 ในโหมด Continuous และความละเอียดกลาง (ResolutionMid)
    bh1750.begin(ModeContinuous, ResolutionMid);
    bh1750.startConversion(); // เริ่มต้นการแปลงค่าแสง

    // Initialize and enable the watchdog with a 10-second timeout.
    esp_task_wdt_init(WDT_TIMEOUT, true);
    esp_task_wdt_add(NULL);
}

/***********************************************************************
 * FUNCTION:    loop
 * DESCRIPTION: loop process
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void loop()
{
    float lux = 0;
    // ตรวจสอบว่าข้อมูลพร้อมอ่านหรือยัง (Continuous mode)
    if (bh1750.isConversionCompleted())
    {
        // อ่านค่าความเข้มแสง (lux)
        lux = bh1750.read();
        char line1[22], line2[22], line3[22], line4[22];
        snprintf(line1, sizeof(line1), "Unit: %s", unitName.c_str());
        snprintf(line2, sizeof(line2), "Lux: %.1f lx", lux);
        snprintf(line3, sizeof(line3), "Counter: %u", loraCounter);
        snprintf(line4, sizeof(line4), " "); // เว้นว่าง

        // Print to Serial Monitor
        Serial.printf("Unit: %s | Lux: %.1f lx | Counter: %u\r\n", unitName.c_str(), lux, loraCounter);
        // Display on OLED
        mcu.displayOLEDLines(line1, line2, line3, line4);
    }

    // --- Send data via LoRa ---
    static long lastSendTime = 0;
    if (millis() - lastSendTime >= 10000) // Send every 10 seconds
    {
        // --- สร้าง JSON ข้อมูลที่จะส่ง ---
        StaticJsonDocument<256> doc;
        doc["id"] = unitName;
        doc["fw"] = fwVersion;
        doc["topic"] = topic;
        doc["counter"] = loraCounter++;
        doc["param_1"] = NULL;
        doc["param_2"] = NULL;
        doc["param_3"] = lux;
        doc["param_4"] = NULL;
        doc["param_5"] = NULL;
        doc["param_6"] = NULL;
        doc["param_7"] = NULL;
        doc["param_8"] = NULL;
        doc["param_9"] = NULL;
        doc["param_10"] = NULL;

        if (loraCounter > 255)
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
            Serial.printf("Data size: %d bytes\n", (int)len);
            mcu.clearOLED();
            char sizeStr[32];
            snprintf(sizeStr, sizeof(sizeStr), "Size: %d bytes", (int)len);
            char counterStr[16];
            snprintf(counterStr, sizeof(counterStr), "Count: %u", loraCounter - 1);
            mcu.displayOLEDLines("LoRa sent OK", sizeStr, counterStr);
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