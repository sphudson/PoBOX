
//
//  Copyright (c) 2016, Stanford P. Hudson, All Rights Reserved
//

#include "Common.h"
#include "System.h"
#include "Spi.h"

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void SpiInit(void)
{
  SYSTEM_RF_MISO_PSEL |= SYSTEM_RF_MISO_BIT;
  SYSTEM_RF_MOSI_PSEL |= SYSTEM_RF_MOSI_BIT;
  SYSTEM_RF_SCK_PSEL |= SYSTEM_RF_SCK_BIT;

  USICTL0 |= USIPE5 + USIPE6 + USIPE7 + USIMST + USIOE;
  USICTL1 |= USICKPH;
  USICKCTL = USISSEL_2 + USIDIV_1;
  USICTL1 &= ~USIIFG;
  USICTL0 &= ~USISWRST;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
u8_t SpiReadWriteByte(u8_t byte)
{
  USISRL = byte;
  USICNT = 8;
  while (!(USICTL1 & USIIFG)) { };
  return USISRL;
}

