
//
//  Copyright (c) 2016, Stanford P. Hudson, All Rights Reserved
//

#include "Common.h"
#include "NRF24L01P.h"
#include "System.h"
#include "Spi.h"

#define NRF24L01P_IRQ()           (SYSTEM_RF_IRQ_PIN & SYSTEM_RF_IRQ_BIT)
#define NRF24L01P_CE_HIGH()       (SYSTEM_RF_CE_POUT |= SYSTEM_RF_CE_BIT)
#define NRF24L01P_CE_LOW()        (SYSTEM_RF_CE_POUT &= ~SYSTEM_RF_CE_BIT)
#define NRF24L01P_CSN_ASSERT()    (SYSTEM_RF_CSN_POUT &= ~SYSTEM_RF_CSN_BIT)
#define NRF24L01P_CSN_DEASSERT()  (SYSTEM_RF_CSN_POUT |= SYSTEM_RF_CSN_BIT)

// shadow registers
static bool receiverOnOff = OFF;
static u8_t configReg;

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
u8_t NRF24L01PInit(void)
{ 
  u8_t retVal = RET_SUCCESS;
  u8_t regData;
  
  SYSTEM_RF_IRQ_PDIR &= ~SYSTEM_RF_IRQ_BIT;
  SYSTEM_RF_CE_POUT &= ~SYSTEM_RF_CE_BIT;
  SYSTEM_RF_CE_PDIR |= SYSTEM_RF_CE_BIT;
  SYSTEM_RF_CSN_POUT |= SYSTEM_RF_CSN_BIT;
  SYSTEM_RF_CSN_PDIR |= SYSTEM_RF_CSN_BIT;

  NRF24L01P_CSN_DEASSERT();
  NRF24L01P_CE_LOW();

  configReg = 0x7F;
  NRF24L01PWriteRegister(NRF24L01P_REG_CONFIG, &configReg, 1);
  
  // wait for power-up, to Standby-I mode
  SystemDelayUsec(2000ul);  // 2msec
  
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

  return retVal;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
u8_t NRF24L01PWriteCommand(u8_t cmd)
{
  u8_t retByte;
  
  NRF24L01P_CSN_ASSERT();
  retByte = SpiReadWriteByte(cmd);
  NRF24L01P_CSN_DEASSERT();
  
  return retByte;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
u8_t NRF24L01PReadRegister(NRF24L01PRegAddr_t addr, u8_t *buf, u8_t numBytes)
{
  u8_t retByte = 0;
  
  NRF24L01P_CSN_ASSERT();
  SpiReadWriteByte(NRF24L01P_READ_REG | addr);
  while (numBytes--)
  {
    retByte = SpiReadWriteByte(0);
    if (buf)
      { *buf++ = retByte; }
  }
  NRF24L01P_CSN_DEASSERT();
  
  return retByte;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NRF24L01PWriteRegister(NRF24L01PRegAddr_t addr, u8_t *buf, u8_t numBytes)
{
  NRF24L01P_CSN_ASSERT();
  SpiReadWriteByte(NRF24L01P_WRITE_REG | addr);
  while (numBytes--)
    { SpiReadWriteByte(*buf++); }
  NRF24L01P_CSN_DEASSERT();
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
u8_t NRF24L01PWriteFifo(u8_t *pPacket, u8_t numBytes)
{ 
  // write payload to FIFO
  NRF24L01P_CSN_ASSERT();
  SpiReadWriteByte(NRF24L01P_WR_TX_PLOAD);
  while (numBytes--)
    { SpiReadWriteByte(*pPacket++); }
  NRF24L01P_CSN_DEASSERT();
    
  return RET_SUCCESS;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
u8_t NRF24L01PTransmit(u8_t *pBuf, s16_t numBytes)
{
  // enter TX mode
  NRF24L01P_CE_LOW();
  configReg &= 0x7E;
  NRF24L01PWriteRegister(NRF24L01P_REG_CONFIG, &configReg, 1);
  receiverOnOff = OFF;

  NRF24L01P_CE_HIGH();
  NRF24L01PWriteFifo(pBuf, numBytes);
  NRF24L01P_CE_LOW();

  while ((NRF24L01PReadRegister(NRF24L01P_REG_FIFO_STATUS, NULL_PTR, 1) &
          NRF24L01P_FIFO_STATUS_TX_EMPTY) == 0) { };

  return RET_SUCCESS;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NRF24L01PReceiverOnOff(bool onOff)
{
  if (onOff)
  {
    // enter RX mode
    if (receiverOnOff == OFF)
    {
      configReg |= 0x01;
      NRF24L01PWriteRegister(NRF24L01P_REG_CONFIG, &configReg, 1);
      NRF24L01P_CE_HIGH();
      receiverOnOff = ON;
    }
  }
  else
  {
    if (receiverOnOff == ON)
    {
      NRF24L01P_CE_LOW();
      configReg &= 0x7E;
      NRF24L01PWriteRegister(NRF24L01P_REG_CONFIG, &configReg, 1);
      receiverOnOff = OFF;
    }
  }
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
u8_t NRF24L01PReadFifo(u8_t *pPacket, u8_t maxBytes)
{
  u8_t numBytes = 0;
  
  // read payload from FIFO
  NRF24L01P_CSN_ASSERT();
  SpiReadWriteByte(NRF24L01P_RD_RX_PLOAD);
  while (maxBytes--)
  {
    *pPacket++ = SpiReadWriteByte(0);
    numBytes++;
  }
  NRF24L01P_CSN_DEASSERT();
    
  return numBytes;
}

//----------------------------------------------------------------------------
//  NOTE: This function assumes that the chip is already in receive mode
//  PRIM_RX=1, CE=High
//----------------------------------------------------------------------------
u8_t NRF24L01PReceive(u8_t *pBuf, s16_t maxBytes)
{
  u8_t retVal = RET_FAIL;

  if ((NRF24L01PReadRegister(NRF24L01P_REG_FIFO_STATUS, NULL_PTR, 1) &
                NRF24L01P_FIFO_STATUS_RX_EMPTY) == 0)
  {
    NRF24L01PReadFifo(pBuf, maxBytes);
    retVal = RET_SUCCESS;
  }
  
  return retVal;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
u8_t NRF24L01PSetChannel(u8_t channel)
{
  // RF_CH register setup
  NRF24L01PWriteRegister(NRF24L01P_REG_RF_CH, &channel, 1);
  
  return RET_SUCCESS;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
u8_t NRF24L01PSetReceiveAddress(NRF24L01PPipeNum_t pipeNum,
    const u8_t address[NRF24L01P_ADDR_SIZE_BYTES])
{
  // RX_ADDR_Px register setup
  NRF24L01P_CSN_ASSERT();
  SpiReadWriteByte(NRF24L01P_WRITE_REG | (NRF24L01P_REG_RX_ADDR_PO + pipeNum));
  SpiReadWriteByte(address[0]);
  SpiReadWriteByte(address[1]);
  SpiReadWriteByte(address[2]);
  SpiReadWriteByte(address[3]);
  SpiReadWriteByte(address[4]);
  NRF24L01P_CSN_DEASSERT();

  return RET_SUCCESS;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
u8_t NRF24L01PSetTransmitAddress(const u8_t address[NRF24L01P_ADDR_SIZE_BYTES])
{
  // TX_ADDR register setup
  NRF24L01P_CSN_ASSERT();
  SpiReadWriteByte(NRF24L01P_WRITE_REG | NRF24L01P_REG_TX_ADDR);
  SpiReadWriteByte(address[0]);
  SpiReadWriteByte(address[1]);
  SpiReadWriteByte(address[2]);
  SpiReadWriteByte(address[3]);
  SpiReadWriteByte(address[4]);
  NRF24L01P_CSN_DEASSERT();

  return RET_SUCCESS;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NRF24L01PRadioOnOff(bool onOff)
{
  if (onOff)
    { NRF24L01P_CE_HIGH(); }
  else
    { NRF24L01P_CE_LOW(); receiverOnOff = OFF; }
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void NRF24L01PPowerUpDown(bool upDown)
{
  if (upDown) configReg |= 0x02;
  else configReg &= ~0x02;
  NRF24L01PWriteRegister(NRF24L01P_REG_CONFIG, &configReg, 1);
}


