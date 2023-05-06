#include "mq2.h"
#include "sys.h"
#include "delay.h"
#include "math.h"

#define CAL_PPM  10  // 校准环境中PPM值
#define RL	     10  // RL阻值
#define R0	     10  // R0阻值

void MQ2_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1, ENABLE );	  	//使能ADC1通道时钟


    RCC_ADCCLKConfig(RCC_PCLK2_Div6);												//设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);//ADC复位
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);//复位结束
    //PA1、2 作为模拟通道输入引脚
    GPIO_InitStructure.GPIO_Pin = MQ2_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;									//模拟输入引脚
    GPIO_Init(MQ2_GPIO, &GPIO_InitStructure);

    ADC_DeInit(MQ2_ADC);  																//复位ADC2,将外设 ADC1 的全部寄存器重设为缺省值

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;								//ADC工作模式:ADC1和ADC2工作在独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;									//模数转换工作在单通道模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;								//模数转换工作在单次转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;				//转换由软件而不是外部触发启动
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;											//顺序进行规则转换的ADC通道的数目
    ADC_Init(MQ2_ADC, &ADC_InitStructure);												//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器


    ADC_Cmd(MQ2_ADC, ENABLE);															//使能指定的ADC2

    ADC_ResetCalibration(MQ2_ADC);														//使能复位校准

    while(ADC_GetResetCalibrationStatus(MQ2_ADC));										//等待复位校准结束

    ADC_StartCalibration(MQ2_ADC);	 													//开启AD校准

    while(ADC_GetCalibrationStatus(MQ2_ADC));					 						//等待校准结束
}
 
uint32_t MQ2_ADC_Read(void)
{
    //设置指定ADC的规则组通道，一个序列，采样时间
    ADC_RegularChannelConfig(MQ2_ADC, MQ2_ADC_CHX, 1, ADC_SampleTime_239Cycles5 );				//ADC1,ADC通道,采样时间为239.5周期

    ADC_SoftwareStartConvCmd(MQ2_ADC, ENABLE);											//使能指定的ADC1的软件转换启动功能

    while(!ADC_GetFlagStatus(MQ2_ADC, ADC_FLAG_EOC ));									//等待转换结束

    return ADC_GetConversionValue(MQ2_ADC);											//返回最近一次ADC1规则组的转换结果
}
 
//计算平均值
u16 ADC_Average_Data(u8 ADC_Channel)
{
	u16 temp_val=0;
	u8 t;
	for(t=0;t<MQ2_READ_TIMES;t++)	//#define SMOG_READ_TIMES	10	定义烟雾传感器读取次数,读这么多次,然后取平均值
 
	{
		temp_val+=MQ2_ADC_Read();	//读取ADC值
		delay_ms(5);
	}
	temp_val/=MQ2_READ_TIMES;//得到平均值
    return (u16)temp_val;//返回算出的ADC平均值
}
 
//读取MQ7传感器的电压值
float Smog_Get_Vol(void)
{
	u16 adc_value = 0;//这是从MQ-7传感器模块电压输出的ADC转换中获得的原始数字值，该值的范围为0到4095，将模拟电压表示为数字值
	float voltage = 0;//MQ-7传感器模块的电压输出，与一氧化碳的浓度成正比
	
	adc_value = ADC_Average_Data(MQ2_ADC_CHX);//#define SMOG_ADC_CHX	ADC_Channel_4	定义烟雾传感器所在的ADC通道编号
	delay_ms(5);
	
    voltage  = (3.3/4096.0)*(adc_value);
	
	return voltage;
}

// 传感器校准函数，根据当前环境PPM值与测得的RS电压值，反推出R0值。
// 在空气中运行过后测出R0
//float MQ2_PPM_Calibration()
//{
//	float RS = 0;
//	float R0 = 0;
//	RS = (3.3f - Smog_Get_Vol()) / Smog_Get_Vol() * RL;//RL	10  // RL阻值
//	R0 = RS / pow(CAL_PPM / 613.9f, 1 / -2.074f);//CAL_PPM  10  // 校准环境中PPM值
//	return R0;
//}

 
//计算Smog_ppm
float Smog_GetPPM()
{
	float RS = (3.3f - Smog_Get_Vol()) / Smog_Get_Vol() * RL;
	float ppm = 613.9f * pow(RS/R0, -2.074f);
	return  ppm;
}
