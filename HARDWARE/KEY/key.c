/*
	�ļ�����      key.c
	�ļ������� STM32F03C8T6---KEY_Device Driver
	�޸�ʱ�䣺 2018��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/
#include "key.h"
#include "delay.h"
#include "led.h"
/******************************************************************************************************** 
**     ��������:         KEY_Init
**    ��������:          ��ʼ������
**    ��ڲ�����       ��
**    ���ڲ���:          ��
**    ����˵����       ��
********************************************************************************************************/ 
void KEY_Init(void)
{
            
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Pin = KEY3_PIN |KEY1_PIN |KEY2_PIN;
	GPIO_InitStruct.GPIO_Mode  =  GPIO_Mode_IPU;
	
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = KEY0_PIN;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
}

/******************************************************************************************************** 
**     ��������:         KEY_Scan
**    ��������:          �������
**    ��ڲ�����       ��
**    ���ڲ���:         u8 ���尴��
**    ����˵����       ��
********************************************************************************************************/ 
u8 KEY_Scan(void)
{
	static int  k = 1;
	if(k &&(KEY0==0||KEY1==0||KEY2==0||KEY3==0))
	{
		delay_ms(10);//ȥ����
		k = 0;
		if(KEY0 == 0)
			return KEY0_PRES;
		else if(KEY1 == 0)
			return KEY1_PRES;
		else if(KEY2 == 0)
			return KEY2_PRES;
		else if(KEY3 == 0)
			return KEY3_PRES;
		
	}
	else if(KEY0==1 && KEY1==1  && KEY2==1 && KEY3==1)
		k = 1;
	return 0;
}

/******************************************************************************************************** 
**     ��������:         KEY_LED
**    ��������:          �������ģ��
**    ��ڲ�����       ��
**    ���ڲ���:          ��
**    ����˵����       ��
********************************************************************************************************/ 
#if 0
void KEY_LED(void)
{
	int key = 0;
	key = KEY_Scan();
		
	if(key)
	{
		switch(key)
		{
			case KEY0_PRES:
					
					LED0 = !LED0;
				
				break;
			case KEY1_PRES:
				
					LED0 = !LED0;
				
				break;
				
	
		}
	}
}
#endif