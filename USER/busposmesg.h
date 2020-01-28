/*
	文件名：      busposmesg.h
	文件描述： 公交站点信息头
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/
#ifndef __BUSPOSMESG_H_
#define __BUSPOSMESG_H_

#include "sys.h"

#define POS_NUM 9              //站点数量

extern char *pos_ename[POS_NUM];				//站点英文信息
extern u32 pos_voiceaddr[POS_NUM] ;				//站点语音信息
extern const double pos_gpsmesg[POS_NUM][2];   //站点经纬度信息
extern const char pos_cname[POS_NUM][8][16]; // 站点汉字信息
//其他汉字信息
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
