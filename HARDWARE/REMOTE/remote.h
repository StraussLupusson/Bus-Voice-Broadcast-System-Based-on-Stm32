#ifndef __RED_H
#define __RED_H 
#include "sys.h"   

//#define MOBILE_REMOTE  1
#define RDATA PAin(1)	 //红外数据输入脚

#define   ERROR 		 0u			   
#define   CH_DEC		162u
#define   CH		 	   98u
#define   CH_INC         226u
#define	  LEFT            34u
#define   RIGHT           2u
#define   STOP            194u
#define   VOL_DEC         224u
#define   VOL_INC         168u
#define   EQ              144u
#define   KEY_0          104u
#define   KEY_100        152u
#define   KEY_200        176u
#define   KEY_1          48u
#define   KEY_2           24u
#define	  KEY_3          122u
#define	  KEY_4          16u
#define	  KEY_5          56u
#define	  KEY_6          90u
#define	  KEY_7          66u
#define	  KEY_8           74u
#define	  KEY_9           82u
			    
		 
//红外遥控识别码(ID),每款遥控器的该值基本都不一样,但也有一样的.
//我们选用的遥控器识别码为0
#define REMOTE_ID 0     

typedef enum {SINGLE_PRES,DOUBLE_PRES = 1} Pres_Mode;//按下模式是否连按
extern u8 RmtCnt;	//按键按下的次数
extern u8 	REMORE_STA;
void Remote_Init(void);    //红外传感器接收头引脚初始化
u8 *Get_RemoteKey(Pres_Mode);
u8 Get_RemoteKeyValue(void);
u8 Remote_Scan(Pres_Mode mode);	    
#endif

