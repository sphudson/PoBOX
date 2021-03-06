
//
//  Copyright (c) 2016, Stanford P. Hudson, All Rights Reserved
//

#include "stm32f10x.h"
#include "stm32f10x_gpio.h" 
#include "Board.h"
#include "USART.h"
#include "SPI.h"
#include "NRF24L01P.h"
#include "FONA.h"

static __IO uint32_t msTicks;
static uint32_t sysTicks = 0;

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void SysTick_Handler(void)
{ 
  // 1msec exactly
  
  sysTicks++;
    
  if (msTicks != 0)
  {
    msTicks--;
  }
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
uint32_t BoardGetSysTicks(void)
{
  return sysTicks;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void BoardResetSysTick(uint32_t msec)
{
  msTicks = msec;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
int BoardMsecElapsed(void)
{
  return (msTicks == 0);
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void BoardDelayMsec(uint32_t msec)
{
  msTicks = msec;
  while (msTicks) { __WFI(); };
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
int BoardHasExpiredMsec(uint32_t *startTime, uint32_t numMsecs)
{
  int hasExpired = 0;

  if ((sysTicks - *startTime) >= numMsecs)
  {
    hasExpired = 1;
    *startTime = sysTicks;
  }

  return hasExpired;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void BoardGPIOCfgPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIOMode_TypeDef mode)
{
  GPIO_InitTypeDef GPIO_InitStructure; 
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin; 
  GPIO_InitStructure.GPIO_Mode = mode; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOx, &GPIO_InitStructure);
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void BoardInit(void)
{      
  SystemInit();
  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock / 1000);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  BoardGPIOCfgPin(BOARD_LED1_GPIO_PORT, BOARD_LED1_GPIO_PIN, GPIO_Mode_Out_PP);
  BoardGPIOCfgPin(BOARD_LED2_GPIO_PORT, BOARD_LED2_GPIO_PIN, GPIO_Mode_Out_PP);

  USARTInit(USART_DEVNUM_3, 115200, 0);
  
  SPIInit(SPI_DEVNUM_1);
  NRF24L01PInit();
  FONAInit();
}
