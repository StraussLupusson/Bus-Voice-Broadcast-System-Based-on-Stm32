/*
	�ļ�����      busposmesg.h
	�ļ������� ����վ����Ϣͷ
	�޸�ʱ�䣺 2019��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/
#ifndef __BUSPOSMESG_H_
#define __BUSPOSMESG_H_

#include "sys.h"

#define POS_NUM 9              //վ������

extern char *pos_ename[POS_NUM];				//վ��Ӣ����Ϣ
extern u32 pos_voiceaddr[POS_NUM] ;				//վ��������Ϣ
extern const double pos_gpsmesg[POS_NUM][2];   //վ�㾭γ����Ϣ
extern const char pos_cname[POS_NUM][8][16]; // վ�㺺����Ϣ
//����������Ϣ
extern const char daole[][16];
extern const char gj[][16];
extern const char productionmsg[][16];
extern const char schoolname[][16];
extern const char author[][16];
extern const char taitou[][16];
extern const char syz[][16] ;
extern const char xyz[][16] ;
extern const char wd[][16] ;
extern const char sd[][16] ;
extern const char nyr[][16] ;
extern const char title[][16];
extern const char zlay1[][16];
extern const char zlay2[][16];
extern const char riqi[][16];
extern const char shijian[][16];
extern const char zdxz[][16];
extern const char sudu[][16];
extern const char zuozhuan[][16];
extern const char youzhuan[][16];
extern const char dianbo[][16];
extern const char chaosu[][16];
extern const char set[][16];
extern const char shizhe[][16];
extern const char yuyan[][16];
extern const char timeandlanguage[][16];
extern const char tuoluoyi[][16];
extern const char dingwei[][16];
extern const char dakai[][16];
extern const char guanbi[][16];
extern const char jingweidu[][16];
extern const char jingdu[][16];
extern const char weidu[][16];
#endif
