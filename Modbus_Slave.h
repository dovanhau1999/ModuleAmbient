/* 
 * File:   Modbus_Slave.h
 * Author: HAU DO
 *
 * Created on April 7, 2023, 5:32 PM
 */

#ifndef MODBUS_SLAVE_H
#define	MODBUS_SLAVE_H

#include <stdint.h>
#include "ModbusRTU/ModbusRTU.h"
#include "tick.h"
#include "mcc_generated_files/eusart.h"
#include <stdbool.h>
#include <stdio.h>

void ModbusSlave_Init(int8_t _SW_Ad);
void ModbusSlave_Process();

#ifdef	__cplusplus
extern "C" {
#endif

   


#ifdef	__cplusplus
}
#endif

#endif	/* MODBUS_SLAVE_H */

