/*
	文件名：      gps_usart2.c
	文件描述： STM32F03C8T6---LED_Device Driver
	修改时间： 2018年4月21日
	作者：	     汪武林
	其他说明： 无
	
*/
#include "gps_usart2.h"
#include <stdarg.h>
#include "usart.h"
/******************************************************************************************************** 
**     函数名称:         GPS_USART_Init
**    功能描述:          初始化串口2，开启串口2接收中断
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void GPS_USART_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	/* config GPS_USART clock */
	RCC_APB2PeriphClockCmd(GPS_GPIO_CLK, ENABLE);
	RCC_APB1PeriphClockCmd(GPS_USART_CLK, ENABLE);

	/* GPS_USART GPIO config */
  /* Configure GPS_USART Tx (PA.02) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPS_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPS_GPIO_PORT, &GPIO_InitStructure);
	    
  /* Configure GPS_USART Rx (PA.03) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPS_RX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPS_GPIO_PORT, &GPIO_InitStructure);
	  
	/* GPS_USART mode config */
	USART_InitStructure.USART_BaudRate = 9600;                //GPS模块默认使用波特率：9600
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(GPS_USART, &USART_InitStructure); 

	USART_Cmd(GPS_USART, ENABLE);
	
	 USART_ITConfig(GPS_USART, USART_IT_RXNE, ENABLE);//开启中断   
	
	NVIC_InitStructure.NVIC_IRQChannel = GPS_USART_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	prt("GPS_USART finish \r\n");
}


//中断缓存串口数据
volatile    uint8_t uart_p = 0;
uint8_t     uart_buff[UART_BUFF_SIZE];
 vu8  Flag_GPGGA_OK = 0;
 vu8  Flag_GPRMC_OK = 0;
 vu8 Flag_time_OK = 0;
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif 
/******************************************************************************************************** 
**     函数名称:         USART2_IRQHandler
**    功能描述:          USART2中断服务函数，不断接收gps数据
**    入口参数：       无
**    出口参数:          无
**    其他说明：       不断接收GPS数据保存到全局数组uart_buff，并控制Flag_GPS_OK标志，该标志决定GPS数据的解析过程
********************************************************************************************************/ 
void USART2_IRQHandler(void)
{
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();     // 使用uC/OS一定要添加
#endif
    if(uart_p<UART_BUFF_SIZE)
    {
        if(USART_GetITStatus(GPS_USART, USART_IT_RXNE) != RESET)
        {
            uart_buff[uart_p] = USART_ReceiveData(GPS_USART);
			//prt("%c",uart_buff[uart_p]);
			if(uart_buff[uart_p] == '$'){uart_p = 0;Flag_time_OK =0;Flag_GPGGA_OK = 0; Flag_GPRMC_OK = 0;}
            uart_p++;
			if(uart_p >=37 )
			{
				if(uart_buff[4] == 'M' && uart_buff[13] == '.' && uart_buff[19] == ',') Flag_time_OK = 1;
				
			}
			if(uart_p >=59 )
			{
				if( uart_buff[4] == 'G' && uart_buff[6] == ',' && uart_buff[13] == '.')	Flag_GPGGA_OK = 1;
				if( uart_buff[4] == 'M' && uart_buff[52] == ',' && uart_buff[59] == ',')Flag_GPRMC_OK = 1;
			}
        }
    }
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  	 // 使用uC/OS一定要添加										 
#endif
}

