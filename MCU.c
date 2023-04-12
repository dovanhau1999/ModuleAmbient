#include "MCU.h"

//bool LED_Statuc;
static int8_t SW_Ad;
int8_t f_Indicator;


int16_t MB_Register[2];

void ModbusSlave_Process(void) {

    int8_t state = 0;

    MB_Register[0] = SensorAmbient.T.Val16;
    MB_Register[1] = SensorAmbient.H.Val16;
    state = ModbusRTU_Slave_Poll(MB_Register, 2);
    
    return state;
}

void Task_MB(void) {
    static bool f_Modbus_Init = false;
    if (!f_Modbus_Init) {
        ModbusSlave_Init(SW_Ad);
        f_Modbus_Init = true;
    } else {
        ModbusSlave_Process();
    }
}

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

static void Device_Init(void) {
    static int8_t value_SW1, value_SW2, value_SW3, value_SW4;

    value_SW1 = SW1_GetValue();
    value_SW2 = SW2_GetValue();
    value_SW3 = SW3_GetValue();
    value_SW4 = SW4_GetValue();

    /* Tinh dia chi cua Device */
    SW_Ad = (((value_SW1 & 0x01) | (value_SW2 & 0x02) | (value_SW3 & 0x04) | (value_SW4 & 0x08)) & (0xFF));
}

void App_Init(void) {
    Device_Init();
    TMR1_StartTimer();
    Tick_Init_SES();
}

void App_Process(void) {
    Task_Sensor();
    Task_MB();
    Task_Indicator();
}