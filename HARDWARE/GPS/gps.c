/*
	文件名：      gps.c
	文件描述： STM32F03C8T6---GPS_Device Driver
	修改时间： 2018年4月29日
	作者：	     汪武林
	其他说明： 无
	
*/
#include "gps.h"
#include "delay.h"
#include <string.h>
#include "oled.h"
#include "usart.h"
#include "ISD4004_task.h"
#include <math.h>

vu8 GPGGA_OK_FLAG = 0;
vu8 GPRMC_OK_FLAG = 0;

nmea_msg GPS_Msg;  /*定义GPS信息变量*/
static CPU_SR_ALLOC();
static void    GMTconvert(nmea_utc_time *SourceTime, nmea_utc_time *ConvertTime, uint8_t GMT,uint8_t AREA) ;
/******************************************************************************************************** 
**     函数名称:         GPS_GetTime
**    功能描述:          解析GPS模块接收到的时间数据，并保存到全局GPS_Msg结构体
**    入口参数：       无
**    出口参数:        无
**    其他说明：       阻塞型
********************************************************************************************************/ 
void GPS_GetTime(void)
{
		//获取GPRMC 时间数据
		delay_us(5);
		while(Flag_time_OK != 1 );
		OS_CRITICAL_ENTER();
		//获取UTC时间
		GPS_Msg.utc.hour = 10*(uart_buff[7]-0x30) + (uart_buff[8]-0x30);
		GPS_Msg.utc.min = 10*(uart_buff[9]-0x30) + (uart_buff[10]-0x30);
		GPS_Msg.utc.sec = 10*(uart_buff[11]-0x30) + (uart_buff[12]-0x30);
		GPS_Msg.utc.year = 10*(uart_buff[29]-0x30) + (uart_buff[30]-0x30);
		GPS_Msg.utc.month = 10*(uart_buff[27]-0x30) + (uart_buff[28]-0x30);
		GPS_Msg.utc.date = 10*(uart_buff[25]-0x30) + (uart_buff[26]-0x30);
		GMTconvert(&GPS_Msg.utc,&GPS_Msg.LocalTime,8,1);	//转北京时间
		Flag_time_OK = 0;
		
		OS_CRITICAL_EXIT();
		
		
		
}

