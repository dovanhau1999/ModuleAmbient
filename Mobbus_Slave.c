#include "Modbus_Slave.h"
#include "mcc_generated_files/pin_manager.h"

#define millis() Get_millis()

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))
//#define double(h,l) (l & 0xffff) | ((h & 0xffff) << 16) 


static const unsigned char fctsupported[] = {
    MB_FC_READ_COILS,
    MB_FC_READ_DISCRETE_INPUT,
    MB_FC_READ_HOLD_REGISTERS,
    MB_FC_READ_INPUT_REGISTER,
    MB_FC_WRITE_COIL,
    MB_FC_WRITE_REGISTER,
    MB_FC_WRITE_MULTIPLE_COILS,
    MB_FC_WRITE_MULTIPLE_REGISTERS
};

static MODBUS SES_Modbus;

/*                                      MODBUS                                      */
/*==================================================================================*/
static uint8_t validateRequest(void);
static void buildException(uint8_t exception);
static uint8_t ModbusSlaveF04(uint16_t *reg, uint16_t size);
static uint8_t Modbus_getRxBuff(void);
static void Modbus_sendTxBuff(void);
static uint16_t Modbus_calcCRC(uint8_t len);
static void ModbusSlave_Process(void);

/*==================================================================================*/

static uint8_t ModbusSlaveF04(uint16_t *reg, uint16_t size) {
    VALUE16 valueAdd, valueRegsno;
    valueAdd._Byte[1] = SES_Modbus.au8Buffer[ADD_HI];
    valueAdd._Byte[0] = SES_Modbus.au8Buffer[ADD_LO];
    uint16_t u8StartAdd = (uint16_t) valueAdd.Val16;

    valueRegsno._Byte[1] = SES_Modbus.au8Buffer[ NB_HI];
    valueRegsno._Byte[0] = SES_Modbus.au8Buffer[ NB_LO];
    uint8_t u8regsno = (uint8_t) valueRegsno.Val16;

    uint8_t u8CopyBufferSize;
    uint16_t i;

    SES_Modbus.au8Buffer[ 2 ] = u8regsno * 2;
    SES_Modbus.u8BufferSize = 3;

    for (i = u8StartAdd; i < u8StartAdd + u8regsno; i++) {
        SES_Modbus.au8Buffer[SES_Modbus.u8BufferSize ] = highByte(reg[i]);
        SES_Modbus.u8BufferSize++;
        SES_Modbus.au8Buffer[ SES_Modbus.u8BufferSize ] = lowByte(reg[i]);
        SES_Modbus.u8BufferSize++;
    }
    u8CopyBufferSize = SES_Modbus.u8BufferSize + 2;
    Modbus_sendTxBuff();

    return u8CopyBufferSize;
}

static void buildException(uint8_t exception) {
    /* get the original FUNC code */
    uint8_t u8func = SES_Modbus.au8Buffer[ FUNC ];

    SES_Modbus.au8Buffer[ ID ] = SES_Modbus.u8id;
    SES_Modbus.au8Buffer[ FUNC ] = u8func + 0x80;
    SES_Modbus.au8Buffer[ 2 ] = exception;
    SES_Modbus.u8BufferSize = EXCEPTION_SIZE;
}

static uint8_t Modbus_getRxBuff(void) {
    bool bBuffOverflow = false;

    if (SES_Modbus.u8txenpin > 1) {

    }

    SES_Modbus.u8BufferSize = 0;
    while (eusartRxCount) {
        SES_Modbus.au8Buffer [SES_Modbus.u8BufferSize] = EUSART_Read();
        SES_Modbus.u8BufferSize++;

        if (SES_Modbus.u8BufferSize >= MAX_BUFFER) bBuffOverflow = true;
    }
    SES_Modbus.u16InCnt++;

    if (bBuffOverflow) {
        SES_Modbus.u16errCnt++;
        return (uint8_t) ERR_BUFF_OVERFLOW;
    }

    return SES_Modbus.u8BufferSize;
}

static uint8_t validateRequest(void) {
    /* check message CRC vs calculated CRC */
    uint16_t u16MsgCRC = (uint16_t)
            ((SES_Modbus.au8Buffer[SES_Modbus.u8BufferSize - 2] << 8)
            | SES_Modbus.au8Buffer[SES_Modbus.u8BufferSize - 1]); // combine the crc Low & High bytes
    if (Modbus_calcCRC(SES_Modbus.u8BufferSize - 2) != u16MsgCRC) {
        SES_Modbus.u16errCnt++;
        return NO_REPLY;
    }

    /* check function code */
    bool isSupported = false;
    uint8_t i = 0;
    for (i = 0; i< sizeof ( fctsupported); i++) {
        if (fctsupported[i] == SES_Modbus.au8Buffer[FUNC]) {
            isSupported = 1;
            break;
        }
    }
    if (!isSupported) {
        SES_Modbus.u16errCnt++;
        return EXC_FUNC_CODE;
    }

    return 0;
}

