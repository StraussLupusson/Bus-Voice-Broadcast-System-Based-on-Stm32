/*
	�ļ�����      key.c
	�ļ������� STM32F03C8T6--key_Device Driver
	�޸�ʱ�䣺 2018��5��1��
	���ߣ�	     ������
	����˵���� ��
	
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

//��ʼ������
void KEY_Init(void);
//�������
u8 KEY_Scan(void);
//���ж�
void KEY_LED(void);

#endif

