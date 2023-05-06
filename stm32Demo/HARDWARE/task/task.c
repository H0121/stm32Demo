#include "task.h"
#include "Keypad.h"
#include "string.h"
#include "W25QXX.h"
#include "usart.h"
#include "delay.h"
#include "oled.h"
#include "as608.h"
#include "Servo.h"
#include "esp8266.h"
#include "mq2.h"
#include "dht11.h"
#include "led.h"
#include "relay.h"
#include "string.h"
extern u8 Inputpwd[];
extern u8 ripwd[];
extern  u8 qsrxpwd[];
extern u8 mmxgcgBMP[];
extern u8 pwderrorBMP[];
extern u8 TempBMP[];
extern u8 HumiBMP[];
extern u8 setBMP[];
extern u8 cgBMP[];
extern u8 qaszBMP[];
extern u8 zwBMP[];
extern u8 zcBMP[];
extern u8 qzaycBMP[];
extern u8 dbBMP[];
extern u8 twoBMP[];
extern u8 cgBMP[];
extern u8 sbBMP[];
extern u8 scBMP[];
extern u8 mbBMP[];
extern u8 lrBMP[];
extern u8 yzBMP[];
extern u8 dfBMP[];
extern u8 qkBMP[];
extern u8 qsrBMP[];
extern u8 delBMP[];
extern u8 quittBMP[];
extern u8 xgBMP[];
u8 cmd = 0;
float MQ2_val = 0;
float temp = 0,humi = 0;
char data[32];
int setTempVal,setHumiVal;
u8 setTempValChar[3] = "00",setHumiValChar[3] = "00";
u8 pwd_Init[]={"0000"};
int TH = 0;
int ControlHumi = 0,ControlTemp = 0,ControlFan = 0;

u8 inputPwd(char key)
{
	char chr = key;

	u8 len = 0;
	char IpuPwd[5];
	u8 pwd[5];
	printf("inputPwd\n");
	OLED_Clear();
	OLED_DrawBMP(16,2,96,16,Inputpwd);
	while(1)
	{
		if(chr && !Getkey())
		{
			if(chr >= '0' && chr <= '9' )
			{
				IpuPwd[len] = chr;
				printf("chr:%c,len:%d,IpuPwd:%s\n",chr,len,IpuPwd);
				OLED_ShowChar(32+len*16,4,'*',16);
				len++;
				if(len == 4)
				{
					IpuPwd[len] = '\0';
					len = 0;
					sprintf((char *)pwd,"%s",IpuPwd);
					if(strcmp((char *)pwd,(char *)pwd_Init) == 0)
					{
						printf("ok\n");
						if(cmd)
							Servo_write(90);
						delay_ms(20);
						OLED_Clear();
						OLED_DrawBMP(32,2,64,16,ripwd);
						delay_ms(1000);
						delay_ms(1000);
						delay_ms(1000);
						OLED_Clear();
						if(cmd)
							Servo_write(0);
						return 1;
					}
					else
					{
						printf("password error\n");
						OLED_Clear();
						OLED_DrawBMP(32,2,64,16,pwderrorBMP);
						delay_ms(1500);
						OLED_Clear();
						return 0;
					}
				}
			}
			else if(chr == '*')
			{
				OLED_Clear();
				OLED_DrawBMP(32,2,48,16,quittBMP);
				delay_ms(1000);
				OLED_Clear();
				return 0;
			}
		}
		chr = Getkey();
		delay_ms(10);
	}
}

