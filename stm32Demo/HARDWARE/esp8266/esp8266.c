#include "esp8266.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "usart.h"
#include "task.h"
u8 receiveData;
int statu = 0;
int i = 0;

void Esp8266_Init(u32 bound)
{
	GPIO_InitTypeDef gpio;
	USART_InitTypeDef uart;
	NVIC_InitTypeDef nvic_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	USART_DeInit(ESP8266_USART);
	
	gpio.GPIO_Pin = ESP8266_TXD;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ESP8266_GPIO,&gpio);
	
	gpio.GPIO_Pin = ESP8266_RXD;
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(ESP8266_GPIO,&gpio);
	
	uart.USART_BaudRate = bound;																			//波特率设置
	uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制
	uart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;									//模式
	uart.USART_Parity = USART_Parity_No;															//无奇偶校验
	uart.USART_StopBits = USART_StopBits_1;
	uart.USART_WordLength = USART_WordLength_8b;											//字长
	
	nvic_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	nvic_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	nvic_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	nvic_InitStructure.NVIC_IRQChannelSubPriority = 4;
	
	NVIC_Init(&nvic_InitStructure);
	USART_Init(ESP8266_USART,&uart);
	USART_ITConfig(ESP8266_USART, USART_IT_RXNE,ENABLE);
	USART_ITConfig(ESP8266_USART,USART_IT_ERR,ENABLE);
	USART_Cmd(ESP8266_USART,ENABLE);
}

void USART2_IRQHandler()
{
	u8 res;
	if(USART_GetITStatus(ESP8266_USART,USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(ESP8266_USART,USART_IT_RXNE);
		res = USART_ReceiveData(ESP8266_USART);
		ReceiveData(res);
	}
}

void ESP8266_SendByte(u8 data)
{
	USART_SendData(ESP8266_USART,data);
	while(USART_GetFlagStatus(ESP8266_USART,USART_FLAG_TXE) == RESET);
}

void ESP8266_SendString(char *str)
{
	while(*str != '\0')
	{
		USART_SendData(ESP8266_USART,*str++);
		while(USART_GetFlagStatus(ESP8266_USART,USART_FLAG_TC) == RESET);
	}
}

void ReceiveData(u8 data)
{
	if(data == '<')
		statu = 1;
	else if(data == '>' && statu == 1)
	{
		statu = 0;
		i = 0;
		printf("receiveData:%c\n",receiveData);
		ledControl(receiveData);
	}
	else if(statu == 1)
	{
		receiveData = data;
	}
}
