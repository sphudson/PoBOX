
//
//  Copyright (c) 2016, Stanford P. Hudson, All Rights Reserved
//

#ifndef _NRF24L01P_H_
#define _NRF24L01P_H_

#include "Common.h"

#define NRF24L01P_PAYLOAD_SIZE        (32)
#define NRF24L01P_MAX_REGS            (0x1E)   // max number of registers
#define NRF24L01P_MAX_CHANNELS        (125)    // max # of freq channels
#define NRF24L01P_BASE_FREQ_MHZ       (2400)
#define NRF24L01P_ADDR_SIZE_BYTES     (5)

#define NRF24L01P_RF_POWER_NEG18DBM   (0)
#define NRF24L01P_RF_POWER_NEG12DBM   (1)
#define NRF24L01P_RF_POWER_NEG6DBM    (2)
#define NRF24L01P_RF_POWER_0DBM       (3)

// Interrupt flags
#define NRF24L01P_INT_IDLE            (0x00)  // Idle, no interrupt pending
#define NRF24L01P_INT_MAX_RT          (0x10)  // Max #of TX retrans interrupt
#define NRF24L01P_INT_TX_DS           (0x20)  // TX data sent interrupt
#define NRF24L01P_INT_RX_DR           (0x40)  // RX data received
#define NRF24L01P_INT_ALL             (NRF24L01P_INT_RX_DR | \
                                      NRF24L01P_INT_TX_DS | \
                                      NRF24L01P_INT_MAX_RT) // All flags

// SPI commands
#define NRF24L01P_READ_REG            (0x00)  // Read from register
#define NRF24L01P_WRITE_REG           (0x20)  // Write to register
#define NRF24L01P_RD_RX_PLOAD         (0x61)  // RX payload register address
#define NRF24L01P_WR_TX_PLOAD         (0xA0)  // TX payload register address
#define NRF24L01P_FLUSH_TX            (0xE1)  // Flush TX register
#define NRF24L01P_FLUSH_RX            (0xE2)  // Flush RX register
#define NRF24L01P_REUSE_TX_PL         (0xE3)  // Reuse TX payload register
#define NRF24L01P_ACTIVATE            (0x50)  // Activate feature command
#define NRF24L01P_R_RX_PL_WID         (0x60)  // Read RX payload width
#define NRF24L01P_W_ACK_PAYLOAD       (0xA8)  // Write payload with ACK packet
#define NRF24L01P_W_TX_PAYLOAD_NOACK  (0xB0)  // Disable ACK with this packet
#define NRF24L01P_NOP                 (0xFF)  // No operation

// pipe enumerations
typedef enum
{
  NRF24L01P_PIPE_0,
  NRF24L01P_PIPE_1,
  NRF24L01P_PIPE_2,
  NRF24L01P_PIPE_3,
  NRF24L01P_PIPE_4,
  NRF24L01P_PIPE_5
} NRF24L01PPipeNum_t;

// register address enumerations
typedef enum
{
  NRF24L01P_REG_CONFIG,
  NRF24L01P_REG_EN_AA,
  NRF24L01P_REG_EN_RXADDR,
  NRF24L01P_REG_SETUP_AW,
  NRF24L01P_REG_SETUP_RETR,
  NRF24L01P_REG_RF_CH,
  NRF24L01P_REG_RF_SETUP,
  NRF24L01P_REG_STATUS,
  NRF24L01P_REG_OBSERVE_TX,
  NRF24L01P_REG_RPD,
  NRF24L01P_REG_RX_ADDR_PO,
  NRF24L01P_REG_RX_ADDR_P1,
  NRF24L01P_REG_RX_ADDR_P2,
  NRF24L01P_REG_RX_ADDR_P3,
  NRF24L01P_REG_RX_ADDR_P4,
  NRF24L01P_REG_RX_ADDR_P5,
  NRF24L01P_REG_TX_ADDR,
  NRF24L01P_REG_RX_PW_P0,
  NRF24L01P_REG_RX_PW_P1,
  NRF24L01P_REG_RX_PW_P2,
  NRF24L01P_REG_RX_PW_P3,
  NRF24L01P_REG_RX_PW_P4,
  NRF24L01P_REG_RX_PW_P5,
  NRF24L01P_REG_FIFO_STATUS,
  NRF24L01P_REG_ACK_PLD,
  NRF24L01P_REG_TX_PLD,
  NRF24L01P_REG_RX_PLD,
  NRF24L01P_REG_RESERVED,
  NRF24L01P_REG_DYNPD,
  NRF24L01P_REG_FEATURE
} NRF24L01PRegAddr_t;

