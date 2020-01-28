/*
	文件名：      key.c
	文件描述： STM32F03C8T6---KEY_Device Driver
	修改时间： 2018年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/
#include "key.h"
#include "delay.h"
#include "led.h"
/******************************************************************************************************** 
**     函数名称:         KEY_Init
**    功能描述:          初始化按键
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
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
**     函数名称:         KEY_Scan
**    功能描述:          按键检测
**    入口参数：       无
**    出口参数:         u8 具体按键
**    其他说明：       无
********************************************************************************************************/ 
u8 KEY_Scan(void)
{
	static int  k = 1;
	if(k &&(KEY0==0||KEY1==0||KEY2==0||KEY3==0))
	{
		delay_ms(10);//去抖动
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
**     函数名称:         KEY_LED
**    功能描述:          按键点灯模板
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
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