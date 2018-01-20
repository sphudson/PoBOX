
//
//  Copyright (c) 2016, Stanford P. Hudson, All Rights Reserved
//

#ifndef _COMMON_H_
#define _COMMON_H_

#include <msp430.h>

typedef signed char           s8_t;
typedef unsigned char 	      u8_t;
typedef short                 s16_t;
typedef unsigned short        u16_t;
typedef long                  s32_t;
typedef unsigned long         u32_t;
typedef void                  *pointer;
typedef char                  bool;

#define TRUE                  (1)
#define FALSE                 (0)
#define NULL_PTR              ((void *)0)
#define ON                    (TRUE)
#define OFF                   (FALSE)
#define KB                    (1024)
#define MB                    (1024ul*1024ul)
#define UP                    (1)
#define DOWN                  (0)

typedef enum
{
  RET_SUCCESS                  = 0x00,
  RET_FAIL                     = 0x01,
  RET_MAX
} RetCode_t;

#endif


