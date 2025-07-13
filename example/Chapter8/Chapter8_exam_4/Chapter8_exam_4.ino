/***********************************************************************
 * Project      :     smartbuilding360hub BH1750 Light Sensor (Erriez Library)
 * Description  :     อ่านค่าแสงจาก BH1750 และควบคุมรีเลย์ตามระดับแสง
 * Hardware     :     tenergy32hub + BH1750 + Relay
 * Author       :     Tenergy Innovation Co., Ltd.
 * Date         :     26/06/2025
 * Revision     :     1.5
 ***********************************************************************/
#include <Arduino.h>
#include <tenergy32hub.h>
#include <Wire.h>
#include <ErriezBH1750.h>
#include <esp_task_wdt.h>
#include <esp_system.h> // สำหรับ esp_read_mac

/**************************************/
/*          Firmware Version          */
/**************************************/
String version = "1.5";

/**************************************/
/*          Header project            */
/**************************************/
void header_print(void)
{
    Serial.printf("\r\n***********************************************************************\r\n");
    Serial.printf("* Project      :     smartbuilding360hub BH1750 Light Sensor (Erriez)\r\n");
    Serial.printf("* Description  :     Read light intensity from BH1750, control relay, show on Serial & OLED\r\n");
    Serial.printf("* Hardware     :     tenergy32hub + BH1750 + Relay\r\n");
    Serial.printf("* Author       :     Tenergy Innovation Co., Ltd.\r\n");
    Serial.printf("* Date         :     26/06/2025\r\n");
    Serial.printf("* Revision     :     %s\r\n", version.c_str());
    Serial.printf("***********************************************************************/\r\n");
}

/**************************************/
/*        define object variable      */
/**************************************/
Tenergy32Hub mcu;   // อ็อบเจกต์ควบคุมบอร์ด tenergy32hub
BH1750 bh1750(LOW); // อ็อบเจกต์เซนเซอร์ BH1750 (I2C address 0x23)

/**************************************/
/*       Constant define value        */
/**************************************/
#define WDT_TIMEOUT 10
#define RELAY_PIN 25 // กำหนดขารีเลย์ (แก้ไขตามการต่อจริง)

/**************************************/
/*        define global variable      */
/**************************************/
bool relayState = false; // สถานะรีเลย์

/**************************************/
/*           define function          */
/**************************************/


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

    // เริ่มต้นเซนเซอร์ BH1750 ในโหมด Continuous และความละเอียดกลาง (ResolutionMid)
    bh1750.begin(ModeContinuous, ResolutionMid);
    bh1750.startConversion(); // เริ่มต้นการแปลงค่าแสง

    pinMode(RELAY_PIN, OUTPUT);   // กำหนดขารีเลย์เป็น output
    digitalWrite(RELAY_PIN, LOW); // ปิดรีเลย์เริ่มต้น

    esp_task_wdt_init(WDT_TIMEOUT, true); // ตั้งค่า Watchdog Timer
    esp_task_wdt_add(NULL);

    Serial.println("System Ready");
}

/***********************************************************************
 * FUNCTION:    loop
 * DESCRIPTION: อ่านค่าแสง ควบคุมรีเลย์ และแสดงผล
 ***********************************************************************/
void loop()
{
    uint16_t lux;
    char line1[22], line2[22];

    // ตรวจสอบว่าข้อมูลพร้อมอ่านหรือยัง (Continuous mode)
    if (bh1750.isConversionCompleted())
    {
        // อ่านค่าความเข้มแสง (lux)
        lux = bh1750.read();

        // ควบคุมรีเลย์ตามระดับแสง
        if (lux < 5)
        {
            // ถ้าไม่มีแสง ให้รีเลย์ ON
            digitalWrite(RELAY_PIN, HIGH);
            relayState = true;
        }
        else if (lux > 30)
        {
            // ถ้ามีแสง ให้รีเลย์ OFF
            digitalWrite(RELAY_PIN, LOW);
            relayState = false;
        }
        // แสดงผลทาง Serial Monitor
        Serial.print(F("Light: "));
        Serial.print(lux);
        Serial.print(F(" LUX | Relay: "));
        Serial.println(relayState ? "ON" : "OFF");

        // เตรียมข้อความแสดงบน OLED
        snprintf(line1, sizeof(line1), "Light: %d LUX", lux);
        snprintf(line2, sizeof(line2), "Relay: %s", relayState ? "ON" : "OFF");

        // แสดงผลบน OLED
        mcu.displayOLEDLines(line1, line2, "", "");
    }
    else
    {
        Serial.println(F("Light: - | Relay: -"));
        mcu.displayOLEDLines("Light: -", "Relay: -", "", "");
    }

    esp_task_wdt_reset(); // รีเซ็ต Watchdog Timer
    delay(500);           // หน่วงเวลา 0.5 วินาที
}