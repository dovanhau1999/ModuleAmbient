#include "Modbus_Slave.h"
#include "mcc_generated_files/pin_manager.h"

#define millis() Get_millis()


int8_t MB_UID;
int16_t MB_Register[2];

static const unsigned char fctsupported[] =
{
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
static void buildException(int8_t exception);
static int8_t ModbusRTU_Slave_Poll(uint16_t *reg, uint16_t size);
static void ModbusSlaveF04(uint16_t *reg, int8_t size);
static int8_t Modbus_getRxBuff(void);
static void Modbus_sendTxBuff(void);
static uint16_t Modbus_calcCRC(uint8_t len);
                               
                               
/*==================================================================================*/
extern volatile uint8_t eusartRxTail;
extern volatile uint8_t eusartRxHead;
static uint8_t EUSART_RxDataAvailable(void);


/*==================================================================================*/

static uint8_t validateRequest(void)
{
    /* check message CRC vs calculated CRC */
    uint16_t u16MsgCRC= 
            ((SES_Modbus.au8Buffer[SES_Modbus.u8BufferSize - 2] << 8) 
            | SES_Modbus.au8Buffer[SES_Modbus.u8BufferSize - 1]); // combine the crc Low & High bytes
    if ( Modbus_calcCRC( SES_Modbus.u8BufferSize-2 ) != u16MsgCRC )
    {
        SES_Modbus.u16errCnt ++;
        return NO_REPLY;
    }

    /* check function code */
    bool isSupported = false;
    uint8_t i=0;
    for (i = 0; i< sizeof( fctsupported ); i++)
    {
        if (fctsupported[i] == SES_Modbus.au8Buffer[FUNC])
        {
            isSupported = 1;
            break;
        }
    }
    if (!isSupported)
    {
        SES_Modbus.u16errCnt ++;
        return EXC_FUNC_CODE;
    }
    
    return 0;
}



static int8_t ModbusRTU_Slave_Poll(uint16_t *reg, uint16_t size)
{
    
    SES_Modbus.u8regsize = size;
    uint8_t u8Current;
    
    u8Current = EUSART_RxDataAvailable();
    
    if (u8Current == 0) return 0;
    
    /* Check T35 after frame end or still no frame end */
    if (u8Current != SES_Modbus.u8lastRec)
    {
        SES_Modbus.u8lastRec = u8Current;
        SES_Modbus.u32time = millis();
        return 0;
    }
    if ((unsigned long)(millis() - SES_Modbus.u32time) < (unsigned long)T35) return 0;
    
    SES_Modbus.u8lastRec = 0;
    uint8_t i8state = Modbus_getRxBuff();
    SES_Modbus.u8lastError = i8state;
    if (i8state < 7) return i8state;

    /* check slave id */
    if (SES_Modbus.au8Buffer[ ID ] != SES_Modbus.u8id) return 0;
 
    /* validate message: CRC, FCT, address and size */
    uint8_t u8exception = validateRequest();
    if (u8exception > 0)
    {
        if (u8exception != NO_REPLY)
        {
            buildException( u8exception );
            Modbus_sendTxBuff();
        }
        SES_Modbus.u8lastError = u8exception;
        return u8exception;
    }

    SES_Modbus.u32timeOut = millis();
    SES_Modbus.u8lastError = 0;
    
    ModbusSlaveF04( reg, size );
    
    return i8state;
}

static uint8_t EUSART_RxDataAvailable(void)
{
    uint8_t size;
    uint8_t snapshot_rxTail = (uint8_t) eusartRxTail;
    
    if (snapshot_rxTail < eusartRxHead)
    {
        size = ( 16 - ( eusartRxHead - snapshot_rxTail )); //64 is SIZE of BUFFER Receive ( 16 is defined in file 'eusart.h')
    }
    else
    {
        size = ( (snapshot_rxTail - eusartRxHead));
    }
    
    if (size > 0xFF)
    {
        return size;
    }
    
    return size;
}

static void Modbus_sendTxBuff(void)
{
    
}

static uint16_t Modbus_calcCRC(uint8_t len)
{
    unsigned int temp, temp2, flag;
    temp = 0xFFFF;
    unsigned char i=0;
    for (i = 0; i < len; i++)
    {
        temp = temp ^ SES_Modbus.au8Buffer[i];
        unsigned char j=0;
        for (j = 1; j <= 8; j++)
        {
            flag = temp & 0x0001;
            temp >>=1;
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

void ModbusSalve_Init(void)
{
    SES_Modbus.u8id = 1; // slave number = 1...247
    SES_Modbus.u8txenpin = 1; //Set pin EN of chip modbus
    SES_Modbus.u16timeOut = 1000;
    SES_Modbus.u32overTime = 0;
    
    if (SES_Modbus.u8txenpin == 1) EN_SetLow(); else EN_SetHigh();
    
    SES_Modbus.u8lastRec = SES_Modbus.u8BufferSize = 0;
    SES_Modbus.u16InCnt = SES_Modbus.u16OutCnt = SES_Modbus.u16errCnt = 0;
}

void ModbusSlave_Process(int16_t *reg, int8_t size)
{
    int8_t state = 0;
//    state = ModbusRTU_Slave_Poll();
    
}



