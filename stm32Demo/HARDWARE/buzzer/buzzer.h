#ifndef __buzzer_h

#define __buzzer_h
#include "sys.h"

#define buzzerOn() GPIO_SetBits(GPIOC,GPIO_Pin_5);
#define buzzerOff() GPIO_ResetBits(GPIOC,GPIO_Pin_5);

void buzzerInit(void);
#endif
