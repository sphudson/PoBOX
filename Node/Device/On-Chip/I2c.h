
//
//  Copyright (c) 2016, Stanford P. Hudson, All Rights Reserved
//

#ifndef _I2C_H_
#define _I2C_H_

#include "Common.h"

void I2cInit(void);
void I2cStart(void);
void I2cStop(void);
u8_t I2cWriteByte(u8_t byte);
u8_t I2cReadByte(u8_t last);

#endif


