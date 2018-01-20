
//
//  Copyright (c) 2016, Stanford P. Hudson, All Rights Reserved
//

#include "stm32f10x.h"
#include "NRF24L01P.h"
#include "Board.h"
#include "SPI.h"
#include "stm32f10x_spi.h"

#define NRF24L01P_CE_HIGH()       (GPIO_SetBits(BOARD_NRF24L01P_CE_GPIO_PORT, BOARD_NRF24L01P_CE_GPIO_PIN))
#define NRF24L01P_CE_LOW()        (GPIO_ResetBits(BOARD_NRF24L01P_CE_GPIO_PORT, BOARD_NRF24L01P_CE_GPIO_PIN))
#define NRF24L01P_CSN_ASSERT()    (GPIO_ResetBits(BOARD_NRF24L01P_CSN_GPIO_PORT, BOARD_NRF24L01P_CSN_GPIO_PIN))
#define NRF24L01P_CSN_DEASSERT()  (GPIO_SetBits(BOARD_NRF24L01P_CSN_GPIO_PORT, BOARD_NRF24L01P_CSN_GPIO_PIN))

// shadow registers
static int receiverOnOff = 0;
static uint8_t configReg;

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NRF24L01PInit(void)
{ 
  uint8_t regData;  
  
  BoardGPIOCfgPin(BOARD_NRF24L01P_CE_GPIO_PORT, BOARD_NRF24L01P_CE_GPIO_PIN, GPIO_Mode_Out_PP);
  BoardGPIOCfgPin(BOARD_NRF24L01P_CSN_GPIO_PORT, BOARD_NRF24L01P_CSN_GPIO_PIN, GPIO_Mode_Out_PP);

  NRF24L01P_CSN_DEASSERT();
  NRF24L01P_CE_LOW();
 
  configReg = 0x7F;
  NRF24L01PWriteRegister(NRF24L01P_REG_CONFIG, &configReg, 1);
  
  // wait for power-up, to Standby-I mode
  BoardDelayMsec(2);  // 2msec
  
  // RETR register setup
  regData = 0;
  NRF24L01PWriteRegister(NRF24L01P_REG_SETUP_RETR, &regData, 1);

  // AW register setup
  regData = 3;
  NRF24L01PWriteRegister(NRF24L01P_REG_SETUP_AW, &regData, 1);

  // RF_SETUP register setup
  regData = 0x0F;
  NRF24L01PWriteRegister(NRF24L01P_REG_RF_SETUP, &regData, 1);

  // RX_PW_P0 register setup
  regData = 32;
  NRF24L01PWriteRegister(NRF24L01P_REG_RX_PW_P0, &regData, 1);

  // RX_PW_P0 register setup
  NRF24L01PWriteRegister(NRF24L01P_REG_RX_PW_P1, &regData, 1);

  // RF_CH register setup
  regData = 20;
  NRF24L01PWriteRegister(NRF24L01P_REG_RF_CH, &regData, 1);

  // EN_AA register setup
  regData = 0;
  NRF24L01PWriteRegister(NRF24L01P_REG_EN_AA, &regData, 1);
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
uint8_t NRF24L01PWriteCommand(uint8_t cmd)
{
  uint8_t retByte;
  
  NRF24L01P_CSN_ASSERT();
  SPIReadWriteByte(SPI_DEVNUM_1, cmd);
  NRF24L01P_CSN_DEASSERT();
  
  return retByte;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
uint8_t NRF24L01PReadRegister(NRF24L01PRegAddr_t addr, uint8_t *buf, uint8_t numBytes)
{
  uint8_t retByte = 0;
  
  NRF24L01P_CSN_ASSERT();
  SPIReadWriteByte(SPI_DEVNUM_1, NRF24L01P_READ_REG | addr);
  while (numBytes--)
  {
    retByte = SPIReadWriteByte(SPI_DEVNUM_1, 0);
    if (buf)
      { *buf++ = retByte; }
  }
  
  NRF24L01P_CSN_DEASSERT();
  
  return retByte;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NRF24L01PWriteRegister(NRF24L01PRegAddr_t addr, uint8_t *buf, uint8_t numBytes)
{
  NRF24L01P_CSN_ASSERT();
  SPIReadWriteByte(SPI_DEVNUM_1, NRF24L01P_WRITE_REG | addr);
  while (numBytes--)
  { 
    SPIReadWriteByte(SPI_DEVNUM_1, *buf++);
  }  
  NRF24L01P_CSN_DEASSERT();
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NRF24L01PWriteFifo(uint8_t *pPacket, uint8_t numBytes)
{ 
  // write payload to FIFO
  NRF24L01P_CSN_ASSERT();
  SPIReadWriteByte(SPI_DEVNUM_1, NRF24L01P_WR_TX_PLOAD);
  
  while (numBytes--)
  { 
    SPIReadWriteByte(SPI_DEVNUM_1, *pPacket++);
  }
  
  NRF24L01P_CSN_DEASSERT();
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NRF24L01PTransmit(uint8_t *pBuf, int16_t numBytes)
{
  // enter TX mode
  NRF24L01P_CE_LOW();
  configReg &= 0x7E;
  NRF24L01PWriteRegister(NRF24L01P_REG_CONFIG, &configReg, 1);
  receiverOnOff = 0;

  NRF24L01P_CE_HIGH();
  NRF24L01PWriteFifo(pBuf, numBytes);
  NRF24L01P_CE_LOW();

  while ((NRF24L01PReadRegister(NRF24L01P_REG_FIFO_STATUS, 0, 1) &
          NRF24L01P_FIFO_STATUS_TX_EMPTY) == 0) { };
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NRF24L01PReceiverOnOff(int onOff)
{
  if (onOff)
  {
    // enter RX mode
    if (!receiverOnOff)
    {
      configReg |= 0x01;
      NRF24L01PWriteRegister(NRF24L01P_REG_CONFIG, &configReg, 1);
      NRF24L01P_CE_HIGH();
      receiverOnOff = 1;
    }
  }
  else
  {
    if (receiverOnOff)
    {
      NRF24L01P_CE_LOW();
      configReg &= 0x7E;
      NRF24L01PWriteRegister(NRF24L01P_REG_CONFIG, &configReg, 1);
      receiverOnOff = 0;
    }
  }
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
uint8_t NRF24L01PReadFifo(uint8_t *pPacket, uint8_t maxBytes)
{
  uint8_t numBytes = 0;
  
  // read payload from FIFO
  NRF24L01P_CSN_ASSERT();

  SPIReadWriteByte(SPI_DEVNUM_1, NRF24L01P_RD_RX_PLOAD);
 
  while (maxBytes--)
  {
    *pPacket++ = SPIReadWriteByte(SPI_DEVNUM_1, 0);
    numBytes++;
  }
  NRF24L01P_CSN_DEASSERT();
    
  return numBytes;
}

//----------------------------------------------------------------------------
//  NOTE: This function assumes that the chip is already in receive mode
//  PRIM_RX=1, CE=High
//----------------------------------------------------------------------------
uint8_t NRF24L01PReceive(uint8_t *pBuf, int16_t maxBytes)
{
  uint8_t retVal = 0;

  if ((NRF24L01PReadRegister(NRF24L01P_REG_FIFO_STATUS, 0, 1) &
                NRF24L01P_FIFO_STATUS_RX_EMPTY) == 0)
  {
    NRF24L01PReadFifo(pBuf, maxBytes);
    retVal = 1;
  }
  
  return retVal;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NRF24L01PSetChannel(uint8_t channel)
{
  // RF_CH register setup
  NRF24L01PWriteRegister(NRF24L01P_REG_RF_CH, &channel, 1);
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NRF24L01PSetReceiveAddress(NRF24L01PPipeNum_t pipeNum,
    const uint8_t address[NRF24L01P_ADDR_SIZE_BYTES])
{
  // RX_ADDR_Px register setup
  NRF24L01P_CSN_ASSERT();
  SPIReadWriteByte(SPI_DEVNUM_1, NRF24L01P_WRITE_REG | (NRF24L01P_REG_RX_ADDR_PO + pipeNum));
  SPIReadWriteByte(SPI_DEVNUM_1, address[0]);
  SPIReadWriteByte(SPI_DEVNUM_1, address[1]);
  SPIReadWriteByte(SPI_DEVNUM_1, address[2]);
  SPIReadWriteByte(SPI_DEVNUM_1, address[3]);
  SPIReadWriteByte(SPI_DEVNUM_1, address[4]);
  NRF24L01P_CSN_DEASSERT();
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NRF24L01PSetTransmitAddress(const uint8_t address[NRF24L01P_ADDR_SIZE_BYTES])
{
  // TX_ADDR register setup
  NRF24L01P_CSN_ASSERT();
  SPIReadWriteByte(SPI_DEVNUM_1, NRF24L01P_WRITE_REG | NRF24L01P_REG_TX_ADDR);
  SPIReadWriteByte(SPI_DEVNUM_1, address[0]);
  SPIReadWriteByte(SPI_DEVNUM_1, address[1]);
  SPIReadWriteByte(SPI_DEVNUM_1, address[2]);
  SPIReadWriteByte(SPI_DEVNUM_1, address[3]);
  SPIReadWriteByte(SPI_DEVNUM_1, address[4]);
  NRF24L01P_CSN_DEASSERT();
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NRF24L01PRadioOnOff(int onOff)
{
  if (onOff)
    { NRF24L01P_CE_HIGH(); }
  else
    { NRF24L01P_CE_LOW(); receiverOnOff = 0; }
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NRF24L01PPowerUpDown(int upDown)
{
  if (upDown) configReg |= 0x02;
  else configReg &= ~0x02;
  NRF24L01PWriteRegister(NRF24L01P_REG_CONFIG, &configReg, 1);
}

