#include "I2C_SHT30.h"


#define Address_and_Write_bit      0x88
#define Address_and_Read_bit       0x89
#define Timer_SCL_free             1

SENSOR_AMBIENT SensorAmbient;

/* Note:
 MSB_ENABLED = 0x2C;    
 MSB_DISABLED = 0x24;
 LSB_ENABLED_HIGH = 0x06;
 LSB_EMABLED_MEDIUM = 0x0D;
 LSB_ENABLED_LOW = 0x10;
 LSB_DISABLED_HIGH = 0x00;
 LSB_DISABLED_MEDIUM = 0x0B;
 LSB_DISABLED_LOW = 0x16;
 */
uint8_t SHT30_CMD_MEASURE_H_Enable[2] = {0x2C, 0x06};
uint8_t SHT30_CMD_MEASURE_M_Enable [2] = {0x2C, 0x0D};
uint8_t SHT30_CMD_MEASURE_L_Enable [2] = {0x2C, 0x10};
uint8_t SHT30_CMD_MEASURE_H_Disable [2] = {0x24, 0x00};
uint8_t SHT30_CMD_MEASURE_M_Disable [2] = {0x24, 0x0B};
uint8_t SHT30_CMD_MEASURE_L_Disable [2] = {0x24, 0x16};

//extern int8_t f_Indicator;

void ReadData(void) {
    uint8_t aData[6];
    uint32_t valTime;

    I2C_WriteNBytes(Address_and_Write_bit, SHT30_CMD_MEASURE_H_Disable, 2);
    valTime = Get_millis();
    while (((uint32_t) Get_millis() - valTime) < (uint32_t) 2 * Timer_SCL_free);

    I2C_ReadNBytes(Address_and_Read_bit, aData, 6);

    SensorAmbient.T._Byte[1] = aData[0];
    SensorAmbient.T._Byte[0] = aData[1];

    SensorAmbient.H._Byte[1] = aData[3];
    SensorAmbient.H._Byte[0] = aData[4];
}

void Task_Sensor(void) {
    static uint32_t valTime = 0;

//    f_Indicator = OFF_Sensor;

    /* Check Timer Tick*/
    if ((((uint32_t) Get_millis() - valTime) >= (uint32_t) 2 * TICK1S)) {
        valTime = Get_millis();
//        f_Indicator = ON_Sensor;
        ReadData();
        if ((SensorAmbient.T.Val16 < 0) || (SensorAmbient.H.Val16 < 0)) {
            SensorAmbient.T.Val16 = 0x8000;
            SensorAmbient.H.Val16 = 0x8000;
//            f_Indicator = ERR_Sensor;
        }
    }
}