
//
//  Copyright (c) 2016, Stanford P. Hudson, All Rights Reserved
//

#ifndef _FONA_H_
#define _FONA_H_

#include "stm32f10x.h"

void FONAInit(void);
void FONAPowerUp(void);
void FONAShutdown(void);
int FONAWaitCommandResponse(char *cmdString, char *respString, uint32_t numMsec);

#endif
