#ifndef	__KEYPAD_H
#define __KEYPAD_H

#include "stm32f10x_gpio.h"

char Getkey(void);
void Keypad_Init(void);
void Keypad_GPIO_Init(GPIO_TypeDef* GPIOx, u32 GPIO_Pin, GPIOMode_TypeDef GPIO_Mode);

#endif
