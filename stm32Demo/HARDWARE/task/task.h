#ifndef __task_h
#define __task_h
#include "sys.h"

#define SIZE 5
#define sizeTemp 3
extern u8 pwd_Init[];
u8 inputPwd(char k);
void setPwd(void);
void setTemp(void);
void setHumi(void);
void addPf(void);
void delPf(void);
void timer5Init(void);
void ledControl(u8 k);
#endif
