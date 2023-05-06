#ifndef __LCD1602_h
#define __LCD1602_h
#include "sys.h"

#define	LCD_I2C_ADDRESS		 0x4e	//I2C地址，需要左移一位

#define LCD1602_IIC_SCL    PBout(14) //SCL
#define LCD1602_IIC_SDA    PBout(13) //SDA	 
#define LCD1602_READ_SDA   PBin(13)  //输入SDA 

#define LCD1602_SDA_IN()  {GPIOB->CRH&=0XFF0FFFFF;GPIOB->CRH|=8<<20;}
#define LCD1602_SDA_OUT() {GPIOB->CRH&=0XFF0FFFFF;GPIOB->CRH|=3<<20;}

#define LCD1602_SCL_GPIO_CLK	RCC_APB2Periph_GPIOB
#define LCD1602_SCL_PORT			GPIOB
#define LCD1602_SCL_PIN				GPIO_Pin_14

#define LCD1602_SDA_GPIO_CLK	RCC_APB2Periph_GPIOB
#define LCD1602_SDA_PORT			GPIOB
#define LCD1602_SDA_PIN				GPIO_Pin_13

//IIC所有操作函数
void LCD1602_IIC_Init(void);                //初始化IIC的IO口				 
void LCD1602_IIC_Start(void);				//发送IIC开始信号
void LCD1602_IIC_Stop(void);	  			//发送IIC停止信号
void LCD1602_IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 LCD1602_IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 LCD1602_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void LCD1602_IIC_Ack(void);					//IIC发送ACK信号
void LCD1602_IIC_NAck(void);				//IIC不发送ACK信号
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