/******************************************************************************************************** 
**     函数名称:         GPS_AnalysisData
**    功能描述:          解析GPS模块接收到的数据，并保存到全局GPS_Msg结构体
**    入口参数：       无
**    出口参数:        无
**    其他说明：       阻塞型
********************************************************************************************************/ 
void GPS_AnalysisData(void)
{
			//获取GPGGA 数据
			delay_ms(20);
			while(Flag_GPGGA_OK != 1);
			OS_CRITICAL_ENTER();
			prt("GPGGA OK  \r\n");
			GPS_Msg.nshemi   = uart_buff[28];
			sprintf((char *)&GPS_Msg.latitude,"%c%c%c%c%c%c%c%c%c%c",\
										uart_buff[17],uart_buff[18],\
										uart_buff[19],uart_buff[20],
										uart_buff[21], //dot
										uart_buff[22],uart_buff[23],uart_buff[24],uart_buff[25],uart_buff[26]);
			
			GPS_Msg.ewhemi   = uart_buff[42];
			sprintf((char *)&GPS_Msg.longitude,"%c%c%c%c%c%c%c%c%c%c%c",\
										uart_buff[30],uart_buff[31],uart_buff[32],\
										uart_buff[33],uart_buff[34],
										uart_buff[35], // dot
										uart_buff[36],uart_buff[37],uart_buff[38],uart_buff[39],uart_buff[40]);
			GPS_Msg.latitude_val = 0.01* atof((const char *)GPS_Msg.latitude); //纬度值
			GPS_Msg.longitude_val = 0.01* atof((const char *)GPS_Msg.longitude); //纬度值
			
//			GPS_Msg.latitude_val = 10*(uart_buff[17]-0x30)+uart_buff[18]-0x30 +\
//							   (1e6*(uart_buff[19]-0x30) + 1e5*(uart_buff[20]-0x30) +\
//								1e4*(uart_buff[22]-0x30) + 1e3*(uart_buff[23]-0x30) +\
//								1e2*(uart_buff[24]-0x30)  + 1e1*(uart_buff[25]-0x30) +(uart_buff[26]-0x30))/6e6;
//								
//			GPS_Msg.longitude_val = 100*(uart_buff[30]-0x30)+10*(uart_buff[31]-0x30)+uart_buff[32]-0x30 +\
//								(1e6*(uart_buff[33]-0x30) + 1e5*(uart_buff[34]-0x30) +\
//								1e4*(uart_buff[36]-0x30) + 1e3*(uart_buff[37]-0x30) +\
//								1e2*(uart_buff[38]-0x30)  + 1e1*(uart_buff[39]-0x30) +(uart_buff[40]-0x30))/6e6;
								
			sprintf((char *)&GPS_Msg.altitude,"%c%c%c%c",\
														uart_buff[54],uart_buff[55],\
													uart_buff[56],uart_buff[57]/*,uart_buff[59]*/);
			
			GPS_Msg.altitude_val = atof((const char *)GPS_Msg.altitude); //海拔高度
			memset(GPS_Msg.gpssta,0,sizeof(GPS_Msg.gpssta));
			if( (uart_buff[44] - 0x30) == 0)
				strcpy(GPS_Msg.gpssta,"Not Positioned");
			else
				strcpy(GPS_Msg.gpssta,"Positioned");
				
			GPS_Msg.svnum = 10*(uart_buff[46]-0x30) + (uart_buff[47]-0x30);
			
			//prt("定位状态;%u \r\n",GPS_Msg.gpssta);
			//prt("可见卫星数;%u \r\n",GPS_Msg.svnum);
			//prt("海拔：%s\r\n",GPS_Msg.altitude);
			
			
			//prt("纬度：%c %lf \r\n",GPS_Msg.nshemi,GPS_Msg.latitude_val);
			//prt("经度：%c %lf \r\n",GPS_Msg.ewhemi,GPS_Msg.longitude_val);		
			
			Flag_GPGGA_OK = 0;
			GPGGA_OK_FLAG = 1;				
			OS_CRITICAL_EXIT();	
			//获取GPRMC 数据		
			while(Flag_GPRMC_OK != 1);
			OS_CRITICAL_ENTER();
			prt("GPRMC OK ");
			GPS_Msg.utc.hour = 10*(uart_buff[7]-0x30) + (uart_buff[8]-0x30);
			GPS_Msg.utc.min = 10*(uart_buff[9]-0x30) + (uart_buff[10]-0x30);
			GPS_Msg.utc.sec = 10*(uart_buff[11]-0x30) + (uart_buff[12]-0x30);
			GPS_Msg.utc.year = 10*(uart_buff[57]-0x30) + (uart_buff[58]-0x30);
			GPS_Msg.utc.month = 10*(uart_buff[55]-0x30) + (uart_buff[56]-0x30);
			GPS_Msg.utc.date = 10*(uart_buff[53]-0x30) + (uart_buff[54]-0x30);
		
			sprintf((char *)&GPS_Msg.speed,"%c%c%c%c%c",uart_buff[46],uart_buff[47],
										uart_buff[48],uart_buff[49],
										uart_buff[50]);
			GPS_Msg.speed_val = 1.852 *atof((const char *)GPS_Msg.speed); //节转KM/H
										
			GMTconvert(&GPS_Msg.utc,&GPS_Msg.LocalTime,8,1);	
			Flag_GPRMC_OK = 0;
			GPRMC_OK_FLAG = 1;			
			OS_CRITICAL_EXIT();
		//prt("20%u年 %u月 %u日  \r\n",GPS_Msg.LocalTime.year,GPS_Msg.LocalTime.month,GPS_Msg.LocalTime.date);
		//prt("%u时 %u分 %u秒 \r\n",GPS_Msg.LocalTime.hour,GPS_Msg.LocalTime.min,GPS_Msg.LocalTime.sec);
		prt("速度：%s \r\n",GPS_Msg.speed);	
		prt("速度值：%.3f \r\n",GPS_Msg.speed_val);
		
}
/******************************************************************************************************** 
**     函数名称:         GPS_GetPos
**    功能描述:          GPS定位处理函数，得到站点
**    入口参数：       	double latitude,double longitude,double busline[POS_NUM][2]
**    出口参数:         u8
**    其他说明：       传入当前经纬度和路线全部站点组成的经纬度二维数组，返回对应站点值，如果定位点不是站点将返回0
********************************************************************************************************/ 
  u8 GPS_GetPos(double latitude,double longitude,const double busline[POS_NUM][2])    //海大四饭到校门口
  {
		u8 i;
		for(i = 0; i< POS_NUM; i++)
		{
			if(latitude> (busline[i][0]-PRECISION) &&latitude<(busline[i][0]+PRECISION) &&\
					longitude>(busline[i][1]-PRECISION) && longitude< (busline[i][1]+PRECISION) )
				return i+1;
			else
				continue;
		}
		return 0;
		
  }

