/***********************************************************************
 * Project      :     smartbuilding360hub SHT30 Temp & Humidity Sensor + Buzzer Alert
 * Description  :     อ่านค่าอุณหภูมิและความชื้นจาก SHT30 แสดงผลทาง Serial & OLED
 *                   แจ้งเตือนด้วยเสียง Buzzer (ฟังก์ชัน beep) เมื่ออุณหภูมิสูงกว่าค่าที่กำหนด
 *                   สามารถตั้งค่า TEMP_ALERT ได้ด้วยปุ่ม SW1 (เพิ่ม) และ SW2 (ลด) ทีละ 0.5°C และบันทึกลง EEPROM
 * Hardware     :     tenergy32hub + SHT30 + Buzzer + SW1 + SW2 + Relay
 * Author       :     Tenergy Innovation Co., Ltd.
 * Date         :     26/06/2025
 * Revision     :     1.3
 ***********************************************************************/
#include <Arduino.h>
#include <tenergy32hub.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>
#include <EEPROM.h>
#include <esp_task_wdt.h>
#include <esp_system.h> // สำหรับ esp_read_mac

/**************************************/
/*          Firmware Version          */
/**************************************/
String version = "1.3";

/**************************************/
/*        define global variable      */
/**************************************/

/**************************************/
/*        define object variable      */
/**************************************/
Tenergy32Hub mcu;                        // อ็อบเจกต์ควบคุมบอร์ด tenergy32hub
Adafruit_SHT31 sht30 = Adafruit_SHT31(); // อ็อบเจกต์เซนเซอร์ SHT30

/**************************************/
/*       Constant define value        */
/**************************************/
#define WDT_TIMEOUT 10
#define TEMP_ALERT_MIN 10.0
#define TEMP_ALERT_MAX 50.0
#define EEPROM_SIZE 8
#define EEPROM_ADDR_TEMP_ALERT 0
#define RELAY_PIN 25 // กำหนดขารีเลย์ (แก้ไขตามการต่อจริง)

/**************************************/
/*        define setting value        */
/**************************************/
float TEMP_ALERT = 30.0; // ค่าอุณหภูมิแจ้งเตือนเริ่มต้น (°C)

/**************************************/
/*          Header project            */
/**************************************/
void header_print(void)
{
    Serial.printf("\r\n***********************************************************************\r\n");
    Serial.printf("* Project      :     smartbuilding360hub SHT30 Temp & Humidity Sensor + Buzzer Alert\r\n");
    Serial.printf("* Description  :     Read temperature & humidity from SHT30, show on Serial & OLED\r\n");
    Serial.printf("*                :     Alert with beep if temperature > %.1f C\r\n", TEMP_ALERT);
    Serial.printf("*                :     Set TEMP_ALERT by SW1(+), SW2(-) step 0.5C\r\n");
    Serial.printf("*                :     Save TEMP_ALERT to EEPROM\r\n");
    Serial.printf("*                :     Relay ON if Temp > TEMP_ALERT, OFF otherwise\r\n");
    Serial.printf("* Hardware     :     tenergy32hub + SHT30 + Buzzer + SW1 + SW2 + Relay\r\n");
    Serial.printf("* Author       :     Tenergy Innovation Co., Ltd.\r\n");
    Serial.printf("* Date         :     26/06/2025\r\n");
    Serial.printf("* Revision     :     %s\r\n", version.c_str());
    Serial.printf("***********************************************************************/\r\n");
}

/***********************************************************************
 * FUNCTION:    saveTempAlertToEEPROM
 * DESCRIPTION: บันทึกค่า TEMP_ALERT ลงใน EEPROM
 ***********************************************************************/
void saveTempAlertToEEPROM(float value)
{
    EEPROM.put(EEPROM_ADDR_TEMP_ALERT, value); // เขียนค่า float ลง EEPROM
    EEPROM.commit();                           // สั่งให้บันทึกจริง
}

/***********************************************************************
 * FUNCTION:    loadTempAlertFromEEPROM
 * DESCRIPTION: อ่านค่า TEMP_ALERT จาก EEPROM
 ***********************************************************************/
float loadTempAlertFromEEPROM()
{
    float value = 30.0;
    EEPROM.get(EEPROM_ADDR_TEMP_ALERT, value);
    // ตรวจสอบค่าที่อ่านได้ว่าถูกต้องหรือไม่
    if (isnan(value) || value < TEMP_ALERT_MIN || value > TEMP_ALERT_MAX)
        value = 30.0;
    return value;
}

/***********************************************************************
 * FUNCTION:    setup
 * DESCRIPTION: กำหนดค่าเริ่มต้นระบบและเซนเซอร์
 ***********************************************************************/
