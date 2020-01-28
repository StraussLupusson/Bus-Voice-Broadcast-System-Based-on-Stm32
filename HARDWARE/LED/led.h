/*
	�ļ����� led.c
	�ļ�������STM32F03C8T6---LED_Device Driver
	�޸�ʱ�䣺2018��4��21��
	���ߣ�������
	
*/
#ifndef __LED_H_
#define __LED_H_
#include "sys.h"


//-----------------LED �˿ڶ���----------------  					   
#define LED0_PIN                 GPIO_Pin_12
#define LED0_GPIO_PORT            GPIOB
#define LED0_GPIO_CLK             RCC_APB2Periph_GPIOB
#define LED0 PBout(12)

//#define LED1 PFout(9)
//#define LED2 PFout(10)
//#define LED3 PEout(13)
//#define LED4 PEout(14)

//λ��ʵ��IO�˿�
//#define MYBITBAND(addr,bit) (uint32_t *)(0x42000000 + ((addr - 0x40000000)<<5) + (bit<<2) )   //�Ĵ���ӳ���ַ��ʽ
//#define PFO(n)  *(volatile uint32_t *)MYBITBAND(GPIOF_BASE+0x14,n)     //���λ������

#define LED_PRE 100     //PWM�����Ƶ�Ƶ��


void LED_Init(void);    //led��ʼ��

//void LEDStroll(void);
//void OpenAllLed(void);
//void CloseAllLed(void);

////������
//void Light(u8 light);
//void LED_PWM_Init(void);  //������
//void BreLight(void);

#endif



