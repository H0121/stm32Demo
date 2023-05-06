
#include "keypad.h"

#define ROW 4		// 4*3矩阵键盘
#define COL 4

GPIO_TypeDef* ROW_GPIO_PORT[ROW] = {GPIOA, GPIOC, GPIOC, GPIOC};					// 行引脚断端口
u32 ROW_GPIO_PIN[ROW] = {GPIO_Pin_0, GPIO_Pin_2, GPIO_Pin_0, GPIO_Pin_14};	// 行引脚

GPIO_TypeDef* COL_GPIO_PORT[COL] = {GPIOC, GPIOC, GPIOC,GPIOC};									// 列引脚端口
u32 COL_GPIO_PIN[COL] = {GPIO_Pin_3, GPIO_Pin_1, GPIO_Pin_15,GPIO_Pin_13};						// 列引脚

char KEY[ROW][COL] = {				// 键值矩阵
		{'1','2','3','A'},
		{'4','5','6','B'},
		{'7','8','9','C'},
		{'*','0','#','D'}
};

void Keypad_Init() {			// 初始化行列引脚
	int i;
	for(i = 0; i < ROW; i++)
		Keypad_GPIO_Init(ROW_GPIO_PORT[i], ROW_GPIO_PIN[i], GPIO_Mode_Out_PP);	// 初始化行引脚，输出
	for(i = 0; i < COL; i++)
		Keypad_GPIO_Init(COL_GPIO_PORT[i], COL_GPIO_PIN[i], GPIO_Mode_IPU);			// 初始化列引脚，输入
}

void Keypad_GPIO_Init(GPIO_TypeDef* GPIOx, u32 GPIO_Pin, GPIOMode_TypeDef GPIO_Mode) {	// 引脚配置函数
	GPIO_InitTypeDef GPIO_InitStruct;
	if(GPIOx == GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	else if(GPIOx == GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	else if(GPIOx == GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	else if(GPIOx == GPIOD) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

char Getkey() {				// 键盘扫描函数，返回按下的键值
	int i, j;
	for(i = 0; i < ROW; i++) 
	{
		for(j = 0; j < ROW; j++)
		{
			if(j != i)
				GPIO_SetBits(ROW_GPIO_PORT[j], ROW_GPIO_PIN[j]);
			GPIO_ResetBits(ROW_GPIO_PORT[i], ROW_GPIO_PIN[i]);
		}
		for(j = 0; j < COL; j++) 
		{
			if(!GPIO_ReadInputDataBit(COL_GPIO_PORT[j], COL_GPIO_PIN[j]))
				return KEY[i][j];
		}
  }
	return 0;
}

