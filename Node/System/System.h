
//
//  Copyright (c) 2016, Stanford P. Hudson, All Rights Reserved
//

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "Common.h"

#define SYSTEM_MCLK_FREQ       (1000000ul)  // SMCLK clock frequency (in Hz)
#define SYSTEM_ACLK_FREQ       (32768ul)     // ACLK clock frequency (in Hz)
#define SystemSleep()          { LPM0;      __no_operation(); }
#define SystemWakeup()         { LPM0_EXIT; __no_operation(); }
#define SystemDelayUsec(_usec) { __delay_cycles(_usec * (SYSTEM_MCLK_FREQ / 1000000ul)); }

#define SYSTEM_DISABLE_INTERRUPTS() \
    { u16_t gieState = _get_interrupt_state() & GIE; \
    _disable_interrupts();
#define SYSTEM_ENABLE_INTERRUPTS() \
    if (gieState) _enable_interrupts(); }

#define SYSTEM_GREEN_LED_ON()      SYSTEM_GREEN_LED_POUT |= SYSTEM_GREEN_LED_BIT
#define SYSTEM_GREEN_LED_OFF()     SYSTEM_GREEN_LED_POUT &= ~SYSTEM_GREEN_LED_BIT
#define SYSTEM_GREEN_LED_TOGGLE()  SYSTEM_GREEN_LED_POUT ^= SYSTEM_GREEN_LED_BIT

//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
#define SYSTEM_GREEN_LED_BIT           (BIT0)
#define SYSTEM_GREEN_LED_PIN           (P1IN)
#define SYSTEM_GREEN_LED_POUT          (P1OUT)
#define SYSTEM_GREEN_LED_PDIR          (P1DIR)
#define SYSTEM_GREEN_LED_REN           (P1REN)
#define SYSTEM_GREEN_LED_PSEL          (P1SEL)
#define SYSTEM_GREEN_LED_PIFG          (P1IFG)
#define SYSTEM_GREEN_LED_PIE           (P1IE)
#define SYSTEM_GREEN_LED_PIES          (P1IES)

#define SYSTEM_RF_IRQ_BIT              (BIT1)
#define SYSTEM_RF_IRQ_PIN              (P1IN)
#define SYSTEM_RF_IRQ_POUT             (P1OUT)
#define SYSTEM_RF_IRQ_PDIR             (P1DIR)
#define SYSTEM_RF_IRQ_REN              (P1REN)
#define SYSTEM_RF_IRQ_PSEL             (P1SEL)
#define SYSTEM_RF_IRQ_PIFG             (P1IFG)
#define SYSTEM_RF_IRQ_PIE              (P1IE)

#define SYSTEM_RF_CSN_BIT              (BIT2)
#define SYSTEM_RF_CSN_PIN              (P1IN)
#define SYSTEM_RF_CSN_POUT             (P1OUT)
#define SYSTEM_RF_CSN_PIES             (P1IES)
#define SYSTEM_RF_CSN_PDIR             (P1DIR)
#define SYSTEM_RF_CSN_PIFG             (P1IFG)
#define SYSTEM_RF_CSN_PIE              (P1IE)
#define SYSTEM_RF_CSN_REN              (P1REN)
#define SYSTEM_RF_CSN_PSEL             (P1SEL)

#define SYSTEM_RF_CE_BIT               (BIT3)
#define SYSTEM_RF_CE_PIN               (P1IN)
#define SYSTEM_RF_CE_POUT              (P1OUT)
#define SYSTEM_RF_CE_PIES              (P1IES)
#define SYSTEM_RF_CE_PDIR              (P1DIR)
#define SYSTEM_RF_CE_PIFG              (P1IFG)
#define SYSTEM_RF_CE_PIE               (P1IE)
#define SYSTEM_RF_CE_REN               (P1REN)
#define SYSTEM_RF_CE_PSEL              (P1SEL)

