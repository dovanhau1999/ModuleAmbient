/* 
 * File:   Modbus.h
 * Author: HAU DO
 *
 * Created on April 10, 2023, 3:58 PM
 */

#ifndef MODBUS_H
#define	MODBUS_H
#include "main.h"

#define MAX_BUFFER 150

enum ERR_LIST {
    ERR_NOT_MASTER = -1,
    ERR_POLLING = -2,
    ERR_BUFF_OVERFLOW = -3,
    ERR_BAD_CRC = -4,
    ERR_EXCEPTION = -5
};

enum MB_FC {
    MB_FC_NONE = 0, /*!< null operator */
    MB_FC_READ_COILS = 1, /*!< FCT=1 -> read coils or digital outputs */
    MB_FC_READ_DISCRETE_INPUT = 2, /*!< FCT=2 -> read digital inputs */
    MB_FC_READ_HOLD_REGISTERS = 3, /*!< FCT=3 -> read registers or analog outputs */
    MB_FC_READ_INPUT_REGISTER = 4, /*!< FCT=4 -> read analog inputs */
    MB_FC_WRITE_COIL = 5, /*!< FCT=5 -> write single coil or output */
    MB_FC_WRITE_REGISTER = 6, /*!< FCT=6 -> write single register */
    MB_FC_WRITE_MULTIPLE_COILS = 15, /*!< FCT=15 -> write multiple coils or outputs */
    MB_FC_WRITE_MULTIPLE_REGISTERS = 16 /*!< FCT=16 -> write multiple registers */
};

enum {
    NO_REPLY = 255,
    EXC_FUNC_CODE = 1,
    EXC_ADDR_RANGE = 2,
    EXC_REGS_QUANT = 3,
    EXC_EXECUTE = 4
};

typedef struct {
    uint8_t u8id; //!< 0=master, 1..247=slave number
    uint8_t u8txenpin; //!< flow control pin: 0=USB or RS-232 mode, >1=RS-485 mode
    uint8_t u8state;
    uint8_t u8lastError;
    uint8_t au8Buffer[MAX_BUFFER];
    uint8_t u8BufferSize;
    uint8_t u8lastRec;
    uint16_t *au16regs;
    uint16_t u16InCnt, u16OutCnt, u16errCnt;
    uint16_t u16timeOut;
    uint32_t u32time, u32timeOut, u32overTime;
    uint16_t u8regsize;

    uint16_t *HOLDING_REGS;
    uint16_t *INPUT_REGS;

} MODBUS;
#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* MODBUS_H */

