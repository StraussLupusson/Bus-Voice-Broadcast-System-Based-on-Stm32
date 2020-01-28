/*
	文件名：      key.c
	文件描述： STM32F03C8T6--key_Device Driver
	修改时间： 2018年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/
#ifndef __KEY_H_
#define __KEY_H_
#include "sys.h"


#define KEY0_PIN                 GPIO_Pin_0
#define KEY1_PIN                 GPIO_Pin_15
#define KEY2_PIN                 GPIO_Pin_14
#define KEY3_PIN                 GPIO_Pin_13


#define  KEY0_PRES  1
#define  KEY1_PRES  2
#define  KEY2_PRES  3
#define  KEY3_PRES  4

#define  KEY0  PAin(0)
#define  KEY1  PCin(15)
#define  KEY2  PCin(14)
#define  KEY3  PCin(13)
//#define  KEY3  PEin(4)

//初始化按键
void KEY_Init(void);
//按键检测
u8 KEY_Scan(void);
//非中断
void KEY_LED(void);

#endif

