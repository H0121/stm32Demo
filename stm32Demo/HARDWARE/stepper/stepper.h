#ifndef __STEPPER_H
#define __STEPPER_H
#include "sys.h"

extern unsigned char Flag;
extern unsigned char TIM2_Pulse_TIM3_Counter_OK;
void Stepper_init(void);
void TIM4_Master_TIM3_Slave_Configuration(u32 PulseFrequency,u32 pulse);
void Frequence_Setting(u32 PulseFrequency);
void Output_Pulse(u16 Num);
void angle_set(u8 dir,u8 angle);
void Stepper_Ctronl(u8 dir,u8 speed,u16 angle,u8 subdivide);
#endif
