/*
	文件名：      GPS.c
	文件描述： STM32F03C8T6---GPS Driver
	修改时间： 2018年4月21日
	作者：	     汪武林
	其他说明： 无
	
*/
#ifndef __GPS_H_
#define	__GPS_H_

#include "sys.h"
#include "gps_usart2.h"

/*******************GPS NMEA-0183协议重要参数结构体定义 ******************************************/

//UTC时间信息
__packed typedef struct  
{										    
 	u16 year;	//年份
	u8 month;	//月份
	u8 date;	//日期
	u8 hour; 	//小时
	u8 min; 	//分钟
	u8 sec; 	//秒钟
}nmea_utc_time;   	   
//NMEA 0183 协议解析后数据存放结构体
__packed typedef struct  
{										    
 	u8 svnum;					//可见GPS卫星数
	nmea_utc_time utc;			//UTC时间
	nmea_utc_time LocalTime;		//北京时间
	double latitude_val;				//纬度 
	u8 latitude[11];				//纬度 
	u8 nshemi;					//北纬/	  
	double longitude_val;			    //经度 
	u8 longitude[12];			    //经度 
	u8 ewhemi;					//东经/西经,E:东经;W:西经
	char gpssta[15];					//GPS状态:0,未定位;1,非差分定位;2,差分定位;6,正在估算.				  
	u8 altitude[5];			 	//海拔高度
	double altitude_val;
	u8 speed[6];					//地面速率
	double speed_val;
}nmea_msg;
/*****************************************  end  ***************************************************/
extern vu8 GPGGA_OK_FLAG;
extern vu8 GPRMC_OK_FLAG ;

extern nmea_msg GPS_Msg;  /*定义GPS信息变量*/
#define GPS_Init GPS_USART_Init

#define POS_NUM 9			//站点数目
#define PRECISION 0.00005  // 定位精度
u8 IsLeapYear(u8 iYear) ;//判断平闰年
void GPS_GetTime(void);
void GPS_AnalysisData(void);
u8 GPS_GetPos(double latitude,double longitude,const double busline[POS_NUM][2]) ;

#endif 
