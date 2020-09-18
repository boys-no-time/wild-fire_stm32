#ifndef __INODES_INCLUDE_H__
#define __INODES_INCLUDE_H__

/* 1. 设备header：也许它们override什么重要东西，所以要最先. */
#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "stm32f10x_conf.h" // it includes almost all driver headers

/* 2. ANSI header：这个其次，但要在所有用户header之前. */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <float.h>

#include <core_cm3.h>
#include <misc.h>

#include <rthw.h>
#include <rtthread.h>

#include "delay.h"


#include "debug.h"
#include "board.h"

#include "bsp_breathing.h"
#include "ws2812b.h"















#endif //__INODES_INCLUDE_H__

