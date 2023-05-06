#ifndef __SPI_H
#define __SPI_H
#include "sys.h"


/*
PB3:SPI3_SCK
PB4:SPI3_MISO
PB5:SPI_MOSI
*/

void SPI3_Init(void);			 //初始化SPI口
void SPI3_SetSpeed(u8 SpeedSet); //设置SPI速度   
u8 SPI3_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
		 
#endif

