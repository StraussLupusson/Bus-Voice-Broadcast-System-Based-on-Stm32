/*
	�ļ�����      gps_usart2.h
	�ļ������� STM32F03C8T6---gps_usart2 Driver
	�޸�ʱ�䣺 2018��4��21��
	���ߣ�	     ������
	����˵���� ��
	
*/
#ifndef __USART2_H_
#define	__USART2_H_

#include "sys.h"

#define UART_BUFF_SIZE      255
//     GPS�˿ڶ���
#define GPS_TX_PIN              GPIO_Pin_2
#define GPS_RX_PIN              GPIO_Pin_3
#define GPS_GPIO_PORT            GPIOA
#define GPS_GPIO_CLK             RCC_APB2Periph_GPIOA
#define GPS_USART_CLK             RCC_APB1Periph_USART2
#define GPS_USART  USART2
#define GPS_USART_IRQn  USART2_IRQn
//USART2_IRQHandler
void GPS_USART_Init(void);


extern volatile    uint8_t uart_p;
extern uint8_t     uart_buff[UART_BUFF_SIZE];
extern vu8  Flag_GPGGA_OK ;
extern vu8  Flag_GPRMC_OK ;
extern vu8  Flag_time_OK;
#endif /* __USART2_H */
