#ifndef __led_h

#define __led_h
#include "sys.h"

#define kledOn() 	GPIO_SetBits(GPIOB,GPIO_Pin_1);
#define kledOff() GPIO_ResetBits(GPIOB,GPIO_Pin_1);

#define tledOn() 	GPIO_SetBits(GPIOB,GPIO_Pin_2);
#define tledOff() GPIO_ResetBits(GPIOB,GPIO_Pin_2);

void led_Init(void);

#endif
