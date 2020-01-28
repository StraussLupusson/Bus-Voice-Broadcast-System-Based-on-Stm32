/*
	�ļ����� led.c
	�ļ�������STM32F03C8T6---LED_Device Driver
	�޸�ʱ�䣺2018��4��21��
	���ߣ�������
	
*/

#include "led.h"
#include "delay.h"


/******************************************************************************************************** 
**     ��������:         LED_Init
**    ��������:          LED��ʼ��
**    ��ڲ�����       	 ��
**    ���ڲ���:          ��
********************************************************************************************************/ 
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	 
  RCC_APB2PeriphClockCmd(LED0_GPIO_CLK, ENABLE);	    //ʹ��PC�˿�ʱ��
  GPIO_InitStructure.GPIO_Pin = LED0_PIN;			    //LED0-->PC.13 �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO���ٶ�Ϊ50MHz
  GPIO_Init(LED0_GPIO_PORT, &GPIO_InitStructure);			     //��ʼ��PC.13
  GPIO_SetBits(LED0_GPIO_PORT,LED0_PIN);					//PC.13 �����

}

#if 0
/******************************************************************************************************** 
**     ��������:         LED_PWM_Init
**    ��������:          LED������TIM PWM��ʼ��
**    ��ڲ�����         ��
**    ���ڲ���:          ��
********************************************************************************************************/ 
void LED_PWM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;
	//NVIC_InitTypeDef  TIME_NVIC_InitStruct;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC ,ENABLE);
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3������ӳ��  TIM3_CH2->PB5    
 
   //���ø�����Ϊ�����������,���TIM3 CH2��PWM���岨��	GPIOB.5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIO
 
	
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
**     ��������:         LEDStroll
**    ��������:          ��ˮ��
**    ��ڲ�����       	 ��
**    ���ڲ���:          ��
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
**     ��������:         BreLight
**    ��������:          ������
**    ��ڲ�����       	 ��
**    ���ڲ���:          ��
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
**     ��������:         Light
**    ��������:          PWM ���
**    ��ڲ�����       	 u8 light ���ƹ����� 0--100
**    ���ڲ���:          ��
********************************************************************************************************/ 
void Light(u8 light)
{
	TIM_SetCompare1(TIM3,light);
}

/******************************************************************************************************** 
**     ��������:         CloseAllLed
**    ��������:          �ر����е�
**    ��ڲ�����       	 ��
**    ���ڲ���:          ��
********************************************************************************************************/ 
void CloseAllLed(void)
{
	LED1 = LED2 = LED3 = LED4 = 1;
}
/******************************************************************************************************** 
**     ��������:         OpenAllLed
**    ��������:          �����е�
**    ��ڲ�����       	 ��
**    ���ڲ���:          ��
********************************************************************************************************/ 
void OpenAllLed(void)
{
	LED1 = LED2 = LED3 = LED4 = 0;
	
}
#endif