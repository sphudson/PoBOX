
//
//  Copyright (c) 2016, Stanford P. Hudson, All Rights Reserved
//

#include "stm32f10x.h"
#include <string.h>
#include <ctype.h>
#include "Board.h"
#include "USART.h"
#include "FONA.h"

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void FONAInit(void)
{  
  BoardGPIOCfgPin(BOARD_FONA_KEY_GPIO_PORT, BOARD_FONA_KEY_GPIO_PIN, GPIO_Mode_Out_PP);
  BoardGPIOCfgPin(BOARD_FONA_PS_GPIO_PORT, BOARD_FONA_PS_GPIO_PIN, GPIO_Mode_IN_FLOATING);
  BoardGPIOCfgPin(BOARD_FONA_RESET_GPIO_PORT, BOARD_FONA_RESET_GPIO_PIN, GPIO_Mode_Out_PP);
  
  GPIO_SetBits(BOARD_FONA_RESET_GPIO_PORT, BOARD_FONA_RESET_GPIO_PIN);
  GPIO_SetBits(BOARD_FONA_KEY_GPIO_PORT, BOARD_FONA_KEY_GPIO_PIN);
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void FONAPowerUp(void)
{
  if (GPIO_ReadInputDataBit(BOARD_FONA_PS_GPIO_PORT, BOARD_FONA_PS_GPIO_PIN) == RESET)
  {
    GPIO_ResetBits(BOARD_FONA_KEY_GPIO_PORT, BOARD_FONA_KEY_GPIO_PIN);
    BoardDelayMsec(2000);
    GPIO_SetBits(BOARD_FONA_KEY_GPIO_PORT, BOARD_FONA_KEY_GPIO_PIN);
    while (GPIO_ReadInputDataBit(BOARD_FONA_PS_GPIO_PORT, BOARD_FONA_PS_GPIO_PIN) == RESET) { __WFI(); };
  }
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void FONAShutdown(void)
{
  if (GPIO_ReadInputDataBit(BOARD_FONA_PS_GPIO_PORT, BOARD_FONA_PS_GPIO_PIN) == SET)
  {
    GPIO_ResetBits(BOARD_FONA_KEY_GPIO_PORT, BOARD_FONA_KEY_GPIO_PIN);
    BoardDelayMsec(2000);
    GPIO_SetBits(BOARD_FONA_KEY_GPIO_PORT, BOARD_FONA_KEY_GPIO_PIN);
    while (GPIO_ReadInputDataBit(BOARD_FONA_PS_GPIO_PORT, BOARD_FONA_PS_GPIO_PIN) == SET) { __WFI(); };
  }
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
int FONAReadLineString(char *bufPtr, int maxChars)
{
  uint32_t timer = BoardGetSysTicks();
  int numChars = 0;
  char ch;

  while (1)
  {
    // max 100ms wait after last char received
    if (BoardHasExpiredMsec(&timer, 100)) { break; }

    if (USARTRxAvailable(USART_DEVNUM_3))
    {
      timer = BoardGetSysTicks();
      ch = USARTReadByte(USART_DEVNUM_3);

      if (isprint(ch))
      {
        *bufPtr++ = ch;
        numChars++;
      }
      if (ch == '\r') { break; }
      if (numChars >= maxChars) { break; }
    }
  }
  *bufPtr = 0; // NULL-terminate

  return numChars;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
int FONAWaitCommandResponse(char *cmdString, char *respString, uint32_t numMsec)
{
  char buf[64];
  uint32_t timer = BoardGetSysTicks();
  int retVal = 0;
  
  USARTFlush(USART_DEVNUM_3);
  USARTPrintString(USART_DEVNUM_3, cmdString);

  while (1)
  {
    __WFI();
    
    if (BoardHasExpiredMsec(&timer, numMsec)) { break; }
        
    if (FONAReadLineString(buf, sizeof(buf)) > 0)
    {
      if (strstr(buf, respString) != 0)
      {
        retVal = 1;
        break;
      }
    }
  }

  return retVal;
}
