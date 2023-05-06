#ifndef __esp8266_h

#define __esp8266_h
#include "sys.h"

#define ESP8266_USART USART2
#define ESP8266_GPIO GPIOA
#define ESP8266_TXD GPIO_Pin_2
#define ESP8266_RXD GPIO_Pin_3

void ReceiveData(u8 data);
void Esp8266_Init(u32 bound);
void ESP8266_SendByte(u8 data);
void ESP8266_SendString(char *str);
#endif
