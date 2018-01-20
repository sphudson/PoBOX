
//
//  Copyright (c) 2016, Stanford P. Hudson, All Rights Reserved
//

#include "stm32f10x.h"
#include "SPI.h"
#include "Board.h"
#include "stm32f10x_gpio.h" 
#include "stm32f10x_spi.h" 
#include "string.h"

typedef struct
{
  SPI_TypeDef               *spiDevice;
} SPIDevStruct_t;

static SPIDevStruct_t device[SPI_DEVNUM_MAX];

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
uint8_t SPIReadWriteByte(SPIDevNum_t devNum, uint8_t byte)
{
  SPI_I2S_SendData(SPI1, byte);
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) { };
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) { };
  return (SPI_I2S_ReceiveData(SPI1));
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void SPIInit(SPIDevNum_t devNum)
{
  SPI_InitTypeDef SPI_InitStructure;
  SPIDevStruct_t *devPtr;
  
  memset(&device[devNum], 0, sizeof(device[devNum]));
  
  switch (devNum)
  {
    case SPI_DEVNUM_1:
      
      devPtr = &device[SPI_DEVNUM_1];
      devPtr->spiDevice = SPI1;
      
      BoardGPIOCfgPin(BOARD_SPI1_MISO_GPIO_PORT, BOARD_SPI1_MISO_GPIO_PIN, GPIO_Mode_IN_FLOATING);
      BoardGPIOCfgPin(BOARD_SPI1_MOSI_GPIO_PORT, BOARD_SPI1_MOSI_GPIO_PIN, GPIO_Mode_AF_PP);
      BoardGPIOCfgPin(BOARD_SPI1_SCK_GPIO_PORT, BOARD_SPI1_SCK_GPIO_PIN, GPIO_Mode_AF_PP);

      SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
      SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
      SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
      SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
      SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
      SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
      SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
      SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
      SPI_InitStructure.SPI_CRCPolynomial = 0;
      SPI_Init(devPtr->spiDevice, &SPI_InitStructure);

      SPI_Cmd(devPtr->spiDevice, ENABLE);
      break;
      
    default:
      break;
  }
}