// registers
typedef union
{
  struct
  {
    u8_t PRIM_RX     : 1;  // RX/TX control:  0=PTX, 1=PRX
    u8_t PWR_UP      : 1;  // 0=Power down, 1=Power up
    u8_t CRCO        : 1;  // CRC encoding: 0=8-bit, 1=16-bit
    u8_t EN_CRC      : 1;  // Enable CRC
    u8_t MASK_MAX_RT : 1;  // MAX_RT IRQ: 0=active, 1=off
    u8_t MASK_TX_DS  : 1;  // TX_DS IRQ: 0=active, 1=off
    u8_t MASK_RX_DR  : 1;  // RX_DR IRQ: 0=active, 1=off
    u8_t reserved    : 1;
  } bits;
  u8_t byte;
} NRF24L01PRegConfig_t;

typedef union
{
  struct
  {
    u8_t ENAA_P0     : 1;  // Enable auto-ack data pipe 0
    u8_t ENAA_P1     : 1;  // Enable auto-ack data pipe 1
    u8_t ENAA_P2     : 1;  // Enable auto-ack data pipe 2
    u8_t ENAA_P3     : 1;  // Enable auto-ack data pipe 3
    u8_t ENAA_P4     : 1;  // Enable auto-ack data pipe 4
    u8_t ENAA_P5     : 1;  // Enable auto-ack data pipe 5
    u8_t reserved    : 2;
  } bits;
  u8_t byte;
} NRF24L01PRegEnAA_t;

typedef union
{
  struct
  {
    u8_t ERX_P0      : 1;  // Enable data pipe 0
    u8_t ERX_P1      : 1;  // Enable data pipe 1
    u8_t ERX_P2      : 1;  // Enable data pipe 2
    u8_t ERX_P3      : 1;  // Enable data pipe 3
    u8_t ERX_P4      : 1;  // Enable data pipe 4
    u8_t ERX_P5      : 1;  // Enable data pipe 5
    u8_t reserved    : 2;
  } bits;
  u8_t byte;
} NRF24L01PRegEnRxAddr_t;

typedef union
{
  struct
  {
    u8_t AW          : 2;  // RX/TX addr bytes: 0=illegal, 1=3, 2=4, 3=5
    u8_t reserved    : 6;
  } bits;
  u8_t byte;
} NRF24L01PRegSetupAw_t;

typedef union
{
  struct
  {
    u8_t ARC    : 4;  // Auto retransmit count: 0=disabled, 1=1, ..., 15=15
    u8_t ARD    : 4;  // Auto retransmit delay: 0=250uS, 1=500uS, ..., 15=4ms
  } bits;
  u8_t byte;
} NRF24L01PRegSetupRetR_t;

typedef union
{
  struct
  {
    u8_t RF_CH       : 7;  // Frequency channel
    u8_t reserved    : 1;
  } bits;
  u8_t byte;
} NRF24L01PRegRfCh_t;

