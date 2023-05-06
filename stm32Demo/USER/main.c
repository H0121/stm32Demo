#include "string.h"
#include "delay.h"
#include "stdio.h"
#include "usart.h"
#include "sys.h"
#include "esp8266.h"
#include "spi.h"
#include "Keypad.h"
#include "mq2.h"
#include "oled.h"
#include "W25QXX.h"
#include "dht11.h"
#include "as608.h"
#include "usart3.h"
#include "task.h"
#include "buzzer.h"
#include "relay.h"
#include "Servo.h"
#include "led.h"
#define AS608_baund  9600//指纹模块波特率
extern u8 TempBMP[];
extern u8 HumiBMP[];
extern u8 CH4BMP[];
extern u8 pwd_Init[];
extern char data[32];
extern float MQ2_val,temp,humi;
extern int setTempVal,setHumiVal;
extern u8 setTempValChar[3],setHumiValChar[3],cmd;
extern int TH;
extern int ControlHumi,ControlTemp,ControlFan;
int main(void)
{ 
	char key;
	float MQ2MaxVal = 100;
	setTempVal = 0,setHumiVal = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();
	uart_init(9600);
	OLED_Init();
	OLED_Clear();
	Keypad_Init();
	W25QXX_Init();
	MQ2_Init();
	PS_StaGPIO_Init();
  usart3_init(AS608_baund);
	Esp8266_Init(9600);
	DHT22_Init();    //DHT22初始化
	timer5Init();		 //初始化定时器5
	relay_Init();		 //初始化继电器
	buzzerInit();		 //初始化蜂鸣器
	Servo_init();    //初始化舵机
	led_Init();			 //初始化灯
	while(PS_HandShake(&AS608Addr))//与AS608模块握手
	{
		printf("未连接上AS608!\r\n");
		delay_ms(1000);
		printf("再次尝试连接中....\r\n");
		delay_ms(1000);	  
	}
	printf("连接上AS608!\r\n");
	W25QXX_Read(pwd_Init,FLASH_SIZE-100,5);
	W25QXX_Read(setTempValChar,FLASH_SIZE-50,3);
	sscanf((char*)setTempValChar,"%d",&setTempVal);
	printf("setTempValChar:%s,setTempVal:%d\r\n",setTempValChar,setTempVal);
//	W25QXX_Read(setHumiVal,FLASH_SIZE-25,4);
	printf("pwd_Init:%s\r\n",pwd_Init);
	Servo_write(0);
  while(1)
	{
		key = Getkey();
		if(key)
		{
			delay_ms(10);
			while(Getkey() != 0) {};
			switch(key)
			{
				case '#':break;
				case '*':cmd = 0;setPwd();break;
				case 'A':cmd = 0;addPf();break;
				case 'B':cmd = 0;delPf();break;
				case 'C':cmd = 0;setTemp();break;
				case 'D':cmd = 0;setHumi();break;
				default:cmd = 1;inputPwd(key);break;
			}
		}
		if(PS_Sta)
		{
			press_FR();
		}
		OLED_DrawBMP(0,0,32,16,TempBMP);
		OLED_ShowFloat(36,0,temp,3,1,16);
		OLED_ShowNum(90,0,setTempVal,2,16);
		
		OLED_DrawBMP(0,2,32,16,HumiBMP);
		OLED_ShowFloat(36,2,humi,3,1,16);
		OLED_ShowNum(90,2,setHumiVal,2,16);
		
		OLED_DrawBMP(0,4,32,16,CH4BMP);
		OLED_ShowFloat(36,4,MQ2_val,3,1,16);
		OLED_ShowNum(90,4,MQ2MaxVal,3,16);
		
		if(TH == 1)
		{
			if(temp > (setTempVal + 2))
			{
				FanON();
				HotOff();
			}
			else if(temp < (setTempVal - 2))
			{
				FanOff();
				HotON();
			}
			if(humi > (setHumiVal + 5))
			{
				HotON();
				HumiOff();
			}
			else if(humi < (setHumiVal + 5))
			{
				HotOff();
				HumiON();
			}
		}
		else if(TH == 0)
		{
			if(ControlHumi == 0)
				HumiOff();
			if(ControlTemp == 0)
				HotOff();
			if(ControlFan == 0)
				FanOff();
		}
		if(MQ2_val > MQ2MaxVal)
		{
			buzzerOn();
		}
		else if(MQ2_val < MQ2MaxVal)
		{
			buzzerOff();
		}
		delay_ms(10);
	}
}
