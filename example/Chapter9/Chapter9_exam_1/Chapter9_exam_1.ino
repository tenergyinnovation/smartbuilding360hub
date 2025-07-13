/***********************************************************************
 * Project      :     smartbuilding360hub SHT30 Temp & Humidity Sensor
 * Description  :     อ่านค่าอุณหภูมิและความชื้นจาก SHT30 และแสดงผลทาง Serial & OLED
 * Hardware     :     tenergy32hub + SHT30
 * Author       :     Tenergy Innovation Co., Ltd.
 * Date         :     26/06/2025
 * Revision     :     1.0
 ***********************************************************************/
#include <Arduino.h>
#include <tenergy32hub.h>
#include <Wire.h>
#include <Adafruit_SHT31.h> // ใช้ไลบรารี Adafruit SHT31
#include <esp_task_wdt.h>
#include <esp_system.h> // สำหรับ esp_read_mac

/**************************************/
/*          Firmware Version          */
/**************************************/
String version = "1.0";

/**************************************/
/*        define global variable      */
/**************************************/


/**************************************/
/*          Header project            */
/**************************************/
void header_print(void)
{
    Serial.printf("\r\n***********************************************************************\r\n");
    Serial.printf("* Project      :     smartbuilding360hub SHT30 Temp & Humidity Sensor\r\n");
    Serial.printf("* Description  :     Read temperature & humidity from SHT30, show on Serial & OLED\r\n");
    Serial.printf("* Hardware     :     tenergy32hub + SHT30\r\n");
    Serial.printf("* Author       :     Tenergy Innovation Co., Ltd.\r\n");
    Serial.printf("* Date         :     26/06/2025\r\n");
    Serial.printf("* Revision     :     %s\r\n", version.c_str());
    Serial.printf("***********************************************************************/\r\n");
}


/**************************************/
/*        define object variable      */
/**************************************/
Tenergy32Hub mcu;                        // อ็อบเจกต์ควบคุมบอร์ด tenergy32hub
Adafruit_SHT31 sht30 = Adafruit_SHT31(); // อ็อบเจกต์เซนเซอร์ SHT30

/**************************************/
/*       Constant define value        */
/**************************************/
#define WDT_TIMEOUT 10

/***********************************************************************
 * FUNCTION:    setup
 * DESCRIPTION: กำหนดค่าเริ่มต้นระบบและเซนเซอร์
 ***********************************************************************/
void setup()
{
    Serial.begin(115200); // เริ่มต้น Serial Monitor
    header_print();       // แสดงข้อมูลโปรเจกต์

    mcu.begin();           // เริ่มต้นบอร์ด tenergy32hub
    mcu.displayOLEDInfo(); // แสดงข้อมูลบน OLED
    vTaskDelay(1000);      // หน่วงเวลา 1 วินาที

    Wire.begin(); // เริ่มต้น I2C

    // เริ่มต้นเซนเซอร์ SHT30
    if (!sht30.begin(0x44))
    { // 0x44 คือ I2C address ของ SHT30
        Serial.println("Couldn't find SHT30 sensor!");
        mcu.displayOLED("SHT30 not found!");
        while (1)
            delay(1);
    }

    esp_task_wdt_init(WDT_TIMEOUT, true); // ตั้งค่า Watchdog Timer
    esp_task_wdt_add(NULL);

    Serial.println("System Ready");
}

/***********************************************************************
 * FUNCTION:    loop
 * DESCRIPTION: อ่านค่าอุณหภูมิและความชื้น แสดงผลทาง Serial และ OLED
 ***********************************************************************/
void loop()
{
    float temperature = sht30.readTemperature(); // อ่านค่าอุณหภูมิ (°C)
    float humidity = sht30.readHumidity();       // อ่านค่าความชื้น (%RH)
    char line1[22], line2[22];

    // ตรวจสอบว่าค่าอ่านได้ถูกต้องหรือไม่
    if (!isnan(temperature) && !isnan(humidity))
    {
        // แสดงผลทาง Serial Monitor
        Serial.print("Temp: ");
        Serial.print(temperature, 1);
        Serial.print(" C | Humidity: ");
        Serial.print(humidity, 1);
        Serial.println(" %");

        // เตรียมข้อความแสดงบน OLED
        snprintf(line1, sizeof(line1), "Temp: %.1f C", temperature);
        snprintf(line2, sizeof(line2), "Humidity: %.1f %%", humidity);

        // แสดงผลบน OLED
        mcu.displayOLEDLines(line1, line2, "", "");
    }
    else
    {
        Serial.println("Sensor read error!");
        mcu.displayOLED("Sensor read error!");
    }

    esp_task_wdt_reset(); // รีเซ็ต Watchdog Timer
    delay(1000);          // หน่วงเวลา 1 วินาที
}