typedef union
{
  struct
  {
    u8_t reserved1       : 1;
    u8_t RF_PWR          : 2;  // Output power (dBm): 0=-18, 1=-12, 2=-6, 3=0
    u8_t RF_DR_HIGH      : 1;  // Air data rate
    u8_t PLL_LOCK        : 1;  // Force PLL lock signal:  set to zero
    u8_t RF_DR_LOW       : 1;  // Air data rate
    u8_t reserved2       : 1;
    u8_t CONT_WAVE       : 1;  // Continuous carrier transmit
  } bits;
  u8_t byte;
} NRF24L01PRegRfSetup_t;

typedef union
{
  struct
  {
    u8_t TX_FULL      : 1;  // TX FIFO full flag
    u8_t RX_P_NO      : 3;  // Data pipe # for RX_FIFO reading
    u8_t MAX_RT       : 1;  // Max # TX retransmits int flag
    u8_t TX_DS        : 1;  // Data sent int flag
    u8_t RX_DR        : 1;  // Data received int flag
    u8_t reserved     : 1;
  } bits;
  u8_t byte;
} NRF24L01PRegStatus_t;

typedef union
{
  struct
  {
    u8_t ARC_CNT      : 4;  // Count of lost packets
    u8_t PLOS_CNT     : 4;  // Count of retransmitted packets
  } bits;
  u8_t byte;
} NRF24L01PRegObserveTx_t;

typedef union
{
  struct
  {
    u8_t RPD          : 1;  // Receive power detector
    u8_t reserved     : 7;  
  } bits;
  u8_t byte;
} NRF24L01PRegRpd_t;

typedef union
{
  struct
  {
    u8_t RX_PW_P0     : 6;  // Num bytes in data pipe 0
    u8_t reserved     : 2;
  } bits;
  u8_t byte;
} NRF24L01PRegRxPwP0_t;

typedef union
{
  struct
  {
    u8_t RX_PW_P1     : 6;  // Num bytes in data pipe 1
    u8_t reserved     : 2;
  } bits;
  u8_t byte;
} NRF24L01PRegRxPwP1_t;

typedef union
{
  struct
  {
    u8_t RX_PW_P2     : 6;  // Num bytes in data pipe 2
    u8_t reserved     : 2;
  } bits;
  u8_t byte;
} NRF24L01PRegRxPwP2_t;

typedef union
{
  struct
  {
    u8_t RX_PW_P3     : 6;  // Num bytes in data pipe 3
    u8_t reserved     : 2;
  } bits;
  u8_t byte;
} NRF24L01PRegRxPwP3_t;

typedef union
{
  struct
  {
    u8_t RX_PW_P4     : 6;  // Num bytes in data pipe 4
    u8_t reserved     : 2;
  } bits;
  u8_t byte;
} NRF24L01PRegRxPwP4_t;

typedef union
{
  struct
  {
    u8_t RX_PW_P5     : 6;  // Num bytes in data pipe 5
    u8_t reserved     : 2;
  } bits;
  u8_t byte;
} NRF24L01PRegRxPwP5_t;

#define NRF24L01P_FIFO_STATUS_RX_EMPTY     (0x01)
#define NRF24L01P_FIFO_STATUS_RX_FULL      (0x02)
#define NRF24L01P_FIFO_STATUS_TX_EMPTY     (0x10)
#define NRF24L01P_FIFO_STATUS_TX_FULL      (0x20)
#define NRF24L01P_FIFO_STATUS_TX_REUSE     (0x40)

typedef union
{
  struct
  {
    u8_t RX_EMPTY     : 1;  // RX FIFO empty
    u8_t RX_FULL      : 1;  // RX FIFO full
    u8_t reserved1    : 2;
    u8_t TX_EMPTY     : 1;  // TX FIFO empty
    u8_t TX_FULL      : 1;  // TX FIFO full
    u8_t TX_REUSE     : 1;  // Reuse last transmitted packet
    u8_t reserved2    : 1;
  } bits;
  u8_t byte;
} NRF24L01PRegFifoStatus_t;

