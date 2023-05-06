#include "lcd1602.h"
#include "delay.h"
#include "stdlib.h"
#include "usart.h"
#include "string.h"
void LCD1602_IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(LCD1602_SCL_GPIO_CLK | LCD1602_SDA_GPIO_CLK, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = LCD1602_SCL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LCD1602_SCL_PORT, &GPIO_InitStructure);
 
	GPIO_InitStructure.GPIO_Pin = LCD1602_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LCD1602_SDA_PORT, &GPIO_InitStructure);
	LCD1602_IIC_SCL=1;
	LCD1602_IIC_SDA=1;
}

//����IIC��ʼ�ź�
void LCD1602_IIC_Start(void)
{
	LCD1602_SDA_OUT();     //sda�����
	LCD1602_IIC_SDA=1;	  	  
	LCD1602_IIC_SCL=1;
	delay_us(4);
 	LCD1602_IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	LCD1602_IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void LCD1602_IIC_Stop(void)
{
	LCD1602_SDA_OUT();//sda�����
	LCD1602_IIC_SCL=0;
	LCD1602_IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	LCD1602_IIC_SCL=1; 
	LCD1602_IIC_SDA=1;//����I2C���߽����ź�
	delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 LCD1602_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	LCD1602_SDA_IN();      //SDA����Ϊ����  
	LCD1602_IIC_SDA=1;delay_us(1);	   
	LCD1602_IIC_SCL=1;delay_us(1);	 
	while(LCD1602_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			LCD1602_IIC_Stop();
			return 1;
		}
	}
	LCD1602_IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void LCD1602_IIC_Ack(void)
{
	LCD1602_IIC_SCL=0;
	LCD1602_SDA_OUT();
	LCD1602_IIC_SDA=0;
	delay_us(2);
	LCD1602_IIC_SCL=1;
	delay_us(2);
	LCD1602_IIC_SCL=0;
}
//������ACKӦ��		    
void LCD1602_IIC_NAck(void)
{
	LCD1602_IIC_SCL=0;
	LCD1602_SDA_OUT();
	LCD1602_IIC_SDA=1;
	delay_us(2);
	LCD1602_IIC_SCL=1;
	delay_us(2);
	LCD1602_IIC_SCL=0;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void LCD1602_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	  LCD1602_SDA_OUT(); 	    
    LCD1602_IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
			LCD1602_IIC_SDA=(txd&0x80)>>7;
			txd<<=1; 	  
			delay_us(2);   
			LCD1602_IIC_SCL=1;
			delay_us(2); 
			LCD1602_IIC_SCL=0;	
			delay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 LCD1602_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	LCD1602_SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        LCD1602_IIC_SCL=0; 
        delay_us(2);
		LCD1602_IIC_SCL=1;
        receive<<=1;
        if(LCD1602_READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        LCD1602_IIC_NAck();//����nACK
    else
        LCD1602_IIC_Ack(); //����ACK   
    return receive;
}

//u8 LCD1602_Read_Data(u8 address)
//{
//	u8 dat;
//	LCD1602_IIC_Start();
//	LCD1602_IIC_Send_Byte(address+1);
//	LCD1602_IIC_Wait_Ack();
//	dat = LCD1602_IIC_Read_Byte(LCD1602_IIC_Wait_Ack());
//	LCD1602_IIC_NAck();
//	LCD1602_IIC_Stop();
//	return dat;
//}

void LCD1602_IIC_Write_Data(u8 data)
{
	LCD1602_IIC_Start();
	LCD1602_IIC_Send_Byte(LCD_I2C_ADDRESS);
	LCD1602_IIC_Wait_Ack();
	LCD1602_IIC_Send_Byte(data);
	LCD1602_IIC_Wait_Ack();
	LCD1602_IIC_Stop();
}
//дָ��
void LCD1602_Write_Cmd(u8 cmd)
{
	u8 HighByte4,LowByte4;
	HighByte4 = (cmd & 0xf0);
	LowByte4 = ((cmd << 4) & 0xf0);
	LCD1602_IIC_Write_Data(HighByte4 | 0x0c);//0000 BG=1 EN=1 RW=0 RS = 0 �򿪱����
	delay_ms(5);
	LCD1602_IIC_Write_Data(HighByte4 | 0x08);//0000 BG=1 EN=0 RW=0 RS = 0	�رձ����
	delay_ms(5);
	LCD1602_IIC_Write_Data(LowByte4 | 0x0c);//0000 BG=1 EN=1 RW=0 RS = 0 �򿪱����
	delay_us(40);
	LCD1602_IIC_Write_Data(LowByte4 | 0x08);//0000 BG=1 EN=0 RW=0 RS = 0	�رձ����
	delay_ms(5);
}
//д����
void LCD1602_Write_Data(u8 data)
{
	
	u8 HighByte4,LowByte4;
	HighByte4 = (data & 0xf0);
	LowByte4 = ((data << 4) & 0xf0);
	LCD1602_IIC_Write_Data(HighByte4 | 0x0d);//0000 BG=1 EN=1 RW=0 RS = 1 �򿪱����
	delay_ms(5);
	LCD1602_IIC_Write_Data(HighByte4 | 0x09);//0000 BG=1 EN=0 RW=0 RS = 1	�رձ����
	delay_ms(5);
	LCD1602_IIC_Write_Data(LowByte4 | 0x0d);//0000 BG=1 EN=1 RW=0 RS = 1 �򿪱����
	delay_ms(5);
	LCD1602_IIC_Write_Data(LowByte4 | 0x09);//0000 BG=1 EN=0 RW=0 RS = 0	�رձ����
	delay_ms(5);
}

void LCD1602_Init(void)
{
	LCD1602_IIC_Init();
	delay_ms(200);
	
	LCD1602_Write_Cmd(0x33);
	delay_ms(5);
	LCD1602_Write_Cmd(0x33);
	delay_ms(5);
	LCD1602_Write_Cmd(0x33);
	delay_ms(5);
	LCD1602_Write_Cmd(0x33);
	delay_ms(5);
	LCD1602_Write_Cmd(0x32);
	delay_ms(5);
	LCD1602_Write_Cmd(0x28);
	delay_ms(5);
	LCD1602_Write_Cmd(0x06);
	LCD1602_Write_Cmd(0x0c);//������ʾ���޹���Ҳ���˸
	LCD1602_Write_Cmd(0x01);
	//LCD1602_Write_Cmd(0x0f);//������ʾ���޹���Ҳ���˸
	LCD1602_Write_Cmd(0x80);
	delay_ms(5);
}

//void LCD1602_SetCursor(u8 x,u8 y)
//{
//	if(x == 0)
//		LCD1602_Write_Cmd(0x80 | y);
//	else
//		LCD1602_Write_Cmd(0xc0 | y);
//}

void LCD1602_showString(u8 x,u8 y,char* str)
{
	if(y == 0)
		LCD1602_Write_Cmd(0x80 + x);
	else if(y == 1)
		LCD1602_Write_Cmd(0xc0 + x);
	while(*str != '\0')
	{
		LCD1602_Write_Data(*str);
		str++;
	}
}

void LCD1602_showChar(u8 x,u8 y,char str)
{
	if(y == 0)
		LCD1602_Write_Cmd(0x80 + x);
	else if(y == 1)
		LCD1602_Write_Cmd(0xc0 + x);
	LCD1602_Write_Data(str);
}

void LCD1602_Clear(void)
{
	LCD1602_Write_Cmd(0x01);
	delay_ms(2);
}

void LCD1602_showNum(u8 x,u8 y,s32 num)
{
//	char s[16];
//	sprintf(s,"%d",num);
//	printf("num:%s,size:%d\r\n",s,sizeof(s));
//	LCD1602_showString(x,y,s);
	u8 t,temp,len,num1=1,flag = 0;
	if(num < 0)
	{
		flag = 1;
		num = -num;
	}
	for(len=0;len<=16;len++)
	{
		if(num > num1)
			num1 *= 10;
		else
			break;
	}
	for(t = 0;t < len;t++)
	{
		if(flag)
		{
			LCD1602_showString(x,y,"-");
			flag = 0;
			x++;
		}
		temp=(num/lcd_pow(10, len-t-1))%10;
		printf("temp:%d\r\n",temp);
		LCD1602_showChar(x,y,temp+'0');
		x++;
	}
}

u32 lcd_pow(u8 n,u8 m)
{
	u32 result = 1;
	while(m--) result *= n;
	return result;
}


