void setup()
{
    Serial.begin(115200);
    header_print();

    mcu.begin();
    mcu.displayOLEDInfo();
    vTaskDelay(1000);

    Wire.begin();

    // เริ่มต้น EEPROM
    EEPROM.begin(EEPROM_SIZE);

    // โหลดค่า TEMP_ALERT จาก EEPROM
    TEMP_ALERT = loadTempAlertFromEEPROM();
    Serial.printf("Load TEMP_ALERT from EEPROM: %.1f C\r\n", TEMP_ALERT);

    // เริ่มต้นเซนเซอร์ SHT30
    if (!sht30.begin(0x44))
    {
        Serial.println("Couldn't find SHT30 sensor!");
        mcu.displayOLED("SHT30 not found!");
        while (1)
            delay(1);
    }

    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW); // ปิดรีเลย์เริ่มต้น

    esp_task_wdt_init(WDT_TIMEOUT, true);
    esp_task_wdt_add(NULL);

    Serial.println("System Ready");
}

/***********************************************************************
 * FUNCTION:    checkButton
 * DESCRIPTION: ตรวจสอบการกดปุ่ม SW1 และ SW2 (ผ่านฟังก์ชันใน library tenergy32hub)
 ***********************************************************************/
void checkButton()
{
    static uint32_t lastPress = 0;
    if (millis() - lastPress < 250)
        return; // ป้องกันการเด้งของปุ่ม

    // ใช้ฟังก์ชัน mcu.readSW1() และ mcu.readSW2()
    if (mcu.readSW1())
    {
        if (TEMP_ALERT < TEMP_ALERT_MAX)
        {
            TEMP_ALERT += 0.5;
            saveTempAlertToEEPROM(TEMP_ALERT); // บันทึกค่าใหม่ลง EEPROM
            mcu.beep(1, 100);
            Serial.printf("Set TEMP_ALERT: %.1f C\r\n", TEMP_ALERT);
            char buf[32];
            snprintf(buf, sizeof(buf), "Set ALERT: %.1f C", TEMP_ALERT);
            mcu.displayOLED(buf);
        }
        lastPress = millis();
    }
    if (mcu.readSW2())
    {
        if (TEMP_ALERT > TEMP_ALERT_MIN)
        {
            TEMP_ALERT -= 0.5;
            saveTempAlertToEEPROM(TEMP_ALERT); // บันทึกค่าใหม่ลง EEPROM
            mcu.beep(1, 100);
            Serial.printf("Set TEMP_ALERT: %.1f C\r\n", TEMP_ALERT);
            char buf[32];
            snprintf(buf, sizeof(buf), "Set ALERT: %.1f C", TEMP_ALERT);
            mcu.displayOLED(buf);
        }
        lastPress = millis();
    }
}

/***********************************************************************
 * FUNCTION:    loop
 * DESCRIPTION: อ่านค่าอุณหภูมิและความชื้น แสดงผลทาง Serial และ OLED
 *              แจ้งเตือนด้วยเสียง beep เมื่ออุณหภูมิสูงกว่าค่าที่กำหนด
 *              สามารถตั้งค่า TEMP_ALERT ได้ด้วย SW1 และ SW2
 *              ควบคุม Relay ตามอุณหภูมิ
 ***********************************************************************/
void loop()
{
    checkButton();

    float temperature = sht30.readTemperature();
    float humidity = sht30.readHumidity();
    char line1[22], line2[22], line3[22], line4[22];

    // ตรวจสอบว่าค่าอ่านได้ถูกต้องหรือไม่
    if (!isnan(temperature) && !isnan(humidity))
    {
        // ตรวจสอบอุณหภูมิ ถ้าสูงกว่าค่าที่กำหนดให้แจ้งเตือนด้วย beep และ Relay ON
        if (temperature > TEMP_ALERT)
        {
            mcu.beep(3, 300); // ส่งเสียง beep 3 ครั้ง ครั้งละ 300 ms
            mcu.relayOn();    // สั่ง Relay ON ด้วยฟังก์ชันใน library
            Serial.print("ALERT! ");
            snprintf(line1, sizeof(line1), "ALERT! Temp: %.1f C", temperature);
        }
        else
        {
            mcu.relayOff(); // สั่ง Relay OFF ด้วยฟังก์ชันใน library
            snprintf(line1, sizeof(line1), "Temp: %.1f C", temperature);
        }

        // แสดงผลทาง Serial Monitor
        Serial.print("Temp: ");
        Serial.print(temperature, 1);
        Serial.print(" C | Humidity: ");
        Serial.print(humidity, 1);
        Serial.print(" %% | ALERT: ");
        Serial.print(TEMP_ALERT, 1);
        Serial.print(" | Relay: ");
        Serial.println((temperature > TEMP_ALERT) ? "ON" : "OFF");

        // เตรียมข้อความแสดงบน OLED
        snprintf(line2, sizeof(line2), "Humidity: %.1f %", humidity);
        snprintf(line3, sizeof(line3), "Alert: %.1f C", TEMP_ALERT);

        // แสดงผลบน OLED
        mcu.displayOLEDLines(line1, line2, line3, "");
    }
    else
    {
        Serial.println("Sensor read error!");
        mcu.displayOLED("Sensor read error!");
        mcu.relayOff(); // ปิดรีเลย์กรณีอ่านค่าไม่ได้
    }

    // Reset watchdog timer
    esp_task_wdt_reset();
    delay(500);
}