#define SYSTEM_P1_4_BIT                (BIT4)
#define SYSTEM_P1_4_PIN                (P1IN)
#define SYSTEM_P1_4_POUT               (P1OUT)
#define SYSTEM_P1_4_PIES               (P1IES)
#define SYSTEM_P1_4_PDIR               (P1DIR)
#define SYSTEM_P1_4_PIFG               (P1IFG)
#define SYSTEM_P1_4_PIE                (P1IE)
#define SYSTEM_P1_4_REN                (P1REN)
#define SYSTEM_P1_4_PSEL               (P1SEL)

#define SYSTEM_RF_SCK_BIT              (BIT5)
#define SYSTEM_RF_SCK_PIN              (P1IN)
#define SYSTEM_RF_SCK_POUT             (P1OUT)
#define SYSTEM_RF_SCK_PIES             (P1IES)
#define SYSTEM_RF_SCK_PDIR             (P1DIR)
#define SYSTEM_RF_SCK_PIFG             (P1IFG)
#define SYSTEM_RF_SCK_PIE              (P1IE)
#define SYSTEM_RF_SCK_REN              (P1REN)
#define SYSTEM_RF_SCK_PSEL             (P1SEL)

#define SYSTEM_RF_MISO_BIT             (BIT6)
#define SYSTEM_RF_MISO_PIN             (P1IN)
#define SYSTEM_RF_MISO_POUT            (P1OUT)
#define SYSTEM_RF_MISO_PIES            (P1IES)
#define SYSTEM_RF_MISO_PDIR            (P1DIR)
#define SYSTEM_RF_MISO_PIFG            (P1IFG)
#define SYSTEM_RF_MISO_PIE             (P1IE)
#define SYSTEM_RF_MISO_REN             (P1REN)
#define SYSTEM_RF_MISO_PSEL            (P1SEL)

#define SYSTEM_RF_MOSI_BIT             (BIT7)
#define SYSTEM_RF_MOSI_PIN             (P1IN)
#define SYSTEM_RF_MOSI_POUT            (P1OUT)
#define SYSTEM_RF_MOSI_PIES            (P1IES)
#define SYSTEM_RF_MOSI_PDIR            (P1DIR)
#define SYSTEM_RF_MOSI_PIFG            (P1IFG)
#define SYSTEM_RF_MOSI_PIE             (P1IE)
#define SYSTEM_RF_MOSI_REN             (P1REN)
#define SYSTEM_RF_MOSI_PSEL            (P1SEL)

//----------------------------------------------------------------------------
//   Port   ADC   I/O   Int?   Signal
//   ====   ===   ===   ====   ================
//   P2.6   N/A   I/O   No     I2C_SDA
//   P2.7   N/A    O    No     I2C_SCL
//----------------------------------------------------------------------------
#define SYSTEM_I2C_SDA_BIT             (BIT6)
#define SYSTEM_I2C_SDA_PIN             (P2IN)
#define SYSTEM_I2C_SDA_POUT            (P2OUT)
#define SYSTEM_I2C_SDA_PIES            (P2IES)
#define SYSTEM_I2C_SDA_PDIR            (P2DIR)
#define SYSTEM_I2C_SDA_PIFG            (P2IFG)
#define SYSTEM_I2C_SDA_PIE             (P2IE)
#define SYSTEM_I2C_SDA_REN             (P2REN)
#define SYSTEM_I2C_SDA_PSEL            (P2SEL)

#define SYSTEM_I2C_SCL_BIT             (BIT7)
#define SYSTEM_I2C_SCL_PIN             (P2IN)
#define SYSTEM_I2C_SCL_POUT            (P2OUT)
#define SYSTEM_I2C_SCL_PIES            (P2IES)
#define SYSTEM_I2C_SCL_PDIR            (P2DIR)
#define SYSTEM_I2C_SCL_PIFG            (P2IFG)
#define SYSTEM_I2C_SCL_PIE             (P2IE)
#define SYSTEM_I2C_SCL_REN             (P2REN)
#define SYSTEM_I2C_SCL_PSEL            (P2SEL)

void SystemInit(void);
void SystemReset(void);

#endif 
