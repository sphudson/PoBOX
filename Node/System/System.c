
//
//  Copyright (c) 2016, Stanford P. Hudson, All Rights Reserved
//

#include "Common.h"
#include "System.h"
#include "App.h"
#include "Spi.h"
#include "NRF24L01P.h"
#include "I2c.h"

//----------------------------------------------------------------------------
//   Port Definitions:
//
//   Port   ADC   I/O   Int?   Signal
//   ====   ===   ===   ====   ================
//   P1.0   N/A    O    No     GREEN_LED
//   P1.1   N/A   N/A   Yes    RF_IRQ
//   P1.2   N/A   N/A   No     RF_CSN
//   P1.3   N/A   N/A   No     RF_CE
//   P1.4   N/A   N/A   No     N/C
//   P1.5   N/A   N/A   No     RF_SCK
//   P1.6   N/A   N/A   No     RF_MISO
//   P1.7   N/A   N/A   No     RF_MOSI
//
//   P2.6   N/A   I/O   No     I2C_SDA
//   P2.7   N/A    O    No     I2C_SCL
//
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void SystemInit()
{   
  // delay for ~100ms for Vcc to stabilize (clock is ~1MHz at power-up)
  __delay_cycles(100000);

  // setup DCO
  // make sure SYSTEM_MCLK_FREQ is correct in System.h
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;

  // setup LFXT1CLK to use external crystal
//  P2SEL |= BIT6 + BIT7;
//  BCSCTL3 = LFXT1S_0 + XCAP_3;

  _disable_interrupts();

  // initialize baseline support subsystems and device drivers
  SpiInit();
  I2cInit();

  // initialize device modules (device drivers will be initialized)
  NRF24L01PInit();
  
  // application init comes last after all peripherals/modules up
  AppInit();
  
  _enable_interrupts();
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
int _system_pre_init()
{
  WDTCTL = WDTPW + WDTHOLD;
//  P1DIR = P1SEL = P1IES = P1IE = 0;
//  P2DIR = P2SEL = P2IES = P2IE = 0;
  return 1;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void SystemReset(void)
{
  typedef void (*callFunc_t)();
  static callFunc_t resetVectorAddr;
  __disable_interrupt();
  resetVectorAddr = *(callFunc_t *)0xFFFE;
  resetVectorAddr();
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
#pragma vector = PORT1_VECTOR
#pragma vector = PORT2_VECTOR
#pragma vector = TIMERA1_VECTOR
//#pragma vector = TIMERA0_VECTOR  AppMain.c
#pragma vector = NMI_VECTOR
#pragma vector = ADC10_VECTOR
#pragma vector = WDT_VECTOR
#pragma vector = USI_VECTOR
__interrupt void DummyIntrHandler() { }
