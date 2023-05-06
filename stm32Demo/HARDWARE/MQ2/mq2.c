#include "mq2.h"
#include "sys.h"
#include "delay.h"
#include "math.h"

#define CAL_PPM  10  // У׼������PPMֵ
#define RL	     10  // RL��ֵ
#define R0	     10  // R0��ֵ

void MQ2_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1, ENABLE );	  	//ʹ��ADC1ͨ��ʱ��


    RCC_ADCCLKConfig(RCC_PCLK2_Div6);												//����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);//ADC��λ
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);//��λ����
    //PA1��2 ��Ϊģ��ͨ����������
    GPIO_InitStructure.GPIO_Pin = MQ2_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;									//ģ����������
    GPIO_Init(MQ2_GPIO, &GPIO_InitStructure);

    ADC_DeInit(MQ2_ADC);  																//��λADC2,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;								//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;									//ģ��ת�������ڵ�ͨ��ģʽ
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;								//ģ��ת�������ڵ���ת��ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;				//ת��������������ⲿ��������
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 1;											//˳����й���ת����ADCͨ������Ŀ
    ADC_Init(MQ2_ADC, &ADC_InitStructure);												//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���


    ADC_Cmd(MQ2_ADC, ENABLE);															//ʹ��ָ����ADC2

    ADC_ResetCalibration(MQ2_ADC);														//ʹ�ܸ�λУ׼

    while(ADC_GetResetCalibrationStatus(MQ2_ADC));										//�ȴ���λУ׼����

    ADC_StartCalibration(MQ2_ADC);	 													//����ADУ׼

    while(ADC_GetCalibrationStatus(MQ2_ADC));					 						//�ȴ�У׼����
}
 
uint32_t MQ2_ADC_Read(void)
{
    //����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
    ADC_RegularChannelConfig(MQ2_ADC, MQ2_ADC_CHX, 1, ADC_SampleTime_239Cycles5 );				//ADC1,ADCͨ��,����ʱ��Ϊ239.5����

    ADC_SoftwareStartConvCmd(MQ2_ADC, ENABLE);											//ʹ��ָ����ADC1�����ת����������

    while(!ADC_GetFlagStatus(MQ2_ADC, ADC_FLAG_EOC ));									//�ȴ�ת������

    return ADC_GetConversionValue(MQ2_ADC);											//�������һ��ADC1�������ת�����
}
 
//����ƽ��ֵ
u16 ADC_Average_Data(u8 ADC_Channel)
{
	u16 temp_val=0;
	u8 t;
	for(t=0;t<MQ2_READ_TIMES;t++)	//#define SMOG_READ_TIMES	10	��������������ȡ����,����ô���,Ȼ��ȡƽ��ֵ
 
	{
		temp_val+=MQ2_ADC_Read();	//��ȡADCֵ
		delay_ms(5);
	}
	temp_val/=MQ2_READ_TIMES;//�õ�ƽ��ֵ
    return (u16)temp_val;//���������ADCƽ��ֵ
}
 
//��ȡMQ7�������ĵ�ѹֵ
float Smog_Get_Vol(void)
{
	u16 adc_value = 0;//���Ǵ�MQ-7������ģ���ѹ�����ADCת���л�õ�ԭʼ����ֵ����ֵ�ķ�ΧΪ0��4095����ģ���ѹ��ʾΪ����ֵ
	float voltage = 0;//MQ-7������ģ��ĵ�ѹ�������һ����̼��Ũ�ȳ�����
	
	adc_value = ADC_Average_Data(MQ2_ADC_CHX);//#define SMOG_ADC_CHX	ADC_Channel_4	���������������ڵ�ADCͨ�����
	delay_ms(5);
	
    voltage  = (3.3/4096.0)*(adc_value);
	
	return voltage;
}

// ������У׼���������ݵ�ǰ����PPMֵ���õ�RS��ѹֵ�����Ƴ�R0ֵ��
// �ڿ��������й�����R0
//float MQ2_PPM_Calibration()
//{
//	float RS = 0;
//	float R0 = 0;
//	RS = (3.3f - Smog_Get_Vol()) / Smog_Get_Vol() * RL;//RL	10  // RL��ֵ
//	R0 = RS / pow(CAL_PPM / 613.9f, 1 / -2.074f);//CAL_PPM  10  // У׼������PPMֵ
//	return R0;
//}

 
//����Smog_ppm
float Smog_GetPPM()
{
	float RS = (3.3f - Smog_Get_Vol()) / Smog_Get_Vol() * RL;
	float ppm = 613.9f * pow(RS/R0, -2.074f);
	return  ppm;
}
