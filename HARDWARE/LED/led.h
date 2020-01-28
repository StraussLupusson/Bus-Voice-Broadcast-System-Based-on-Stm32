/*
	文件名： led.c
	文件描述：STM32F03C8T6---LED_Device Driver
	修改时间：2018年4月21日
	作者：汪武林
	
*/
#ifndef __LED_H_
#define __LED_H_
#include "sys.h"


//-----------------LED 端口定义----------------  					   
#define LED0_PIN                 GPIO_Pin_12
#define LED0_GPIO_PORT            GPIOB
#define LED0_GPIO_CLK             RCC_APB2Periph_GPIOB
#define LED0 PBout(12)

//#define LED1 PFout(9)
//#define LED2 PFout(10)
//#define LED3 PEout(13)
//#define LED4 PEout(14)

//位带实现IO端口
//#define MYBITBAND(addr,bit) (uint32_t *)(0x42000000 + ((addr - 0x40000000)<<5) + (bit<<2) )   //寄存器映射地址公式
//#define PFO(n)  *(volatile uint32_t *)MYBITBAND(GPIOF_BASE+0x14,n)     //输出位带操作

#define LED_PRE 100     //PWM呼吸灯灯频率


void LED_Init(void);    //led初始化

//void LEDStroll(void);
//void OpenAllLed(void);
//void CloseAllLed(void);

////呼吸灯
//void Light(u8 light);
//void LED_PWM_Init(void);  //调亮度
//void BreLight(void);

#endif



