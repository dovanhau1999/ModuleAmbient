#include "MCU.h"
#include "mcc_generated_files/pin_manager.h"

#define _CheckTimeTick              2000

//bool LED_Statuc;
int8_t SW_Ad;
int8_t f_Indicator;

enum LED_STATUS
{
    OFF_Sensor = 0, 
    ON_Sensor = 1,
    ERR_Sensor = 2
};

extern int16_t Temperature;
extern int16_t Humidity; 


void Task_Sensor(void)
{
    static uint32_t valTime = 0; 
    static int16_t *_pTemp = &Temperature;
    static int16_t *_pHumi = &Humidity;
    
    f_Indicator = OFF_Sensor;
    
    valTime = Get_millis();
    /* Check Timer Tick*/
    if (((uint32_t) Get_millis() - valTime) >= (uint32_t) _CheckTimeTick) 
    {
        f_Indicator = ON_Sensor;
        ReadData();
        if ( (Temperature < 0) || (Humidity < 0) )
        {
            *_pTemp = 0x8000;
            *_pHumi = 0x8000;
            f_Indicator = ERR_Sensor;
        } 
    }
}

void Task_MB(void)
{
    static bool f_Modbus_Init = false;
    if (!f_Modbus_Init)
    {
        ModbusSlave_Init();
        f_Modbus_Init = true;
    } else 
    {
        ModbusSlave_Process();
    }
}

void Task_Indicator()
{
    switch (f_Indicator)
    {
        case OFF_Sensor:
        {
            LED_GREEN_SetHigh();
            LED_RED_SetHigh();
            break;
        }
        case ON_Sensor:
        {
            LED_GREEN_SetLow();
            LED_RED_SetHigh();
            break;
        }
        case ERR_Sensor:
        {
            LED_GREEN_SetHigh();
            LED_RED_SetLow();
            break;
        }
    }
}

void Device_Init(void)
{
    static int8_t value_SW1, value_SW2, value_SW3, value_SW4;
    static int8_t *pWS_Ad = &SW_Ad;
    
    value_SW1 = SW1_GetValue();
    value_SW2 = SW2_GetValue();
    value_SW3 = SW3_GetValue();
    value_SW4 = SW4_GetValue();
    
    /* Tinh dia chi cua Device */
    *pWS_Ad = (((value_SW1 & 0x01) | (value_SW2 & 0x02) | (value_SW3 & 0x03) | (value_SW4 & 0x04)) & (0xFF));
}