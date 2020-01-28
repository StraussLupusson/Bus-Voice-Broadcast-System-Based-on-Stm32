/*
	文件名：      ISD4004.h
	文件描述： STM32F03C8T6---ISD4004_Device Driver
	修改时间： 2018年4月21日
	作者：	     汪武林
	其他说明： 无
	
*/
#ifndef __ISD4004_H_
#define __ISD4004_H_

#include "sys.h"

#define uchar unsigned char
#define uint  unsigned int

//录音和放音键IO口定义：

#define   AN  PCin(14)//放音键控制接口
#define   SW_RECORD PCin(13)//录音键控制口

// ISD4004控制口定义：

#define ISD4004_GPIO_CLK              RCC_APB2Periph_GPIOB
#define ISD4004_GPIO_PORT            GPIOB
#define ISD4004_SS_PIN              GPIO_Pin_5
#define ISD4004_MOSI_PIN              GPIO_Pin_6
//#define ISD4004_MISO_PIN              GPIO_Pin_9
#define ISD4004_SCLK_PIN              GPIO_Pin_8
#define ISD4004_INT_PIN              GPIO_Pin_7

#define ISD4004_AN_PIN              GPIO_Pin_14
#define ISD4004_SW_RECORD_PIN       GPIO_Pin_13

#define SS  PBout(5) 	//4004片选
#define MOSI PBout(6) 	//4004数据输入
//#define MISO PAin(9) 	//4004数据输出
#define SCLK PBout(8)//ISD4004时钟
#define INT  PBin(7)	//4004中断

#define VOICE_PASSAGE_NUM   3  //语音片段数

#define ISD4004_START_ADDR  1900
#define ISD4004_ADDR_INTERVAL  3
#define TIME4_FREQ          100   //100ms

//isd4004控制命令
#define ISD4004_STOP_CMD  0x30
#define ISD4004_POWERUP_CMD  0x20
#define ISD4004_STOPWRDN_CMD  0x10
#define ISD4004_REC_CMD    0xb0
#define ISD4004_SETREC_CMD  0xa0
#define ISD4004_SETPLAY_CMD  0xe0
#define ISD4004_PLAY_CMD  0xf0

extern  vu8 request; //外部放音请求
void 	ISD4004_Init(void);
void 	ISD4004_TIM4Init(u16 msec);  //内部函数
void    ISD4004_RecordingTreat(void);//录音键处理程序
void    ISD4004_PlayingTreat(void);//播放键处理程序
void    ISD4004_Recording(u16 st_addr); // luyin
void    ISD4004_Playing1(u16 st_addr,u16 edd_addr);
void    ISD4004_Playing(u16 addr);
void    ISD4004_Display(void);//显示处理程序
void    ISD4004_setrec(uchar adl,uchar adh);//发送setrec指令
void    ISD4004_rec(void);//发送rec指令
void    ISD4004_stop(void);//stop指令（停止当前操作）
void    ISD4004_powerup(void);//发送上电指令
void    ISD4004_stopwrdn(void);//发送掉电指令
void    ISD4004_send(uchar isdx);//spi串行发送子程序，8位数据
void    ISD4004_setplay(uchar adl,uchar adh);
void    ISD4004_play(void);
void    ISD4004_StopPlaying(void);
#endif
