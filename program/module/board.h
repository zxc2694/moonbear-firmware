#ifndef __MODULE_BOARD_S_H
#define __MODULE_BOARD_S_H

#include "stm32f4xx.h"

#define ENABLE 1
#define DISABLE 0

#define KEY   PBI(2)
#define LED_R 15
#define LED_G 14
#define LED_B 13

void LED_Config(void);

void SetLED(int LED, int state);
void LED_Toggle(int LED);

#endif
