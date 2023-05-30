#include "MCU.h"

//bool LED_Statuc;

static void Device_Init(void);

void Task_Indicator() {
    switch (f_Indicator) {
        case OFF_Sensor:
        {
            LED_GREEN_SetHigh();
            LED_RED_SetHigh();
            break;
        }
        case ON_Sensor:
        {
            LED_GREEN_Toggle();
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

static void Device_Init(void) {
    int8_t value_SW1, value_SW2, value_SW3, value_SW4;

    value_SW1 = SW1_GetValue();
    value_SW2 = SW2_GetValue();
    value_SW3 = SW3_GetValue();
    value_SW4 = SW4_GetValue();

    /* Tinh dia chi cua Device */
    if ((value_SW1 == 1) && (value_SW2 == 0) && (value_SW3 == 0) && (value_SW4 == 0))
    {
        SW_Ad = 0x01;
    } else if ((value_SW1 == 0) && (value_SW2 == 1) && (value_SW3 == 0) && (value_SW4 == 0))
    {
        SW_Ad = 0x02;
    } else if ((value_SW1 == 0) && (value_SW2 == 0) && (value_SW3 == 1) && (value_SW4 == 0))
    {
        SW_Ad = 0x03;
    } else if ((value_SW1 == 0) && (value_SW2 == 0) && (value_SW3 == 0) && (value_SW4 == 1))
    {
        SW_Ad = 0x04;
    }
}

void App_Init(void) {
    Device_Init();
    TMR1_StartTimer();
    Tick_Init_SES();
    /* Set En_HIGH de test sensor*/
}

void App_Process(void) {
    Task_Sensor();
    Task_MB();
    Task_Indicator();
}