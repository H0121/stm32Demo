#ifndef __LCD1602_h
#define __LCD1602_h
#include "sys.h"

#define	LCD_I2C_ADDRESS		 0x4e	//I2C��ַ����Ҫ����һλ

#define LCD1602_IIC_SCL    PBout(14) //SCL
#define LCD1602_IIC_SDA    PBout(13) //SDA	 
#define LCD1602_READ_SDA   PBin(13)  //����SDA 

#define LCD1602_SDA_IN()  {GPIOB->CRH&=0XFF0FFFFF;GPIOB->CRH|=8<<20;}
#define LCD1602_SDA_OUT() {GPIOB->CRH&=0XFF0FFFFF;GPIOB->CRH|=3<<20;}

#define LCD1602_SCL_GPIO_CLK	RCC_APB2Periph_GPIOB
#define LCD1602_SCL_PORT			GPIOB
#define LCD1602_SCL_PIN				GPIO_Pin_14

#define LCD1602_SDA_GPIO_CLK	RCC_APB2Periph_GPIOB
#define LCD1602_SDA_PORT			GPIOB
#define LCD1602_SDA_PIN				GPIO_Pin_13

//IIC���в�������
void LCD1602_IIC_Init(void);                //��ʼ��IIC��IO��				 
void LCD1602_IIC_Start(void);				//����IIC��ʼ�ź�
void LCD1602_IIC_Stop(void);	  			//����IICֹͣ�ź�
void LCD1602_IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 LCD1602_IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 LCD1602_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void LCD1602_IIC_Ack(void);					//IIC����ACK�ź�
void LCD1602_IIC_NAck(void);				//IIC������ACK�ź�
void LCD1602_IIC_Write_Data(u8 data);

void LCD1602_Write_Cmd(u8 cmd);
void LCD1602_Write_Data(u8 data);
void LCD1602_Init(void);
void LCD1602_Clear(void);
u32 lcd_pow(u8 n,u8 m);
void LCD1602_showString(u8 x,u8 y,char* str);
void LCD1602_showChar(u8 x,u8 y,char str);
void LCD1602_showNum(u8 x,u8 y,s32 num);
//void LCD1602_SetCursor(u8 x,u8 y);
#endif
