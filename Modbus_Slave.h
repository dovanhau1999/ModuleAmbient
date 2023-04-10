/* 
 * File:   Modbus_Slave.h
 * Author: HAU DO
 *
 * Created on April 7, 2023, 5:32 PM
 */

#ifndef MODBUS_SLAVE_H
#define	MODBUS_SLAVE_H

#include <stdint.h>

void ModbusSalve_Init(void);
void ModbusSlave_Process(int16_t *reg, int8_t size);

#ifdef	__cplusplus
extern "C" {
#endif

   


#ifdef	__cplusplus
}
#endif

#endif	/* MODBUS_SLAVE_H */

