/* 
 * File:   MCU.h
 * Author: HAU DO
 *
 * Created on April 7, 2023, 5:31 PM
 */

#ifndef MCU_H
#define	MCU_H
#include "I2C_SHT30.h"
#include "Modbus_Slave.h"


void Device_Init(void);
void Task_MB(void);
void Task_Sensor(void);
void Task_Indicator(void);

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* MCU_H */