void setPwd(void)
{
	char chr;
	u8 len = 0;
	u8 flag = 0;
	char IpuPwd[5];
	u8 pwd[5];
	flag = inputPwd(0);
	if(flag)
	{
		OLED_Clear();
		while(1)
		{
			chr = Getkey();
			delay_ms(10);
			OLED_DrawBMP(16,2,112,16,qsrxpwd);
			if(chr && !Getkey())
			{
				if(chr >= '0' && chr <= '9' )
				{
					IpuPwd[len] = chr;
					//OLED_ShowString(32,4,IpuPwd,16);
					OLED_ShowChar(32+16*len,4,chr,16);
					printf("chr:%c,len:%d,IpuPwd:%s\n",chr,len,IpuPwd);
					len++;
				}
				else if(chr == '*')
				{
					OLED_Clear();
					OLED_DrawBMP(16,2,48,16,quittBMP);
					OLED_DrawBMP(64,2,32,16,xgBMP);
					delay_ms(1000);
					OLED_Clear();
					return;
				}
				if(len == 4)
				{
					OLED_Clear();
					IpuPwd[len] = '\0';
					len = 0;
					sprintf((char *)pwd,"%s",IpuPwd);
					W25QXX_Write((u8*)IpuPwd,FLASH_SIZE-100,SIZE);
					W25QXX_Read(pwd_Init,FLASH_SIZE-100,SIZE);
					flag = 0;
					printf("pwd_Init:%s\r\n",pwd_Init);
					OLED_DrawBMP(16,2,96,16,mmxgcgBMP);
					delay_ms(1500);
					OLED_Clear();
					break;
				}
			}
		}
	}
}
void setTemp(void)
{
	int num = 0;
	char chr;
	printf("changeTemp");
	OLED_Clear();
	OLED_DrawBMP(0,2,32,16,setBMP);
	OLED_DrawBMP(32,2,32,16,TempBMP);
	while(1)
	{
		chr = Getkey();
		delay_ms(10);
		if(chr && !Getkey())
		{
			if((chr >= '0' && chr <= '9' ))
			{
				num *= 10;
				num += chr - '0';
				OLED_ShowNum(74,2,num,2,16);
			}
			else if(chr == '#')
			{
				OLED_Clear();
				setTempVal = num;
				sprintf((char*)setTempValChar,"%d",setTempVal);
				W25QXX_Write((u8*)setTempValChar,FLASH_SIZE-50,sizeof(setTempValChar));
				//printf("setTempValChar:%s,sizeof(setTempVal):%d\n",setTempValChar,sizeof(setTempVal));
				OLED_DrawBMP(0,2,32,16,TempBMP);
				OLED_DrawBMP(32,2,32,16,setBMP);
				OLED_DrawBMP(64,2,32,16,cgBMP);
				delay_ms(1500);
				OLED_Clear();
				break;
			}
			else if(chr == '*')
			{
				OLED_Clear();
				OLED_DrawBMP(16,2,48,16,quittBMP);
				OLED_DrawBMP(64,2,32,16,xgBMP);
				delay_ms(1000);
				OLED_Clear();
				return;
			}
		}
	}
}
void setHumi(void)
{
	int num = 0;
	char chr;
	printf("changeTemp");
	OLED_Clear();
	OLED_DrawBMP(0,2,32,16,setBMP);
	OLED_DrawBMP(32,2,32,16,HumiBMP);
	while(1)
	{
		chr = Getkey();
		delay_ms(10);
		if(chr && !Getkey())
		{
			if((chr >= '0' && chr <= '9' ))
			{
				num *= 10;
				num += chr - '0';
				OLED_ShowNum(74,2,num,2,16);
			}
			else if(chr == '#')
			{
				OLED_Clear();
				setHumiVal = num;
				sprintf((char*)setHumiValChar,"%d",setHumiVal);
				W25QXX_Write((u8*)setHumiValChar,FLASH_SIZE-25,sizeof(setHumiValChar));
				OLED_DrawBMP(0,2,32,16,HumiBMP);
				OLED_DrawBMP(32,2,32,16,setBMP);
				OLED_DrawBMP(64,2,32,16,cgBMP);
				delay_ms(1500);
				OLED_Clear();
				break;
			}
			else if(chr == '*')
			{
				OLED_Clear();
				OLED_DrawBMP(16,2,48,16,quittBMP);
				OLED_DrawBMP(64,2,32,16,xgBMP);
				delay_ms(1000);
				OLED_Clear();
				return;
			}
		}
	}
}
void addPf(void)
{
	u8 flag = 0;
	flag = inputPwd(0);
	if(flag)
		Add_FR();
}
void delPf(void)
{
	u8 flag = 0;
	flag = inputPwd(0);
	if(flag)
		Del_FR();
}

void timer5Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE); 
	
	//定时器TIM5初始化
	TIM_TimeBaseStructure.TIM_Period = 9999; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE ); //使能指定的TIM7中断,允许更新中断
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	TIM_Cmd(TIM5,ENABLE);//开启定时器5
}

void TIM5_IRQHandler()
{
	static int num=0;
	if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) 
	{
		
		if(num < 5)
			num++;
		else
			MQ2_val = Smog_GetPPM();
		DHT22_Read_Data(&temp,&humi);
		sprintf(data,"<%.2f,%.2f,%.2f>",temp,humi,MQ2_val);
		ESP8266_SendString(data);
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update );
	}
}

void ledControl(u8 k)
{
	switch(k)
	{
		case '0':kledOn();break;
		case '1':kledOff();break;
		case '2':tledOn();break;
		case '3':tledOff();break;
		case 'A':TH = 1;break;
		case 'B':TH = 0; break;
		case 'C':HumiON();ControlHumi = 1;break;
		case 'D':HumiOff();ControlHumi = 0;break;
		case 'E':HotON();ControlTemp = 1;break;
		case 'F':HotOff();ControlTemp = 0;break;
		case 'H':FanON();ControlFan = 1;break;
		case 'I':FanOff();ControlFan = 0;break;
	}
}
