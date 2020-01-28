/*
	�ļ�����      ISD4004.h
	�ļ������� STM32F03C8T6---ISD4004_Device Driver
	�޸�ʱ�䣺 2018��4��21��
	���ߣ�	     ������
	����˵���� ��
	
*/
#ifndef __ISD4004_H_
#define __ISD4004_H_

#include "sys.h"

#define uchar unsigned char
#define uint  unsigned int

//¼���ͷ�����IO�ڶ��壺

#define   AN  PCin(14)//���������ƽӿ�
#define   SW_RECORD PCin(13)//¼�������ƿ�

// ISD4004���ƿڶ��壺

#define ISD4004_GPIO_CLK              RCC_APB2Periph_GPIOB
#define ISD4004_GPIO_PORT            GPIOB
#define ISD4004_SS_PIN              GPIO_Pin_5
#define ISD4004_MOSI_PIN              GPIO_Pin_6
//#define ISD4004_MISO_PIN              GPIO_Pin_9
#define ISD4004_SCLK_PIN              GPIO_Pin_8
#define ISD4004_INT_PIN              GPIO_Pin_7

#define ISD4004_AN_PIN              GPIO_Pin_14
#define ISD4004_SW_RECORD_PIN       GPIO_Pin_13

#define SS  PBout(5) 	//4004Ƭѡ
#define MOSI PBout(6) 	//4004��������
//#define MISO PAin(9) 	//4004�������
#define SCLK PBout(8)//ISD4004ʱ��
#define INT  PBin(7)	//4004�ж�

#define VOICE_PASSAGE_NUM   3  //����Ƭ����

#define ISD4004_START_ADDR  1900
#define ISD4004_ADDR_INTERVAL  3
#define TIME4_FREQ          100   //100ms

//isd4004��������
#define ISD4004_STOP_CMD  0x30
#define ISD4004_POWERUP_CMD  0x20
#define ISD4004_STOPWRDN_CMD  0x10
#define ISD4004_REC_CMD    0xb0
#define ISD4004_SETREC_CMD  0xa0
#define ISD4004_SETPLAY_CMD  0xe0
#define ISD4004_PLAY_CMD  0xf0

extern  vu8 request; //�ⲿ��������
void 	ISD4004_Init(void);
void 	ISD4004_TIM4Init(u16 msec);  //�ڲ�����
void    ISD4004_RecordingTreat(void);//¼�����������
void    ISD4004_PlayingTreat(void);//���ż��������
void    ISD4004_Recording(u16 st_addr); // luyin
void    ISD4004_Playing1(u16 st_addr,u16 edd_addr);
void    ISD4004_Playing(u16 addr);
void    ISD4004_Display(void);//��ʾ�������
void    ISD4004_setrec(uchar adl,uchar adh);//����setrecָ��
void    ISD4004_rec(void);//����recָ��
void    ISD4004_stop(void);//stopָ�ֹͣ��ǰ������
void    ISD4004_powerup(void);//�����ϵ�ָ��
void    ISD4004_stopwrdn(void);//���͵���ָ��
void    ISD4004_send(uchar isdx);//spi���з����ӳ���8λ����
void    ISD4004_setplay(uchar adl,uchar adh);
void    ISD4004_play(void);
void    ISD4004_StopPlaying(void);
#endif
