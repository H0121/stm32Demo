#ifndef _MQ2_H
#define _MQ2_H
 
//C�ļ�����Ҫ��������ͷ�ļ�
#include "sys.h"
#define   MQ2_ADC			ADC1
#define		MQ2_GPIO		GPIOA
#define   MQ2_Pin			GPIO_Pin_4
#define 	MQ2_ADC_CHX	ADC_Channel_4
#define 	MQ2_READ_TIMES	10				//��������������ȡ����,����ô���,Ȼ��ȡƽ��ֵ
//C�ļ��ж���ĺ���������
void MQ2_Init(void);
float Smog_Get_Vol(void);	//��ȡMQ7�������ĵ�ѹֵ
float MQ2_PPM_Calibration(void);
float Smog_GetPPM(void);
 
#endif
