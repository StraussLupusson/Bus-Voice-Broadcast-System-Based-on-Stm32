/*
	文件名：      dht11.c
	文件描述： STM32F03C8T6---dht11_Device Driver
	修改时间： 2018年4月21日
	作者：	     汪武林
	其他说明： 无
	
*/
#include "dht11.h"
#include "delay.h"

/******************************************************************************************************** 
**     函数名称:         DHT11_Init
**    功能描述:          DHT11初始化
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 	 
ErrorStatus DHT11_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(DHT11_GPIO_CLK, ENABLE);//使能GPIOG时钟

  //GPIOF9,F10初始化设置
  GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);//初始化
  DHT11_Start();
	return DHT11_Check();
}
   
/******************************************************************************************************** 
**     函数名称:         DHT11_Start
**    功能描述:          DHT11起始信号
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void DHT11_Start(void)	   
{                 
	DHT11_MODE_OUT(); 	//SET OUTPUT
	DHT11_DQ_OUT=0; 	//拉低DQ
	delay_ms(20);    	//拉低至少18ms
	DHT11_DQ_OUT=1; 	//DQ=1 
	delay_us(30);     	//主机拉高20~40us
}

/******************************************************************************************************** 
**     函数名称:         DHT11_Check
**    功能描述:          等待DHT11响应
**    入口参数：       无
**    出口参数:          ErrorStatus
**    其他说明：       //返回ERROR:未检测到DHT11的存在
						//返回SUCCESS:存在
********************************************************************************************************/ 
ErrorStatus DHT11_Check(void) 	   
{   
	u8 sleep=0;
	DHT11_MODE_IN();//SET INPUT	 
    while ((DHT11_DQ_IN == 1)&&sleep<100)//等待应答变低
	{
		sleep++;
		delay_us(1);
	};	 
	if(sleep>=100)return ERROR;
	else sleep=0;
    while ((DHT11_DQ_IN == 0)&&sleep<100)//等待应答变高
	{
		sleep++;
		delay_us(1);
	};
	if(sleep>=100)return ERROR;	    
	return SUCCESS;
}

/******************************************************************************************************** 
**     函数名称:         DHT11_Read_One_Bit
**    功能描述:          从DHT11读取一个位
**    入口参数：      	 无
**    出口参数:          u8
**    其他说明：      //返回值：1/0
********************************************************************************************************/ 
u8 DHT11_Read_One_Bit(void) 			 
{
 	u8 sleep=0;
	while((DHT11_DQ_IN == 1) &&sleep<100)//等待变为低电平
	{
		sleep++;
		delay_us(1);
	}
	sleep=0;
	while((DHT11_DQ_IN == 0)&&sleep<100)//等待变高电平
	{
		sleep++;
		delay_us(1);
	}
	delay_us(40);//等待40us
	if(DHT11_DQ_IN)return 1;
	else return 0;		   
}

/******************************************************************************************************** 
**     函数名称:         DHT11_Read_One_Byte
**    功能描述:          //从DHT11读取一个字节
**    入口参数：       无
**    出口参数:          u8
**    其他说明：       //返回值：读到的数据
********************************************************************************************************/ 
u8 DHT11_Read_One_Byte(void)    
{        
    u8 i,dat;
    dat=0;
	for (i=0;i<8;i++) 
	{
   		dat<<=1; 
	    dat|=DHT11_Read_One_Bit();
    }						    
    return dat;
}

/******************************************************************************************************** 
**     函数名称:         DHT11_Read_Data
**    功能描述:          //从DHT11读取一次数据
**    入口参数：       	DHT11_Data *data
**    出口参数:          u8
**    其他说明：       //返回值：0,正常;1,读取失败,读到的数据存到data
********************************************************************************************************/ 
ErrorStatus DHT11_Read_Data(DHT11_Data *data)
{        
 	u8 buf[5];
	u8 i;
	DHT11_Start();
	if(DHT11_Check()==SUCCESS)
	{
		for(i=0;i<5;i++)//读取40位数据
		{
			buf[i]=DHT11_Read_One_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			data->shidu= (float)buf[0]+(float)buf[1]/255;
			data->wendu= (float)buf[2]+(float)buf[3]/255;
		}
		return  SUCCESS;	 
	}
	else return ERROR; 
}