uint8_t ModbusRTU_Slave_Poll(uint16_t *reg, uint16_t size) {

    SES_Modbus.u8regsize = size;
    uint8_t u8Current;

    u8Current = eusartRxCount;

    if (u8Current == 0) return 0;

    /* Check T35 after frame end or still no frame end */
    if (u8Current != SES_Modbus.u8lastRec) {
        SES_Modbus.u8lastRec = u8Current;
        SES_Modbus.u32time = millis();
        return 0;
    }
    if ((unsigned long) (millis() - SES_Modbus.u32time) < (unsigned long) T35) return 0;

    SES_Modbus.u8lastRec = 0;
    uint8_t i8state = Modbus_getRxBuff();
    SES_Modbus.u8lastError = i8state;
    if (i8state < 7) return i8state;

    /* check slave id */
    if (SES_Modbus.au8Buffer[ ID ] != SES_Modbus.u8id) return 0;

    /* validate message: CRC, FCT, address and size */
    uint8_t u8exception = validateRequest();
    if (u8exception > 0) {
        if (u8exception != NO_REPLY) {
            buildException(u8exception);
            Modbus_sendTxBuff();
        }
        SES_Modbus.u8lastError = u8exception;
        return u8exception;
    }

    SES_Modbus.u32timeOut = millis();
    SES_Modbus.u8lastError = 0;

    ModbusSlaveF04(reg, size);

    return (uint8_t) i8state;
}

static void Modbus_sendTxBuff(void) {
    uint16_t u16crc = Modbus_calcCRC(SES_Modbus.u8BufferSize);
    SES_Modbus.au8Buffer[ SES_Modbus.u8BufferSize ] = u16crc >> 8;
    SES_Modbus.u8BufferSize++;
    SES_Modbus.au8Buffer[ SES_Modbus.u8BufferSize ] = u16crc & 0x00ff;
    SES_Modbus.u8BufferSize++;


    if (SES_Modbus.u8txenpin > 1) {
        EN_SetHigh();
        /* set RS485 transceiver to transmit mode */
        //        printf ( EN_GetValue(), HIGH );
    }

    /* transfer buffer to serial line */
    for (int index = 0; index < SES_Modbus.u8BufferSize; index++) {
        EUSART_Write(SES_Modbus.au8Buffer[index]);
    }

    if (SES_Modbus.u8txenpin > 1) {
        /*
         must wait transmission end before changing pin state
        soft serial does not need it since it is blocking
        ...but the implementation in SoftwareSerial does nothing
        anyway, so no harm in calling it.
         */
        while (!EUSART_is_tx_done());
        volatile uint32_t u32overTimecountDown = SES_Modbus.u32overTime;

        while (u32overTimecountDown-- > 0);
        EN_SetLow();
    }

    SES_Modbus.u8BufferSize = 0;

    /* set time-out for master */
    SES_Modbus.u32timeOut = millis();

    /* increase message counter */
    SES_Modbus.u16OutCnt++;
}

static uint16_t Modbus_calcCRC(uint8_t len) {
    unsigned int temp, temp2, flag;
    temp = 0xFFFF;
    unsigned char i = 0;
    for (i = 0; i < len; i++) {
        temp = temp ^ SES_Modbus.au8Buffer[i];
        unsigned char j = 0;
        for (j = 1; j <= 8; j++) {
            flag = temp & 0x0001;
            temp >>= 1;
            if (flag)
                temp ^= 0xA001;
        }
    }
    // Reverse byte order.
    temp2 = temp >> 8;
    temp = (temp << 8) | temp2;
    temp &= 0xFFFF;
    // the returned value is already swapped
    // crcLo byte is first & crcHi byte is last
    return temp;
}

void ModbusSlave_Init(uint8_t _SW_Ad) {
    SES_Modbus.u8id = (uint8_t) _SW_Ad; // slave number = 1...247
    SES_Modbus.u8txenpin = RS485; //Set pin EN of chip modbus
    SES_Modbus.u16timeOut = 1000;
    SES_Modbus.u32overTime = 0;

    if (SES_Modbus.u8txenpin > 1) EN_SetLow();
    else EN_SetHigh();

    SES_Modbus.u8lastRec = SES_Modbus.u8BufferSize = 0;
    SES_Modbus.u16InCnt = SES_Modbus.u16OutCnt = SES_Modbus.u16errCnt = 0;
}

static void ModbusSlave_Process(void) {

    uint8_t state = 0;
    MB_Register[1] = SensorAmbient.T.Val16;
    MB_Register[0] = SensorAmbient.H.Val16;
    state = ModbusRTU_Slave_Poll(MB_Register, 2);

}

void Task_MB(void) {
    static bool f_Modbus_Init = false;
    if (!f_Modbus_Init) {
        ModbusSlave_Init(SW_Ad);
        f_Modbus_Init = true;
    } else {
        ModbusSlave_Process();
    }
    
    /* test MB */
//    EN_SetHigh();
//    printf("done MB\n\r");
}