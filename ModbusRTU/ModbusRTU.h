/* 
 * File:   ModbusRTU.h
 * Author: HAU DO
 *
 * Created on April 10, 2023, 6:20 PM
 */

#ifndef MODBUSRTU_H
#define	MODBUSRTU_H

#include "../Modbus.h"

#define T35  3



enum
{
    RESPONSE_SIZE = 6,
    EXCEPTION_SIZE = 3,
    CHECKSUM_SIZE = 2
};    

/**
 * @enum MESSAGE
 * @brief
 * Indexes to telegram frame positions
 */
enum MESSAGE_MODBUS_RTU
{
    ID                             = 0, //!< ID field
    FUNC, //!< Function code position
    ADD_HI, //!< Address high byte
    ADD_LO, //!< Address low byte
    NB_HI, //!< Number of coils or registers high byte
    NB_LO, //!< Number of coils or registers low byte
    BYTE_CNT  //!< byte counter
};



enum TYPE_CONNECTION
{
    USB = 0,
    RS232 = 1,
    RS485 = 2
 };

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* MODBUSRTU_H */

