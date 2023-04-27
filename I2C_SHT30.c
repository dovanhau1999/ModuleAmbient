#include "I2C_SHT30.h"


#define Address_DFLT      0x44
#define Address_ALT       0x45
#define Timer_SCL_free             4


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

void ReadData(void) {
    uint8_t aData[6];
    static uint32_t valTime = 0;
    I2C_WriteNBytes(Address_DFLT, SHT30_CMD_MEASURE_M_Enable, 2);
    valTime = Get_millis();
    while (((uint32_t) Get_millis() - valTime) < (uint32_t) 2 * Timer_SCL_free);

    I2C_ReadNBytes(Address_DFLT, aData, 6);

//    SensorAmbient.T._Byte[0] = aData[1];
//    SensorAmbient.T._Byte[1] = aData[0];
    double ctemp = (((((aData[0] * 256.0) + aData[1]) * 175) / 65535.0) - 45) * 10;
    SensorAmbient.T.Val16 = (uint16_t) ctemp;
    
//    SensorAmbient.H._Byte[0] = aData[4];
//    SensorAmbient.H._Byte[1] = aData[3];
    double humidity = ((((aData[3] * 256.0) + aData[4]) * 100) / 65535.0) * 10;
    SensorAmbient.H.Val16 = (uint16_t) humidity;
//    printf("after2: 0x%x - 0x%x - 0x%x - 0x%x\n\r", aData[0], aData[1], aData[3], aData[4]);
}

void Task_Sensor(void) {
    static uint32_t valTime = 0;

    /* Check Timer Tick*/
    if ((llabs(((uint32_t) Get_millis() - valTime)) >= (uint32_t) 2 * TICK1S)) {
        valTime = Get_millis();
        f_Indicator = ON_Sensor;
        ReadData();
        if ((SensorAmbient.T.Val16 <= 0) || (SensorAmbient.H.Val16 == 0)) {
            SensorAmbient.T.Val16 = 0xFFFF;
            SensorAmbient.H.Val16 = 0xFFFF;
            f_Indicator = ERR_Sensor;
        }
        /* Calib gia tri temp - humi*/
//        double ctemp = (SensorAmbient.T.Val16 / 65535);
//        ctemp *= 175;
//        ctemp -= 45;
//        double humidity = (SensorAmbient.H.Val16 / 65535);
//        humidity *= 100;
//        printf("Nhiet do: %0.2f\n\r", ctemp);
//        printf("Do am: %0.2f\n\r", humidity);
//        EN_SetHigh();
//        printf("%d - %d\n\r", SensorAmbient.T.Val16, SensorAmbient.H.Val16);
    } 
}