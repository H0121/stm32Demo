#ifndef __SPI_H
#define __SPI_H
#include "sys.h"


/*
PB3:SPI3_SCK
PB4:SPI3_MISO
PB5:SPI_MOSI
*/

void SPI3_Init(void);			 //��ʼ��SPI��
void SPI3_SetSpeed(u8 SpeedSet); //����SPI�ٶ�   
u8 SPI3_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�
		 
#endif

