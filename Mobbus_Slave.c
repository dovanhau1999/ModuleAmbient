#include <stdio.h>

#include "Modbus_Slave.h"

static void validateRequest(void);
static void buildException(int8_t exception);
static void ModbusSlaveF04(uint16_t *reg, int8_t size);
static void Modbus_getRxBuff(void);
static void Modbus_sendTxBuff(void);
static void MOdbus_calcCRC(uint8_t len);