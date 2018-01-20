
//
//  Copyright (c) 2016, Stanford P. Hudson, All Rights Reserved
//

#include "stm32f10x.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "Board.h"
#include "USART.h"
#include "Version.h"
#include "FONA.h"
#include "NRF24L01P.h"
#include "stm32f10x_usart.h" 

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void AppInit(void)
{
}

void BlinkLed(int count, int msec)
{
  while (count--)
  {
    GPIO_ResetBits(BOARD_LED1_GPIO_PORT, BOARD_LED1_GPIO_PIN);
    BoardDelayMsec(msec);
    GPIO_SetBits(BOARD_LED1_GPIO_PORT, BOARD_LED1_GPIO_PIN);
    BoardDelayMsec(msec);
  }
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void AppMain(void)
{
  uint32_t listenTimer;
  uint8_t prevStatus = 0;
  int avg = 0;
  
  uint8_t hostAddr[NRF24L01P_ADDR_SIZE_BYTES] = {0xE7,0xE7,0xE7,0xE7,0xE7};
  
  GPIO_ResetBits(BOARD_LED1_GPIO_PORT, BOARD_LED1_GPIO_PIN);
  BoardDelayMsec(50);
  GPIO_ResetBits(BOARD_LED2_GPIO_PORT, BOARD_LED2_GPIO_PIN);
  BoardDelayMsec(50);
  GPIO_SetBits(BOARD_LED1_GPIO_PORT, BOARD_LED1_GPIO_PIN);
  BoardDelayMsec(50);  
  GPIO_SetBits(BOARD_LED2_GPIO_PORT, BOARD_LED2_GPIO_PIN);
  BoardDelayMsec(50);
  
  NRF24L01PRadioOnOff(0);
  NRF24L01PSetChannel(0);
  NRF24L01PReceiverOnOff(0);
  NRF24L01PSetReceiveAddress(NRF24L01P_PIPE_0, hostAddr);
  
  FONAPowerUp();
  FONAShutdown();

  while (1)
  {
    __WFI();
    
    if (BoardHasExpiredMsec(&listenTimer, 10))
    {
      uint8_t buf[32];
      
      NRF24L01PRadioOnOff(1);
      NRF24L01PReceiverOnOff(1);
      BoardDelayMsec(1);
      NRF24L01PReceiverOnOff(0);
      NRF24L01PRadioOnOff(0);

      if (NRF24L01PReceive(buf, NRF24L01P_PAYLOAD_SIZE) == 1)
      {    
        int nodeStatus = buf[1];
        
        if (prevStatus == 0) { prevStatus = nodeStatus; avg = nodeStatus; }
        
        avg = (avg + nodeStatus) / 2;
        
        // flush FIFO
        while (NRF24L01PReceive(buf, NRF24L01P_PAYLOAD_SIZE)) { __WFI(); };
        
        BlinkLed(1, 50);
        
        if (abs(nodeStatus - prevStatus) > 8)
        {
          BlinkLed(1, 50);
          
          FONAPowerUp();
          USARTPrintString(USART_DEVNUM_3, "ATE0\r");
        
          if (FONAWaitCommandResponse("", "PB DONE", 15000))
          {                    
            BlinkLed(5, 200);
            
            if (FONAWaitCommandResponse("AT+CREG?\r", "+CREG: 0,1", 20000))
            {
              char buf[64];
              
              BlinkLed(5, 100);
              
              sprintf(buf, "AT+CMGSO=\"2144054055\",\"You have mail (%02x %02x).\"\r", prevStatus, nodeStatus);   //nodeStatus=%d\"\r", nodeStatus);
              if (FONAWaitCommandResponse(buf, "+CMGSO: ", 20000))
              { 
                prevStatus = nodeStatus;
                BlinkLed(10, 50);
              }
            }
          }
          
          FONAShutdown();
          BoardDelayMsec(30000);
        }
      }
    }
  }
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
int main(void)
{
  BoardInit();
  AppInit();
  AppMain();
  return 0;
}

