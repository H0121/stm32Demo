#include "stepper.h"
#include "stm32f10x_gpio.h"
#include "usart.h"
 /**************************
 * PA0:step,转速
 * PA12：dir，方向
 * PA11：en，使能
****************************/
 /**************************
 * 函数名：GPIO_Config
 * 描述  ：无
 * 输入  ：无 
 * 输出  ：无
 * 调用  ：主函数
 * 返回值：无
****************************/
u8 flag = 0;
void Stepper_init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure; 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB , ENABLE); //使能IOA
		
	    /* Timer4 Channel 1, PB6 */ 
	  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6; 
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用输出
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	  GPIO_Init(GPIOB, &GPIO_InitStructure);
		 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;         //通用推挽输出模式
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        //指定GPIO引脚可输出的最高频率为50MHZ
		GPIO_Init(GPIOA, &GPIO_InitStructure);                   
}

//================================================================================
 /**************************
 * 函数名：TIM2_Master__TIM3_Slave_Configuration
 * 描述  ：主从定时器配置
 * 输入  ：电机转速PulseFrequency,角度pulse 
 * 输出  ：无
 * 调用  ：主函数
 * 返回值：无
****************************/
void TIM4_Master_TIM3_Slave_Configuration(u32 PulseFrequency, u32 pulse) 
{
	
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//定时器设置结构体
    TIM_OCInitTypeDef  TIM_OCInitStructure; //pwm波对应设置结构体
		NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 | RCC_APB1Periph_TIM3, ENABLE); //开启时钟
 
    TIM_TimeBaseStructure.TIM_Period = PulseFrequency-1;                                                   
    TIM_TimeBaseStructure.TIM_Prescaler =249;                    //设置用来作为TIMx时钟频率除数的预分频值                                                     
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //设置时钟分割：TDTS= Tck_tim            
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);                                       
 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;          //选择定时器模式：TIM脉冲宽度调制模式1       
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_Pulse = PulseFrequency/2-1;                    //设置待装入捕获寄存器的脉冲值                                   
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      //输出极性       
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);                                                         
 
    TIM_SelectMasterSlaveMode(TIM4, TIM_MasterSlaveMode_Enable);
    TIM_SelectOutputTrigger(TIM4, TIM_TRGOSource_Update);
 
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);                              
    TIM_ARRPreloadConfig(TIM4, ENABLE);
		/***定时器3从模式***/
    TIM_TimeBaseStructure.TIM_Period = pulse-1; //设置自动重装载周期值
    TIM_TimeBaseStructure.TIM_Prescaler =0;    
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;     
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
 
    TIM_SelectInputTrigger(TIM3, TIM_TS_ITR3);
    TIM_SelectSlaveMode(TIM3,TIM_SlaveMode_External1 );// 等同 TIM2->SMCR|=0x07 //设置从模式寄存器 
    //   TIM2->SMCR|=0x07;            
    TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE);
 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;        
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;     
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);
		TIM_Cmd(TIM3, ENABLE);
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
		TIM_Cmd(TIM4, ENABLE);	
}

void Stepper_Ctronl(u8 dir,u8 speed,u16 angle,u8 subdivide)
{
	u32 r;
	u32 a;
	GPIO_ResetBits(GPIOA, GPIO_Pin_11);
	if(dir == 0)
		GPIO_ResetBits(GPIOA, GPIO_Pin_12);
	else if(dir == 1)
		GPIO_SetBits(GPIOA, GPIO_Pin_12);
	r = 1440.0/subdivide/speed;
	a = 200*subdivide/360.0*angle;
	TIM4_Master_TIM3_Slave_Configuration(r,a);
}

void TIM3_IRQHandler(void) 
{ 
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)     // TIM_IT_CC1
    { 
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // 清除中断标志位 
        TIM_CtrlPWMOutputs(TIM3, DISABLE);  //主输出使能
        TIM_Cmd(TIM4, DISABLE); // 关闭定时器 
        TIM_Cmd(TIM3, DISABLE); // 关闭定时器 
        TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);         
    } 
}
//void TIM4_IRQHandler(void) 
//{ 
//	static int count = 0;
//    if (TIM_GetFlagStatus(TIM4, TIM_IT_Update) != RESET)
//    {
//			printf("count:%d,tim4:%d\r\n",count++,TIM4->ARR);
//			TIM_ClearITPendingBit(TIM4, TIM_IT_Update); // 清除中断标志位		
//    } 
//}
