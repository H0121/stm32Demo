#ifndef __relay_h
#define __relay_h
#include "sys.h"

#define HotON()		GPIO_SetBits(GPIOC,GPIO_Pin_6);
#define HotOff()	GPIO_ResetBits(GPIOC,GPIO_Pin_6);

#define FanON()	GPIO_SetBits(GPIOC,GPIO_Pin_8 | GPIO_Pin_7);
#define FanOff()	GPIO_ResetBits(GPIOC,GPIO_Pin_8 | GPIO_Pin_7);

#define HumiON()		GPIO_SetBits(GPIOC,GPIO_Pin_9);
#define HumiOff()	GPIO_ResetBits(GPIOC,GPIO_Pin_9);

void relay_Init(void);
#endif
