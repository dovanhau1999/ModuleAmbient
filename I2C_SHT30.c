#include "I2C_SHT30.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/eusart.h"
#include "mcc_generated_files/i2c_master.h"
#include "mcc_generated_files/examples/i2c_master_example.h"

#define Address_and_Write_bit      0x88
#define Address_and_Read_bit       0x89

int16_t Temperature ;
int16_t Humidity;



/*
 MSB_ENABLED = 0x2C;    
 MSB_DISABLED = 0x24;
 LSB_ENABLED_HIGH = 0x06;
 LSB_EMABLED_MEDIUM = 0x0D;
 LSB_ENABLED_LOW = 0x10;
 LSB_DISABLED_HIGH = 0x00;
 LSB_DISABLED_MEDIUM = 0x0B;
 LSB_DISABLED_LOW = 0x16;
 */
uint8_t SHT30_CMD_MEASURE_H_Enable[2] = {0x2C , 0x06};
uint8_t SHT30_CMD_MEASURE_M_Enable [2] = {0x2C, 0x0D};
uint8_t SHT30_CMD_MEASURE_L_Enable [2] = {0x2C, 0x10};
uint8_t SHT30_CMD_MEASURE_H_Disable [2] = {0x24, 0x00};
uint8_t SHT30_CMD_MEASURE_M_Disable [2] = {0x24, 0x0B};
uint8_t SHT30_CMD_MEASURE_L_Disable [2] = {0x24, 0x16};


typedef struct  
{
    uint16_t Address;
    uint16_t Command;
    uint8_t *Buff;
    uint8_t Length;
}I2C_Sequential;
/*==================================================================================*/
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
static void I2C_MasterWaitACK(void);
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

static void Write_to_SHT30  (uint16_t command_SHT30);
static void Read_to_SHT30 (int16_t* TempData, int16_t* HumiData);

/*==================================================================================*/

void ReadData(void);
static uint16_t Make16bit(uint8_t H_Value, uint8_t L_value);

