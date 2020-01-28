/*
	�ļ�����      dht11.c
	�ļ������� STM32F03C8T6---DHT11_Device Driver
	�޸�ʱ�䣺 2018��4��21��
	���ߣ�	     ������
	����˵���� ��
	
*/
#ifndef __DHT11_H_
#define __DHT11_H_
#include "sys.h"   

//-----------------DHT11 �˿ڶ���----------------  					   
#define DHT11_GPIO_PIN              GPIO_Pin_1
#define DHT11_GPIO_PORT             GPIOB
#define DHT11_GPIO_CLK             RCC_APB2Periph_GPIOB

////IO��������											   
#define	DHT11_DQ_OUT PBout(1) //���ݶ˿�	PG9 
#define	DHT11_DQ_IN  PBin(1)  //���ݶ˿�	PG9 

//IO��������
#define DHT11_MODE_IN()  {GPIOB->CRL&=0xFFFFFF0F;GPIOB->CRL|=8<<(4*1);}	//PG9����ģʽ
#define DHT11_MODE_OUT() {GPIOB->CRL&=0xFFFFFF0F;GPIOB->CRL|=3<<(4*1);} 	//PG9���ģʽ 


typedef struct
{
	float wendu;
	float shidu;
}DHT11_Data;

ErrorStatus DHT11_Init(void);//��ʼ��DHT11
ErrorStatus DHT11_Read_Data(DHT11_Data *data);//��ȡ��ʪ��
u8 DHT11_Read_One_Byte(void);//����һ���ֽ�
u8 DHT11_Read_One_Bit(void);//����һ��λ
ErrorStatus DHT11_Check(void);//����Ƿ����DHT11
void DHT11_Start(void);//��λDHT11    
#endif















