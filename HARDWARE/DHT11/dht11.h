/*
	文件名：      dht11.c
	文件描述： STM32F03C8T6---DHT11_Device Driver
	修改时间： 2018年4月21日
	作者：	     汪武林
	其他说明： 无
	
*/
#ifndef __DHT11_H_
#define __DHT11_H_
#include "sys.h"   

//-----------------DHT11 端口定义----------------  					   
#define DHT11_GPIO_PIN              GPIO_Pin_1
#define DHT11_GPIO_PORT             GPIOB
#define DHT11_GPIO_CLK             RCC_APB2Periph_GPIOB

////IO操作函数											   
#define	DHT11_DQ_OUT PBout(1) //数据端口	PG9 
#define	DHT11_DQ_IN  PBin(1)  //数据端口	PG9 

//IO方向设置
#define DHT11_MODE_IN()  {GPIOB->CRL&=0xFFFFFF0F;GPIOB->CRL|=8<<(4*1);}	//PG9输入模式
#define DHT11_MODE_OUT() {GPIOB->CRL&=0xFFFFFF0F;GPIOB->CRL|=3<<(4*1);} 	//PG9输出模式 


typedef struct
{
	float wendu;
	float shidu;
}DHT11_Data;

ErrorStatus DHT11_Init(void);//初始化DHT11
ErrorStatus DHT11_Read_Data(DHT11_Data *data);//读取温湿度
u8 DHT11_Read_One_Byte(void);//读出一个字节
u8 DHT11_Read_One_Bit(void);//读出一个位
ErrorStatus DHT11_Check(void);//检测是否存在DHT11
void DHT11_Start(void);//复位DHT11    
#endif















