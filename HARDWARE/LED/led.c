/*
	文件名： led.c
	文件描述：STM32F03C8T6---LED_Device Driver
	修改时间：2018年4月21日
	作者：汪武林
	
*/

#include "led.h"
#include "delay.h"


/******************************************************************************************************** 
**     函数名称:         LED_Init
**    功能描述:          LED初始化
**    入口参数：       	 无
**    出口参数:          无
********************************************************************************************************/ 
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	 
  RCC_APB2PeriphClockCmd(LED0_GPIO_CLK, ENABLE);	    //使能PC端口时钟
  GPIO_InitStructure.GPIO_Pin = LED0_PIN;			    //LED0-->PC.13 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO口速度为50MHz
  GPIO_Init(LED0_GPIO_PORT, &GPIO_InitStructure);			     //初始化PC.13
  GPIO_SetBits(LED0_GPIO_PORT,LED0_PIN);					//PC.13 输出高

}

#if 0
/******************************************************************************************************** 
**     函数名称:         LED_PWM_Init
**    功能描述:          LED呼吸灯TIM PWM初始化
**    入口参数：         无
**    出口参数:          无
********************************************************************************************************/ 
void LED_PWM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;
	//NVIC_InitTypeDef  TIME_NVIC_InitStruct;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能定时器3时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC ,ENABLE);
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3部分重映射  TIM3_CH2->PB5    
 
   //设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形	GPIOB.5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIO
 
	
	TIM_TimeBaseInitStruct.TIM_Prescaler     = 8399;
	TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period        = 10000/LED_PRE-1;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	//TIM_TimeBaseInitStruct.TIM_RepetitionCounter =
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
	
	
	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	
	TIM_Cmd(TIM3,ENABLE);
	
	TIM_SetCompare1(TIM3,0);

//	TIM_CtrlPWMOutputs(TIM1,ENABLE);
//	TIM_Cmd(TIM1,ENABLE);
	
	//TIM_SetCompare3(TIM1,0);
	//TIM_SetCompare4(TIM1,0);
	
}
/******************************************************************************************************** 
**     函数名称:         LEDStroll
**    功能描述:          流水灯
**    入口参数：       	 无
**    出口参数:          无
********************************************************************************************************/ 
void LEDStroll(void)
{
	
		//GPIO_WriteBit(GPIOF,GPIO_Pin_8,Bit_SET);
		//delay(20000000);
		GPIO_WriteBit(GPIOF,GPIO_Pin_8,Bit_RESET);
		delay_ms(500);
		GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_RESET);
		GPIO_WriteBit(GPIOF,GPIO_Pin_8,Bit_SET);
		delay_ms(500);
		GPIO_WriteBit(GPIOF,GPIO_Pin_9,Bit_SET);
		GPIO_WriteBit(GPIOF,GPIO_Pin_10,Bit_RESET);
		delay_ms(500);
		GPIO_WriteBit(GPIOF,GPIO_Pin_10,Bit_SET);
		GPIO_WriteBit(GPIOE,GPIO_Pin_13,Bit_RESET);
		delay_ms(500);
		
		GPIO_WriteBit(GPIOE,GPIO_Pin_13,Bit_SET);
		GPIO_WriteBit(GPIOE,GPIO_Pin_14,Bit_RESET);
		delay_ms(500);
		GPIO_WriteBit(GPIOE,GPIO_Pin_14,Bit_SET);
		delay_ms(500);
		
	
}

/******************************************************************************************************** 
**     函数名称:         BreLight
**    功能描述:          呼吸灯
**    入口参数：       	 无
**    出口参数:          无
********************************************************************************************************/ 
void BreLight(void)
{
	int i = 50;
	int flag = 1;
	while(1)
	{
		if(flag)
		{
			i++;
			TIM_SetCompare1(TIM3,i);
			//TIM_SetCompare3(TIM1,i);
			//TIM_SetCompare4(TIM1,i);
			
			delay_ms(5);
			if(i==100)
				flag = 0;
				
		}
		else
		{
			i--;
			TIM_SetCompare1(TIM3,i);
			//TIM_SetCompare3(TIM1,i);
			//TIM_SetCompare4(TIM1,i);
			delay_ms(5);
			if(i==0)
				flag = 1;
		}
			
	}
}
/******************************************************************************************************** 
**     函数名称:         Light
**    功能描述:          PWM 点灯
**    入口参数：       	 u8 light ，灯光亮度 0--100
**    出口参数:          无
********************************************************************************************************/ 
void Light(u8 light)
{
	TIM_SetCompare1(TIM3,light);
}

/******************************************************************************************************** 
**     函数名称:         CloseAllLed
**    功能描述:          关闭所有灯
**    入口参数：       	 无
**    出口参数:          无
********************************************************************************************************/ 
void CloseAllLed(void)
{
	LED1 = LED2 = LED3 = LED4 = 1;
}
/******************************************************************************************************** 
**     函数名称:         OpenAllLed
**    功能描述:          打开所有灯
**    入口参数：       	 无
**    出口参数:          无
********************************************************************************************************/ 
void OpenAllLed(void)
{
	LED1 = LED2 = LED3 = LED4 = 0;
	
}
#endif