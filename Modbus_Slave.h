/* 
 * File:   Modbus_Slave.h
 * Author: HAU DO
 *
 * Created on April 7, 2023, 5:32 PM
 */

#ifndef MODBUS_SLAVE_H
#define	MODBUS_SLAVE_H

#include "tick.h"
#include "main.h"

void ModbusSlave_Init(uint8_t _SW_Ad);
uint8_t ModbusRTU_Slave_Poll(uint16_t *reg, uint16_t size);
void Task_MB(void);

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* MODBUS_SLAVE_H */

