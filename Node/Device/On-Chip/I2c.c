
//
//  Copyright (c) 2016, Stanford P. Hudson, All Rights Reserved
//

#include "Common.h"
#include "System.h"
#include "I2c.h"

#define SDA_IN()    ((SYSTEM_I2C_SDA_PIN & SYSTEM_I2C_SDA_BIT) ? 1: 0)
#define SCL_IN()    ((SYSTEM_I2C_SCL_PIN & SYSTEM_I2C_SCL_BIT) ? 1: 0)
#define SDA_LOW()   (SYSTEM_I2C_SDA_PDIR |= SYSTEM_I2C_SDA_BIT)
#define SDA_HIGH()  (SYSTEM_I2C_SDA_PDIR &= ~SYSTEM_I2C_SDA_BIT)
#define SCL_LOW()   (SYSTEM_I2C_SCL_PDIR |= SYSTEM_I2C_SCL_BIT)
#define SCL_HIGH()  (SYSTEM_I2C_SCL_PDIR &= ~SYSTEM_I2C_SCL_BIT)

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void I2cInit(void)
{
  SYSTEM_I2C_SDA_PDIR &= ~SYSTEM_I2C_SDA_BIT;
  SYSTEM_I2C_SDA_PSEL &= ~SYSTEM_I2C_SDA_BIT;
  SYSTEM_I2C_SDA_POUT &= ~SYSTEM_I2C_SDA_BIT;
  SYSTEM_I2C_SCL_PSEL &= ~SYSTEM_I2C_SCL_BIT;
  SYSTEM_I2C_SCL_POUT &= ~SYSTEM_I2C_SCL_BIT;
  SYSTEM_I2C_SCL_PDIR &= ~SYSTEM_I2C_SCL_BIT;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void I2cStart(void)
{
  SDA_HIGH();
//  UtilDelayUsec(2);
  SCL_HIGH();
//  UtilDelayUsec(2);
  SDA_LOW();
//  UtilDelayUsec(2);
  SCL_LOW();
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void I2cStop(void)
{
  SCL_LOW();
//  UtilDelayUsec(2);
  SDA_LOW();
//  UtilDelayUsec(2);
  SCL_HIGH();
//  UtilDelayUsec(2);
  SDA_HIGH();
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
u8_t I2cWriteByte(u8_t byte)
{
  u8_t mask = 0x80;
  u8_t ack;

  while (mask > 0)
  {
    if (byte & mask)
    {
      SDA_HIGH();
    }
    else
    {
      SDA_LOW();
    }
    mask >>= 1;
    SCL_HIGH();
    while (SCL_IN() == 0) { };
//    UtilDelayUsec(2);
    SCL_LOW();
//    UtilDelayUsec(2);
  }

  SDA_HIGH();
  SCL_HIGH();
  ack = SDA_IN();
  if (ack == 0)
  {
    SDA_LOW();
  }
  SCL_LOW();

  return ack;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
u8_t I2cReadByte(u8_t last)
{
  u8_t byte = 0;
  u8_t i;

  SDA_HIGH();

  for (i = 0; i < 8; i++)
  {
    SCL_HIGH();
//    UtilDelayUsec(2);
    byte <<= 1;
    if (SDA_IN())
    {
      byte |= 1;
    }
    SCL_LOW();
//    UtilDelayUsec(2);
  }

  if (last == 0)
  {
    SDA_LOW();
  }

  SCL_HIGH();
//  UtilDelayUsec(2);
  SCL_LOW();
//  UtilDelayUsec(2);

  return byte;
}