///* I2C Register Level interface */
//static bool I2C_MasterOpen()
//{
//    if(!SSPCON1bits.SSPEN)
//    {
//        SSPSTAT = 0x00;
//        SSPCON1 = 0x08;
//        SSPCON2 = 0x00;
//        SSPADD = 0x13;
//        SSPCON1bits.SSPEN = 1;
//        return true;
//    }
//    return false;
//}
//
//static void I2C_MasterClose()
//{
//    SSPCON1bits.SSPEN = 0;
//}
//
//static uint8_t I2C_MasterGetRxData()
//{
//    uint8_t GetData;
//    SSPCON2bits.RCEN = 1;
//    /* Wait read done */
//    while ( !I2C_MasterIsRxBufFull());
//    
//    GetData = SSPBUF;
//    I2C_MasterClearIrq();
//    return GetData;
//}
//
//static void I2C_MasterSendTxData(uint8_t data)
//{
//    SSPBUF  = data;
//    /* wait register BUF set 1 */
//    while (I2C_MasterIsRxBufFull());
//}
//
//static void I2C_MasterEnableRestart(void)
//{
//    SSPCON2bits.RSEN = 1;
//}
//
//static void I2C_MasterDisableRestart(void)
//{
//    SSPCON2bits.RSEN = 0;
//}
//
//static void I2C_MasterStartRx(void)
//{
//    SSPCON2bits.RCEN = 1;
//}
//
//static void I2C_MasterStart(void)
//{
//    SSPCON2bits.SEN = 1;
//}
//
//static void I2C_MasterStop(void)
//{
//    SSPCON2bits.PEN = 1;
//}
//
//static void I2C_MasterWaitACK(void)
//{
//    while(SSPCON2bits.ACKSTAT);
//}
//
//static void I2C_MasterSendACK(void)
//{
//    SSPCON2bits.ACKDT = 0;
//    SSPCON2bits.ACKEN = 1;
//    while (SSPCON2bits.ACKEN);
//}
//
//static void I2C_MasterSendNACK(void)
//{
//    SSPCON2bits.ACKDT = 1;
//    SSPCON2bits.ACKEN = 1;
//    while (SSPCON2bits.ACKEN);
//}
//
//static bool I2C_MasterIsRxBufFull(void)
//{
//    return SSPSTATbits.BF;
//}
//
//static void I2C_MasterEnableIrq(void)
//{
//    PIE1bits.SSPIE = 1;
//}
//
//static bool I2C_MasterIsIrqEnabled(void)
//{
//    return PIE1bits.SSPIE;
//}
//
//static void I2C_MasterDisableIrq(void)
//{
//    PIE1bits.SSPIE = 0;
//}
//
//static void I2C_MasterClearIrq(void)
//{
//    PIR1bits.SSPIF = 0;
//}
//
//static void I2C_MasterSetIrq(void)
//{
//    PIR1bits.SSPIF = 1;
//}
//
//static void I2C_MasterWaitForEvent(void)
//{
//    while(1)
//    {
//        if(PIR1bits.SSPIF)
//        {    
//            break;
//        }
//    }
//}
//
//static void Write_to_SHT30  (uint16_t command_SHT30)
//{
//    /* Start */
//    I2C_MasterStart();
//    
//    /* Send address to slave */
//    I2C_MasterSendTxData(Address_and_Write_bit);
//    /* Clear interrupt */
//    I2C_MasterClearIrq();
//    /* Wait ACK from Slave */
//    I2C_MasterWaitACK();
//    /* Clear interrupt */
//    I2C_MasterClearIrq();
//    
//    /* Send High command to slave */
//    I2C_MasterSendTxData(command_SHT30 >> 8);
//    /* Clear interrupt */
//    I2C_MasterClearIrq();
//    /* Wait ACK from Slave */
//    I2C_MasterWaitACK();
//    /* Clear interrupt */
//    I2C_MasterClearIrq();
//    
//    /* Send Low command to slave */
//    I2C_MasterSendTxData(command_SHT30 >> 8);
//    /* Clear interrupt */
//    I2C_MasterClearIrq();
//    /* Wait ACK from Slave */
//    I2C_MasterWaitACK();
//    /* Clear interrupt */
//    I2C_MasterClearIrq();
//    
//    /* Stop transmit */
//    I2C_MasterStop();
//    /* Wait interrupt */
//    I2C_MasterWaitForEvent();
//    /* Clear interrupt */
//    I2C_MasterClearIrq();
//}
//
//static void Read_to_SHT30 (int16_t* TempData, int16_t* HumiData)
//{
//    uint8_t TemperatureHi, TemperatureLo, TempeCheckSum, HumidityHi, HumidityLo, HumiCheckSum;
//    uint16_t *pTemperature, *pHumidity;
//    
//    pTemperature = (uint16_t*) TempData;
//    pHumidity = (uint16_t*) HumiData;
//    
//    /* Start */
//    I2C_MasterStart();
//    
//    /* Send address to slave */
//    I2C_MasterSendTxData(Address_and_Write_bit);
//    /* Clear interrupt */
//    I2C_MasterClearIrq();
//    /* Wait ACK from Slave */
//    I2C_MasterWaitACK();
//    /* Clear interrupt */
//    I2C_MasterClearIrq();
//    
//    /* Read data from BUF */
//    TemperatureHi = I2C_MasterGetRxData();
//    /* Send ACK to Slave */
//    I2C_MasterSendACK();
//    /* Clear Interrupt */
//    I2C_MasterClearIrq();
//    
//    /* Read data from BUF */
//    TemperatureLo = I2C_MasterGetRxData();
//    /* Send ACK to Slave */
//    I2C_MasterSendACK();
//    /* Clear Interrupt */
//    I2C_MasterClearIrq();
//    
//    /* Read data from BUF */
//    TempeCheckSum = I2C_MasterGetRxData();
//    /* Send ACK to Slave */
//    I2C_MasterSendACK();
//    /* Clear Interrupt */
//    I2C_MasterClearIrq();
//    
//    /* Read data from BUF */
//    HumidityHi = I2C_MasterGetRxData();
//    /* Send ACK to Slave */
//    I2C_MasterSendACK();
//    /* Clear Interrupt */
//    I2C_MasterClearIrq();
//    
//    /* Read data from BUF */
//    HumidityHi = I2C_MasterGetRxData();
//    /* Send ACK to Slave */
//    I2C_MasterSendACK();
//    /* Clear Interrupt */
//    I2C_MasterClearIrq();
//    
//    /* Read data from BUF */
//    HumiCheckSum = I2C_MasterGetRxData();
//    /* Send ACK to Slave */
//    I2C_MasterSendNACK();
//    /* Clear Interrupt */
//    I2C_MasterClearIrq();
//    
//    I2C_MasterStop();
//    
//    *pTemperature = (((*pTemperature) & 0xFF) | (TemperatureLo | 0xFF)) << 8;
//    *pTemperature = (*pTemperature | (TemperatureLo | 0xFF));
//    
//    *pHumidity  = (((*pHumidity) & 0xFFFF) | (HumidityHi | 0xFF)) << 8 ;
//    *pHumidity = (*pHumidity | (HumidityLo | 0xFF));
//    
//}

static uint16_t Make16bit(uint8_t H_Value, uint8_t L_value)
{
    uint16_t OutData;
    
    OutData = OutData | (H_Value & 0xFF);
    OutData = OutData << 8;
    OutData = OutData | (L_value &0xFF);
    
    return OutData;
}

void ReadData(void)
{
    uint8_t aData[6];
    uint16_t *TempData = &Temperature;
    uint16_t *HumiData = &Humidity;
    
    I2C_WriteNBytes(Address_and_Write_bit, SHT30_CMD_MEASURE_H_Disable, 2);
    I2C_ReadNBytes(Address_and_Read_bit, aData, 6);
    *TempData = Make16bit(aData[0], aData[1]);
    *HumiData = Make16bit(aData[3], aData[4]);

//    Write_to_SHT30(SHT30_CMD_MEASURE_H_Enable);
//    
//    Read_to_SHT30(&Temperature, &Humidity);
}