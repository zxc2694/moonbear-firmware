/* #include "algorithm_moveAve.h" */

#ifndef __ALGORITHM_MOVEAVE_H
#define __ALGORITHM_MOVEAVE_H

#include "stm32f4xx.h"
/*=====================================================================================================*/
/*=====================================================================================================*/
int16_t MoveAve_SMA(volatile int16_t NewData, volatile int16_t *MoveAve_FIFO, u8 SampleNum);
int16_t MoveAve_WMA(volatile int16_t NewData, volatile int16_t *MoveAve_FIFO, u8 SampleNum);

/*=====================================================================================================*/
/*=====================================================================================================*/
#endif
