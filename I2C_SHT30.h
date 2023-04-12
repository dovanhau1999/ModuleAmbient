/* 
 * File:   I2C_SHT30.h
 * Author: HAU DO
 *
 * Created on April 7, 2023, 11:42 AM
 */

#ifndef I2C_SHT30_H
#define	I2C_SHT30_H

#include "tick.h"
#include "main.h"

#define TICK1S              1000

typedef union
{
    uint8_t _Byte[2];
    int16_t Val16;
}VALUE16;

typedef struct
{
    VALUE16 T;
    VALUE16 H;
}SENSOR_AMBIENT;

extern SENSOR_AMBIENT SensorAmbient;

void ReadData (void);
void Task_Sensor(void);

#ifdef	__cplusplus
extern "C" {
#endif


    


#ifdef	__cplusplus
}
#endif

#endif	/* I2C_SHT30_H */