/******************************************************************************************************** 
**     函数名称:            bit        IsLeapYear(uint8_t    iYear) 
**    功能描述:            判断闰年(仅针对于2000以后的年份) 
**    入口参数：            iYear    两位年数 
**    出口参数:            uint8_t        1:为闰年    0:为平年 
**    其他说明：    		 无
********************************************************************************************************/ 
 u8 IsLeapYear(u8 iYear) 
{ 
    uint16_t    Year; 
    Year    =    2000+iYear; 
    if((Year&3)==0) 
    { 
        return ((Year%400==0) || (Year%100!=0)); 
    } 
     return 0; 
} 

/******************************************************************************************************** 
**     函数名称:            void    GMTconvert(uint8_t *DT,uint8_t GMT,uint8_t AREA) 
**    功能描述:            格林尼治时间换算世界各时区时间 
**    入口参数：            *DT:    表示日期时间的数组 格式 YY,MM,DD,HH,MM,SS 
**                        GMT:    时区数 
**                        AREA:    1(+)东区 W0(-)西区 
********************************************************************************************************/ 
void    GMTconvert(nmea_utc_time *SourceTime, nmea_utc_time *ConvertTime, uint8_t GMT,uint8_t AREA) 
{ 
    uint32_t    YY,MM,DD,hh,mm,ss;        //年月日时分秒暂存变量 
     
    if(GMT==0)    return;                //如果处于0时区直接返回 
    if(GMT>12)    return;                //时区最大为12 超过则返回         

    YY    =    SourceTime->year;                //获取年 
    MM    =    SourceTime->month;                 //获取月 
    DD    =    SourceTime->date;                 //获取日 
    hh    =    SourceTime->hour;                //获取时 
    mm    =    SourceTime->min;                 //获取分 
    ss    =    SourceTime->sec;                 //获取秒 

    if(AREA)                        //东(+)时区处理 
    { 
        if(hh+GMT<24)    hh    +=    GMT;//如果与格林尼治时间处于同一天则仅加小时即可 
        else                        //如果已经晚于格林尼治时间1天则进行日期处理 
        { 
            hh    =    hh+GMT-24;        //先得出时间 
            if(MM==1 || MM==3 || MM==5 || MM==7 || MM==8 || MM==10)    //大月份(12月单独处理) 
            { 
                if(DD<31)    DD++; 
                else 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
            } 
            else if(MM==4 || MM==6 || MM==9 || MM==11)                //小月份2月单独处理) 
            { 
                if(DD<30)    DD++; 
                else 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
            } 
            else if(MM==2)    //处理2月份 
            { 
                if((DD==29) || (DD==28 && IsLeapYear(YY)==0))        //本来是闰年且是2月29日 或者不是闰年且是2月28日 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
                else    DD++; 
            } 
            else if(MM==12)    //处理12月份 
            { 
                if(DD<31)    DD++; 
                else        //跨年最后一天 
                {               
                    DD    =    1; 
                    MM    =    1; 
                    YY    ++; 
                } 
            } 
        } 
    } 
    else 
    {     
        if(hh>=GMT)    hh    -=    GMT;    //如果与格林尼治时间处于同一天则仅减小时即可 
        else                        //如果已经早于格林尼治时间1天则进行日期处理 
        { 
            hh    =    hh+24-GMT;        //先得出时间 
            if(MM==2 || MM==4 || MM==6 || MM==8 || MM==9 || MM==11)    //上月是大月份(1月单独处理) 
            { 
                if(DD>1)    DD--; 
                else 
                { 
                    DD    =    31; 
                    MM    --; 
                } 
            } 
            else if(MM==5 || MM==7 || MM==10 || MM==12)                //上月是小月份2月单独处理) 
            { 
                if(DD>1)    DD--; 
                else 
                { 
                    DD    =    30; 
                    MM    --; 
                } 
            } 
            else if(MM==3)    //处理上个月是2月份 
            { 
                if((DD==1) && IsLeapYear(YY)==0)                    //不是闰年 
                { 
                    DD    =    28; 
                    MM    --; 
                } 
                else    DD--; 
            } 
            else if(MM==1)    //处理1月份 
            { 
                if(DD>1)    DD--; 
                else        //新年第一天 
                {               
                    DD    =    31; 
                    MM    =    12; 
                    YY    --; 
                } 
            } 
        } 
    }         

    ConvertTime->year   =    YY;                //更新年 
    ConvertTime->month    =    MM;                //更新月 
    ConvertTime->date    =    DD;                //更新日 
    ConvertTime->hour   =    hh;                //更新时 
    ConvertTime->min    =    mm;                //更新分 
    ConvertTime->sec    =    ss;                //更新秒 
}  



/*********************************************************end of file**************************************************/
