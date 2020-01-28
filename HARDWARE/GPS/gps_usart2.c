/*
	�ļ�����      gps_usart2.c
	�ļ������� STM32F03C8T6---LED_Device Driver
	�޸�ʱ�䣺 2018��4��21��
	���ߣ�	     ������
	����˵���� ��
	
*/
#include "gps_usart2.h"
#include <stdarg.h>
#include "usart.h"
/******************************************************************************************************** 
**     ��������:         GPS_USART_Init
**    ��������:          ��ʼ������2����������2�����ж�
**    ��ڲ�����       ��
**    ���ڲ���:          ��
**    ����˵����       ��
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
	USART_InitStructure.USART_BaudRate = 9600;                //GPSģ��Ĭ��ʹ�ò����ʣ�9600
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(GPS_USART, &USART_InitStructure); 

	USART_Cmd(GPS_USART, ENABLE);
	
	 USART_ITConfig(GPS_USART, USART_IT_RXNE, ENABLE);//�����ж�   
	
	NVIC_InitStructure.NVIC_IRQChannel = GPS_USART_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	prt("GPS_USART finish \r\n");
}


//�жϻ��洮������
volatile    uint8_t uart_p = 0;
uint8_t     uart_buff[UART_BUFF_SIZE];
 vu8  Flag_GPGGA_OK = 0;
 vu8  Flag_GPRMC_OK = 0;
 vu8 Flag_time_OK = 0;
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif 
/******************************************************************************************************** 
**     ��������:         USART2_IRQHandler
**    ��������:          USART2�жϷ����������Ͻ���gps����
**    ��ڲ�����       ��
**    ���ڲ���:          ��
**    ����˵����       ���Ͻ���GPS���ݱ��浽ȫ������uart_buff��������Flag_GPS_OK��־���ñ�־����GPS���ݵĽ�������
********************************************************************************************************/ 
void USART2_IRQHandler(void)
{
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();     // ʹ��uC/OSһ��Ҫ���
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
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  	 // ʹ��uC/OSһ��Ҫ���										 
#endif
}

