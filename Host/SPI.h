
//
//  Copyright (c) 2016, Stanford P. Hudson, All Rights Reserved
//

#ifndef _SPI_H_
#define _SPI_H_

#include "stm32f10x.h"

typedef enum
{
  SPI_DEVNUM_1,  // Nordic nRF24L01P
  SPI_DEVNUM_MAX
} SPIDevNum_t;

void SPIInit(SPIDevNum_t devNum);
uint8_t SPIReadWriteByte(SPIDevNum_t devNum, uint8_t byte);

#endif