typedef union
{
  struct
  {
    u8_t DPL_P0       : 1;  // Enable dynamic payload data pipe 0
    u8_t DPL_P1       : 1;  // Enable dynamic payload data pipe 1
    u8_t DPL_P2       : 1;  // Enable dynamic payload data pipe 2
    u8_t DPL_P3       : 1;  // Enable dynamic payload data pipe 3
    u8_t DPL_P4       : 1;  // Enable dynamic payload data pipe 4
    u8_t DPL_P5       : 1;  // Enable dynamic payload data pipe 5
    u8_t reserved1    : 2;
  } bits;
  u8_t byte;
} NRF24L01PRegDynPd_t;

typedef union
{
  struct
  {
    u8_t EN_DYN_ACK     : 1;  // Enable W_TX_PAYLOAD_NOACK command
    u8_t EN_ACK_PAY     : 1;  // Enable payload with ACK
    u8_t EN_DPL         : 1;  // Enable dynamic payload length
    u8_t reserved1      : 5;
  } bits;
  u8_t byte;
} NRF24L01PRegFeature_t;

typedef union
{
  u8_t regArray[NRF24L01P_MAX_REGS];
  struct
  {
    NRF24L01PRegConfig_t      regConfig;
    NRF24L01PRegEnAA_t        regEnAA;
    NRF24L01PRegEnRxAddr_t    regEnRxAddr;
    NRF24L01PRegSetupAw_t     regSetupAw;
    NRF24L01PRegSetupRetR_t   regSetupRetR;
    NRF24L01PRegRfCh_t        regRfCh;
    NRF24L01PRegRfSetup_t     regRfSetup;
    NRF24L01PRegStatus_t      regStatus;
    NRF24L01PRegObserveTx_t   regObserveTx;
    NRF24L01PRegRpd_t         regRpd;
    NRF24L01PRegRxPwP0_t      regPwP0;
    NRF24L01PRegRxPwP1_t      regPwP1;
    NRF24L01PRegRxPwP2_t      regPwP2;
    NRF24L01PRegRxPwP3_t      regPwP3;
    NRF24L01PRegRxPwP4_t      regPwP4;
    NRF24L01PRegRxPwP5_t      regPwP5;
    NRF24L01PRegFifoStatus_t  regFifoStatus;
    u8_t                      regAckPld;
    u8_t                      regTxPld;
    u8_t                      regRxPld;
    u8_t                      reserved;
    NRF24L01PRegDynPd_t       regDynPd;
    NRF24L01PRegFeature_t     regFeature;
  } Regs;
} NRF24L01PRegs_t;

u8_t NRF24L01PInit(void);
u8_t NRF24L01PReadRegister(NRF24L01PRegAddr_t addr, u8_t *buf, u8_t numBytes);
void NRF24L01PWriteRegister(NRF24L01PRegAddr_t addr, u8_t *buf, u8_t numBytes);
u8_t NRF24L01PTransmit(u8_t *pBuf, s16_t numBytes);
u8_t NRF24L01PWriteFifo(u8_t *pPacket, u8_t numBytes);
u8_t NRF24L01PReceive(u8_t *pBuf, s16_t maxBytes);
u8_t NRF24L01PReadFifo(u8_t *pPacket, u8_t maxBytes);
u8_t NRF24L01PSetChannel(u8_t channel);
void NRF24L01PIntrHandler(void);
bool NRF24L01PIsTxFifoFull(void);
bool NRF24L01PIsRxFifoEmpty(void);
void NRF24L01PRadioOnOff(bool onOff);
void NRF24L01PReceiverOnOff(bool onOff);
u8_t NRF24L01PSetTransmitAddress(const u8_t address[NRF24L01P_ADDR_SIZE_BYTES]);
u8_t NRF24L01PSetReceiveAddress(NRF24L01PPipeNum_t pipeNum, const u8_t address[NRF24L01P_ADDR_SIZE_BYTES]);
u8_t NRF24L01PWriteCommand(u8_t cmd);
void NRF24L01PPowerUpDown(bool upDown);


#endif


