#include "stepper.h"
#include "stm32f10x_gpio.h"
#include "usart.h"
 /**************************
 * PA0:step,ת��
 * PA12��dir������
 * PA11��en��ʹ��
****************************/
 /**************************
 * ��������GPIO_Config
 * ����  ����
 * ����  ���� 
 * ���  ����
 * ����  ��������
 * ����ֵ����
****************************/
u8 flag = 0;
void Stepper_init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure; 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB , ENABLE); //ʹ��IOA
		
	    /* Timer4 Channel 1, PB6 */ 
	  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6; 
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�������
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	  GPIO_Init(GPIOB, &GPIO_InitStructure);
		 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;         //ͨ���������ģʽ
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        //ָ��GPIO���ſ���������Ƶ��Ϊ50MHZ
		GPIO_Init(GPIOA, &GPIO_InitStructure);                   
}

//================================================================================
 /**************************
 * ��������TIM2_Master__TIM3_Slave_Configuration
 * ����  �����Ӷ�ʱ������
 * ����  �����ת��PulseFrequency,�Ƕ�pulse 
 * ���  ����
 * ����  ��������
 * ����ֵ����
****************************/
void TIM4_Master_TIM3_Slave_Configuration(u32 PulseFrequency, u32 pulse) 
{
	
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//��ʱ�����ýṹ��
    TIM_OCInitTypeDef  TIM_OCInitStructure; //pwm����Ӧ���ýṹ��
		NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 | RCC_APB1Periph_TIM3, ENABLE); //����ʱ��
 
    TIM_TimeBaseStructure.TIM_Period = PulseFrequency-1;                                                   
    TIM_TimeBaseStructure.TIM_Prescaler =249;                    //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ                                                     
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //����ʱ�ӷָTDTS= Tck_tim            
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);                                       
 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;          //ѡ��ʱ��ģʽ��TIM�����ȵ���ģʽ1       
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_Pulse = PulseFrequency/2-1;                    //���ô�װ�벶��Ĵ���������ֵ                                   
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      //�������       
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);                                                         
 
    TIM_SelectMasterSlaveMode(TIM4, TIM_MasterSlaveMode_Enable);
    TIM_SelectOutputTrigger(TIM4, TIM_TRGOSource_Update);
 
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);                              
    TIM_ARRPreloadConfig(TIM4, ENABLE);
		/***��ʱ��3��ģʽ***/
    TIM_TimeBaseStructure.TIM_Period = pulse-1; //�����Զ���װ������ֵ
    TIM_TimeBaseStructure.TIM_Prescaler =0;    
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;     
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
 
    TIM_SelectInputTrigger(TIM3, TIM_TS_ITR3);
    TIM_SelectSlaveMode(TIM3,TIM_SlaveMode_External1 );// ��ͬ TIM2->SMCR|=0x07 //���ô�ģʽ�Ĵ��� 
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
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // ����жϱ�־λ 
        TIM_CtrlPWMOutputs(TIM3, DISABLE);  //�����ʹ��
        TIM_Cmd(TIM4, DISABLE); // �رն�ʱ�� 
        TIM_Cmd(TIM3, DISABLE); // �رն�ʱ�� 
        TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);         
    } 
}
//void TIM4_IRQHandler(void) 
//{ 
//	static int count = 0;
//    if (TIM_GetFlagStatus(TIM4, TIM_IT_Update) != RESET)
//    {
//			printf("count:%d,tim4:%d\r\n",count++,TIM4->ARR);
//			TIM_ClearITPendingBit(TIM4, TIM_IT_Update); // ����жϱ�־λ		
//    } 
//}
