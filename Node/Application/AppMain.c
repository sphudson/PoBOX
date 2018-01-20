
//
//  Copyright (c) 2016, Stanford P. Hudson, All Rights Reserved
//

#include <string.h>
#include "Version.h"
#include "App.h"
#include "Common.h"
#include "System.h"
#include "NRF24L01P.h"
#include "I2c.h"

static u8_t hostAddr[NRF24L01P_ADDR_SIZE_BYTES] = {0xE7,0xE7,0xE7,0xE7,0xE7};
static u8_t nodeAddr[NRF24L01P_ADDR_SIZE_BYTES] = {0x01,0x02,0x03,0x04,0x05};
static u32_t sysTick;

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void WriteByte(u8_t regMSB, u8_t regLSB, u8_t data)
{
  I2cStart();
  I2cWriteByte(0x52);
  I2cWriteByte(regMSB);
  I2cWriteByte(regLSB);
  I2cWriteByte(data);
  I2cStop();
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
u8_t ReadByte(u8_t regMSB, u8_t regLSB)
{
  u8_t retVal;

  I2cStart();
  I2cWriteByte(0x52);
  I2cWriteByte(regMSB);
  I2cWriteByte(regLSB);
  I2cStop();
  I2cStart();
  I2cWriteByte(0x52 | 1);
  retVal = I2cReadByte(1);
  I2cStop();
  return retVal;
}

static const u8_t setup[] =
{
    0x02, 0x07, 0x01,
    0x02, 0x08, 0x01,
    0x00, 0x96, 0x00,
    0x00, 0x97, 0xfd,
    0x00, 0xe3, 0x00,
    0x00, 0xe4, 0x04,
    0x00, 0xe5, 0x02,
    0x00, 0xe6, 0x01,
    0x00, 0xe7, 0x03,
    0x00, 0xf5, 0x02,
    0x00, 0xd9, 0x05,
    0x00, 0xdb, 0xce,
    0x00, 0xdc, 0x03,
    0x00, 0xdd, 0xf8,
    0x00, 0x9f, 0x00,
    0x00, 0xa3, 0x3c,
    0x00, 0xb7, 0x00,
    0x00, 0xbb, 0x3c,
    0x00, 0xb2, 0x09,
    0x00, 0xca, 0x09,
    0x01, 0x98, 0x01,
    0x01, 0xb0, 0x17,
    0x01, 0xad, 0x00,
    0x00, 0xff, 0x05,
    0x01, 0x00, 0x05,
    0x01, 0x99, 0x05,
    0x01, 0xa6, 0x1b,
    0x01, 0xac, 0x3e,
    0x01, 0xa7, 0x1f,
    0x00, 0x30, 0x00,

    0x00, 0x11, 0x10,
    0x01, 0x0a, 0x30,
    0x00, 0x3f, 0x46,
    0x00, 0x31, 0xFF,
    0x00, 0x40, 0x63,
    0x00, 0x2e, 0x01,
    0x00, 0x1b, 0x60,  // 0x09
    0x00, 0x3e, 0x31,
    0x00, 0x14, 0x24
};

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void AppInit(void)
{
  u16_t i;
  const u8_t *setupPtr;

  // setup LED GPIOs
  SYSTEM_GREEN_LED_POUT &= ~SYSTEM_GREEN_LED_BIT;
  SYSTEM_GREEN_LED_PDIR |= SYSTEM_GREEN_LED_BIT;

  // lamp test
  //SYSTEM_GREEN_LED_ON();
  //SYSTEM_GREEN_LED_OFF();

  // setup timer
  TA0CCTL0 |= CCIE;
  TA0CTL = TASSEL_2 + ID_3 + MC_1;
  TACCR0 = 62500;  // ((1MHz / 8) * 500ms = 62500

  for (setupPtr = setup, i = 0; i < sizeof(setup); i += 3, setupPtr += 3)
    { WriteByte(*setupPtr, *(setupPtr+1), *(setupPtr+2)); }

  NRF24L01PSetTransmitAddress(hostAddr);
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
#pragma vector = TIMERA0_VECTOR
__interrupt void TIMER0A0Interrupt()
{
  sysTick++;
  SystemWakeup();
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void AppMain(void)
{
  u8_t buf[32] = { 0, 0xff };

  NRF24L01PRadioOnOff(OFF);
  NRF24L01PSetChannel(0);

  WriteByte(0x00, 0x18, 0x03);
  WriteByte(0x00, 0x15, 0x07);

  NRF24L01PSetReceiveAddress(NRF24L01P_PIPE_0, nodeAddr);
  NRF24L01PSetTransmitAddress(hostAddr);

  memset(buf, 0, sizeof(buf));
  buf[0] = 0xA5;
  while (1)
  {
    if ((sysTick % 10) == 0)
    {
      if ((ReadByte(0x00, 0x4F) & 0x07) == 0x04)
      {
        u8_t i;
        buf[1] = ReadByte(0x00, 0x62);
        WriteByte(0x00, 0x15, 0x07);
        SYSTEM_GREEN_LED_ON();
        for (i = 0; i < 32; i++)
          NRF24L01PTransmit(buf, NRF24L01P_PAYLOAD_SIZE);
        SystemDelayUsec(10000ul);
        SYSTEM_GREEN_LED_OFF();
      }
    }
    SystemSleep();
  }
}
