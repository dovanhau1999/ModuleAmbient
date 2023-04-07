#include "I2C_SHT30.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/eusart.h"

#define Address_and_Write_bit      0x88
#define Address_and_Read_bit       0x89

int16_t Temperature ;
int16_t Humidity;

struct Command_SHT30
{
    uint16_t SHT30_CMD_MEASURE_H_Enable = 0x2C06;
    uint16_t SHT30_CMD_MEASURE_M_Enable = 0x2C0D;
    uint16_t SHT30_CMD_MEASURE_L_Enable = 0x2C10;
    uint16_t SHT30_CMD_MEASURE_H_Enable = 0x2400;
    uint16_t SHT30_CMD_MEASURE_M_Enable = 0x240B;
    uint16_t SHT30_CMD_MEASURE_L_Enable = 0x2416;
};

typedef struct  
{
    uint16_t Address;
    uint8_t ByteHigh;
    uint8_t ByteLow;
    uint8_t *Buff;
    uint8_t Length;
}I2C_Sequential;

/* I2C interface */
static bool I2C_MasterOpen(void);
static void I2C_MasterClose(void);
static uint8_t I2C_MasterGetRxData(void);
static void I2C_MasterSendTxData(uint8_t data);
static void I2C_MasterEnableRestart(void);
static void I2C_MasterDisableRestart(void);
static void I2C_MasterStartRx(void);
static void I2C_MasterStart(void);
static void I2C_MasterStop(void);
static void I2C_WaitACK(void);
static void I2C_MasterSendACK(void);
static void I2C_MasterSendNACK(void);
static bool I2C_MasterIsRxBufFull(void);

/* Interrupt interface */
static void I2C_MasterEnbaleIrq(void);
static bool I2C_MaterIsTrqEnable(void);
static void I2C_MasterDisableIrq(void);
static void I2C_MasterClearIrq(void);
static void I2C_MasterSetIrq(void);
static void I2C_MasterSetIrq(void);
static void I2C_MasterWaitForEvent(void);

void ReadData(void);


/* I2C Register Level interface */
static bool I2C_MasterOpen()
{
    if(!SSPCON1bits.SSPEN)
    {
        SSPSTAT = 0x00;
        SSPCON1 = 0x08;
        SSPCON2 = 0x00;
        SSPADD = 0x13;
        SSPCON1bits.SSPEN = 1;
        return true;
    }
    return false;
}

static void I2C_MasterClose()
{
    SSPCON1bits_t.SSPEN = 0;
}

static uint8_t I2C_MasterGetRxData()
{
    return SSPBUF;
}

static void I2C_MasterSendTxData(uint8_t data)
{
    SSPBUF  = data;
}

static void I2C_MasterEnableRestart(void)
{
    SSPCON2bits.RSEN = 1;
}

static void I2C_MasterDisableRestart(void)
{
    SSPCON2bits.RSEN = 0;
}

static void I2C_MasterStartRx(void)
{
    SSPCON2bits.RCEN = 1;
}

static void I2C_MasterStart(void)
{
    SSPCON2bits.SEN = 1;
}

static void I2C_MasterStop(void)
{
    SSPCON2bits.PEN = 1;
}

static void I2C_WaitACK(void)
{
    while(SSPCON2bits.ACKSTAT);
}

static void I2C_MasterSendAck(void)
{
    SSPCON2bits.ACKDT = 0;
    SSPCON2bits.ACKEN = 1;
}

static void I2C_MasterSendNack(void)
{
    SSPCON2bits.ACKDT = 1;
    SSPCON2bits.ACKEN = 1;
}

static bool I2C_MasterIsRxBufFull(void)
{
    return SSPSTATbits_t.BF;
}

static void I2C_MasterEnableIrq(void)
{
    PIE1bits.SSPIE = 1;
}

static bool I2C_MasterIsIrqEnabled(void)
{
    return PIE1bits.SSPIE;
}

static void I2C_MasterDisableIrq(void)
{
    PIE1bits.SSPIE = 0;
}

static void I2C_MasterClearIrq(void)
{
    PIR1bits.SSPIF = 0;
}

static void I2C_MasterSetIrq(void)
{
    PIR1bits.SSPIF = 1;
}

static void I2C_MasterWaitForEvent(void)
{
    while(1)
    {
        if(PIR1bits.SSPIF)
        {    
            break;
        }
    }
}

void ReadData(void)
{
    I2C_MasterStart();
    
    
}

