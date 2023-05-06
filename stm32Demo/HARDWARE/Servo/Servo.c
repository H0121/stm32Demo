
#include "Servo.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "usart.h"
#include "delay.h"

volatile u16 count = 0;
volatile int angle = 0;
volatile int flag = 0;
void Servo_init(void)
{
	TIM_TimeBaseInitTypeDef Tim_Initstru;
	GPIO_InitTypeDef GPIO_Initstru;
	TIM_OCInitTypeDef TIM_OCInitStru;
	NVIC_InitTypeDef NVIC_InitStruct;
	//����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	//GPIO����
	GPIO_Initstru.GPIO_Pin = GPIO_Pin_6;
	GPIO_Initstru.GPIO_Mode = GPIO_Mode_AF_PP;//�����������
	GPIO_Initstru.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOB,&GPIO_Initstru);
	//Tim����
	Tim_Initstru.TIM_Period = 1999; //�����Զ���װ��ֵ
	Tim_Initstru.TIM_Prescaler = 719; //����Ԥ��Ƶֵ
	Tim_Initstru.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�
	Tim_Initstru.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
	TIM_TimeBaseInit(TIM4,&Tim_Initstru);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	/*
	NVIC_InitStruct.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	*/
	TIM_OCInitStru.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStru.TIM_OutputState = TIM_OutputState_Enable;//�Ƚ����ʹ��
	TIM_OCInitStru.TIM_OCPolarity = TIM_OCPolarity_Low;//������Ե�
	TIM_OC1Init(TIM4, &TIM_OCInitStru);
	
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);//CH3 Ԥװ��ʹ��
	TIM_ARRPreloadConfig(TIM4, ENABLE);//ʹ�� TIMx �� ARR �ϵ�Ԥװ�ؼĴ���
	TIM_Cmd(TIM4, ENABLE);//��ʹ�� TIM1
}

void Servo_write(int i)
{
	TIM_SetCompare1(TIM4,1950-i-i/9);
}
