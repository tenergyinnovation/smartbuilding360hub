/***********************************************************************
 * Project      :     smartbuilding360hub BH1750 Light Sensor (Erriez Library)
 * Description  :     ตัวอย่างการอ่านค่าความเข้มแสงจากเซนเซอร์ BH1750 (ErriezBH1750) และแสดงผลทาง Serial ทำงานในโหมด OneTime และความละเอียดกลาง (Mid)
 * Hardware     :     tenergy32hub + BH1750
 * Author       :     Tenergy Innovation Co., Ltd.
 * Date         :     26/06/2025
 * Revision     :     1.2
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
String version = "1.2";

/**************************************/
/*          Header project            */
/**************************************/
void header_print(void)
{
    Serial.printf("\r\n***********************************************************************\r\n");
    Serial.printf("* Project      :     smartbuilding360hub BH1750 Light Sensor (Erriez)\r\n");
    Serial.printf("* Description  :     Read light intensity from BH1750 (Erriez) and show on Serial\r\n");
    Serial.printf("* Hardware     :     tenergy32hub + BH1750\r\n");
    Serial.printf("* Author       :     Tenergy Innovation Co., Ltd.\r\n");
    Serial.printf("* Date         :     26/06/2025\r\n");
    Serial.printf("* Revision     :     %s\r\n", version.c_str());
    Serial.printf("***********************************************************************/\r\n");
}

/**************************************/
/*        define object variable      */
/**************************************/
Tenergy32Hub mcu;
BH1750 bh1750(LOW); // กำหนด I2C address (0x23 หรือ 0x5C)

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
 ***********************************************************************/
void setup()
{
    Serial.begin(115200);
    header_print();

    mcu.begin();
    mcu.displayOLEDInfo();
    vTaskDelay(1000);

  

    Wire.begin();

    // เริ่มต้นเซนเซอร์ BH1750 ในโหมด OneTime และความละเอียดต่ำ (Low)
    // สามารถเปลี่ยนเป็น ModeContinuous และ ResolutionMid/High ได้ตามต้องการ
    // ResolutionLow: จะให้ความละเอียดต่ำสุดที่ 4 lx
    // ResolutionMid: จะให้ความละเอียดที่ 1 lx
    // ResolutionHigh: จะให้ความละเอียดสูงสุดที่ 0.5 lx

    bh1750.begin(ModeOneTime, ResolutionMid);

    esp_task_wdt_init(WDT_TIMEOUT, true);
    esp_task_wdt_add(NULL);

    Serial.println("System Ready");
}

/***********************************************************************
 * FUNCTION:    loop
 ***********************************************************************/
void loop()
{
    uint16_t lux;

    // Start conversion (OneTime mode)
    bh1750.startConversion();

    // Wait synchronous for completion (blocking)
    if (bh1750.waitForCompletion())
    {
        // Read light
        lux = bh1750.read();

        // Print light
        Serial.print("Light: ");
        Serial.print(lux);
        Serial.println(" LUX");
    }
    else
    {
        Serial.println("Light: -");
    }

    esp_task_wdt_reset();
    delay(500);
}