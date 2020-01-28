#ifndef __RED_H
#define __RED_H 
#include "sys.h"   

//#define MOBILE_REMOTE  1
#define RDATA PAin(1)	 //�������������

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
			    
		 
//����ң��ʶ����(ID),ÿ��ң�����ĸ�ֵ��������һ��,��Ҳ��һ����.
//����ѡ�õ�ң����ʶ����Ϊ0
#define REMOTE_ID 0     

typedef enum {SINGLE_PRES,DOUBLE_PRES = 1} Pres_Mode;//����ģʽ�Ƿ�����
extern u8 RmtCnt;	//�������µĴ���
extern u8 	REMORE_STA;
void Remote_Init(void);    //���⴫��������ͷ���ų�ʼ��
u8 *Get_RemoteKey(Pres_Mode);
u8 Get_RemoteKeyValue(void);
u8 Remote_Scan(Pres_Mode mode);	    
#endif

