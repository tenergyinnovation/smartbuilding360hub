/***********************************************************************
 * Project      :     smartbuilding360hub BH1750 Light Sensor (Erriez Library)
 * Description  :     ตัวอย่างการอ่านค่าความเข้มแสงจากเซนเซอร์ BH1750 (ErriezBH1750) และแสดงผลทาง Serial และ OLED
 *                   ทำงานในโหมด Continuous และความละเอียดสูง (ResolutionHigh)
 * Hardware     :     tenergy32hub + BH1750
 * Author       :     Tenergy Innovation Co., Ltd.
 * Date         :     26/06/2025
 * Revision     :     1.4
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
String version = "1.4";

/**************************************/
/*          Header project            */
/**************************************/
void header_print(void)
{
    Serial.printf("\r\n***********************************************************************\r\n");
    Serial.printf("* Project      :     smartbuilding360hub BH1750 Light Sensor (Erriez)\r\n");
    Serial.printf("* Description  :     Read light intensity from BH1750 (Erriez) and show on Serial & OLED\r\n");
    Serial.printf("* Hardware     :     tenergy32hub + BH1750\r\n");
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

/**************************************/
/*        define global variable      */
/**************************************/

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

    // เริ่มต้นเซนเซอร์ BH1750 ในโหมด Continuous และความละเอียดสูง (ResolutionHigh)
    bh1750.begin(ModeContinuous, ResolutionHigh);

    // Start conversion
    bh1750.startConversion();

    esp_task_wdt_init(WDT_TIMEOUT, true); // ตั้งค่า Watchdog Timer
    esp_task_wdt_add(NULL);

    Serial.println("System Ready");
}

/***********************************************************************
 * FUNCTION:    loop
 * DESCRIPTION: อ่านค่าแสงและแสดงผลทาง Serial และ OLED
 ***********************************************************************/
void loop()
{
    uint16_t lux;

    // ตรวจสอบว่าข้อมูลพร้อมอ่านหรือยัง (Continuous mode, High resolution)
    if (bh1750.isConversionCompleted())
    {
        // อ่านค่าความเข้มแสง (lux)
        lux = bh1750.read();

        // แสดงผลทาง Serial Monitor โดยแสดงทศนิยม 1 ตำแหน่ง (lux/1.2)
        float lux_f = lux / 1.2;
        Serial.print(F("Light: "));
        Serial.print(lux_f, 1);
        Serial.println(F(" LUX"));

        // แสดงผลบน OLED
        char oledText[32];
        snprintf(oledText, sizeof(oledText), "Light: %.1f LUX", lux_f);
        mcu.displayOLED(oledText);
    }

    esp_task_wdt_reset(); // รีเซ็ต Watchdog Timer
    delay(500);           // หน่วงเวลา 0.5 วินาที
}
