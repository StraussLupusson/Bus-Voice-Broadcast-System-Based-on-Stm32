/*
	�ļ�����      gps.c
	�ļ������� STM32F03C8T6---GPS_Device Driver
	�޸�ʱ�䣺 2018��4��29��
	���ߣ�	     ������
	����˵���� ��
	
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

nmea_msg GPS_Msg;  /*����GPS��Ϣ����*/
static CPU_SR_ALLOC();
static void    GMTconvert(nmea_utc_time *SourceTime, nmea_utc_time *ConvertTime, uint8_t GMT,uint8_t AREA) ;
/******************************************************************************************************** 
**     ��������:         GPS_GetTime
**    ��������:          ����GPSģ����յ���ʱ�����ݣ������浽ȫ��GPS_Msg�ṹ��
**    ��ڲ�����       ��
**    ���ڲ���:        ��
**    ����˵����       ������
********************************************************************************************************/ 
void GPS_GetTime(void)
{
		//��ȡGPRMC ʱ������
		delay_us(5);
		while(Flag_time_OK != 1 );
		OS_CRITICAL_ENTER();
		//��ȡUTCʱ��
		GPS_Msg.utc.hour = 10*(uart_buff[7]-0x30) + (uart_buff[8]-0x30);
		GPS_Msg.utc.min = 10*(uart_buff[9]-0x30) + (uart_buff[10]-0x30);
		GPS_Msg.utc.sec = 10*(uart_buff[11]-0x30) + (uart_buff[12]-0x30);
		GPS_Msg.utc.year = 10*(uart_buff[29]-0x30) + (uart_buff[30]-0x30);
		GPS_Msg.utc.month = 10*(uart_buff[27]-0x30) + (uart_buff[28]-0x30);
		GPS_Msg.utc.date = 10*(uart_buff[25]-0x30) + (uart_buff[26]-0x30);
		GMTconvert(&GPS_Msg.utc,&GPS_Msg.LocalTime,8,1);	//ת����ʱ��
		Flag_time_OK = 0;
		
		OS_CRITICAL_EXIT();
		
		
		
}

/******************************************************************************************************** 
**     ��������:         GPS_AnalysisData
**    ��������:          ����GPSģ����յ������ݣ������浽ȫ��GPS_Msg�ṹ��
**    ��ڲ�����       ��
**    ���ڲ���:        ��
**    ����˵����       ������
********************************************************************************************************/ 
void GPS_AnalysisData(void)
{
			//��ȡGPGGA ����
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
			GPS_Msg.latitude_val = 0.01* atof((const char *)GPS_Msg.latitude); //γ��ֵ
			GPS_Msg.longitude_val = 0.01* atof((const char *)GPS_Msg.longitude); //γ��ֵ
			
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
			
			GPS_Msg.altitude_val = atof((const char *)GPS_Msg.altitude); //���θ߶�
			memset(GPS_Msg.gpssta,0,sizeof(GPS_Msg.gpssta));
			if( (uart_buff[44] - 0x30) == 0)
				strcpy(GPS_Msg.gpssta,"Not Positioned");
			else
				strcpy(GPS_Msg.gpssta,"Positioned");
				
			GPS_Msg.svnum = 10*(uart_buff[46]-0x30) + (uart_buff[47]-0x30);
			
			//prt("��λ״̬;%u \r\n",GPS_Msg.gpssta);
			//prt("�ɼ�������;%u \r\n",GPS_Msg.svnum);
			//prt("���Σ�%s\r\n",GPS_Msg.altitude);
			
			
			//prt("γ�ȣ�%c %lf \r\n",GPS_Msg.nshemi,GPS_Msg.latitude_val);
			//prt("���ȣ�%c %lf \r\n",GPS_Msg.ewhemi,GPS_Msg.longitude_val);		
			
			Flag_GPGGA_OK = 0;
			GPGGA_OK_FLAG = 1;				
			OS_CRITICAL_EXIT();	
			//��ȡGPRMC ����		
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
			GPS_Msg.speed_val = 1.852 *atof((const char *)GPS_Msg.speed); //��תKM/H
										
			GMTconvert(&GPS_Msg.utc,&GPS_Msg.LocalTime,8,1);	
			Flag_GPRMC_OK = 0;
			GPRMC_OK_FLAG = 1;			
			OS_CRITICAL_EXIT();
		//prt("20%u�� %u�� %u��  \r\n",GPS_Msg.LocalTime.year,GPS_Msg.LocalTime.month,GPS_Msg.LocalTime.date);
		//prt("%uʱ %u�� %u�� \r\n",GPS_Msg.LocalTime.hour,GPS_Msg.LocalTime.min,GPS_Msg.LocalTime.sec);
		prt("�ٶȣ�%s \r\n",GPS_Msg.speed);	
		prt("�ٶ�ֵ��%.3f \r\n",GPS_Msg.speed_val);
		
}
/******************************************************************************************************** 
**     ��������:         GPS_GetPos
**    ��������:          GPS��λ���������õ�վ��
**    ��ڲ�����       	double latitude,double longitude,double busline[POS_NUM][2]
**    ���ڲ���:         u8
**    ����˵����       ���뵱ǰ��γ�Ⱥ�·��ȫ��վ����ɵľ�γ�ȶ�ά���飬���ض�Ӧվ��ֵ�������λ�㲻��վ�㽫����0
********************************************************************************************************/ 
  u8 GPS_GetPos(double latitude,double longitude,const double busline[POS_NUM][2])    //�����ķ���У�ſ�
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
**     ��������:            bit        IsLeapYear(uint8_t    iYear) 
**    ��������:            �ж�����(�������2000�Ժ�����) 
**    ��ڲ�����            iYear    ��λ���� 
**    ���ڲ���:            uint8_t        1:Ϊ����    0:Ϊƽ�� 
**    ����˵����    		 ��
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
**     ��������:            void    GMTconvert(uint8_t *DT,uint8_t GMT,uint8_t AREA) 
**    ��������:            ��������ʱ�任�������ʱ��ʱ�� 
**    ��ڲ�����            *DT:    ��ʾ����ʱ������� ��ʽ YY,MM,DD,HH,MM,SS 
**                        GMT:    ʱ���� 
**                        AREA:    1(+)���� W0(-)���� 
********************************************************************************************************/ 
void    GMTconvert(nmea_utc_time *SourceTime, nmea_utc_time *ConvertTime, uint8_t GMT,uint8_t AREA) 
{ 
    uint32_t    YY,MM,DD,hh,mm,ss;        //������ʱ�����ݴ���� 
     
    if(GMT==0)    return;                //�������0ʱ��ֱ�ӷ��� 
    if(GMT>12)    return;                //ʱ�����Ϊ12 �����򷵻�         

    YY    =    SourceTime->year;                //��ȡ�� 
    MM    =    SourceTime->month;                 //��ȡ�� 
    DD    =    SourceTime->date;                 //��ȡ�� 
    hh    =    SourceTime->hour;                //��ȡʱ 
    mm    =    SourceTime->min;                 //��ȡ�� 
    ss    =    SourceTime->sec;                 //��ȡ�� 

    if(AREA)                        //��(+)ʱ������ 
    { 
        if(hh+GMT<24)    hh    +=    GMT;//������������ʱ�䴦��ͬһ�������Сʱ���� 
        else                        //����Ѿ����ڸ�������ʱ��1����������ڴ��� 
        { 
            hh    =    hh+GMT-24;        //�ȵó�ʱ�� 
            if(MM==1 || MM==3 || MM==5 || MM==7 || MM==8 || MM==10)    //���·�(12�µ�������) 
            { 
                if(DD<31)    DD++; 
                else 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
            } 
            else if(MM==4 || MM==6 || MM==9 || MM==11)                //С�·�2�µ�������) 
            { 
                if(DD<30)    DD++; 
                else 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
            } 
            else if(MM==2)    //����2�·� 
            { 
                if((DD==29) || (DD==28 && IsLeapYear(YY)==0))        //��������������2��29�� ���߲�����������2��28�� 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
                else    DD++; 
            } 
            else if(MM==12)    //����12�·� 
            { 
                if(DD<31)    DD++; 
                else        //�������һ�� 
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
        if(hh>=GMT)    hh    -=    GMT;    //������������ʱ�䴦��ͬһ�������Сʱ���� 
        else                        //����Ѿ����ڸ�������ʱ��1����������ڴ��� 
        { 
            hh    =    hh+24-GMT;        //�ȵó�ʱ�� 
            if(MM==2 || MM==4 || MM==6 || MM==8 || MM==9 || MM==11)    //�����Ǵ��·�(1�µ�������) 
            { 
                if(DD>1)    DD--; 
                else 
                { 
                    DD    =    31; 
                    MM    --; 
                } 
            } 
            else if(MM==5 || MM==7 || MM==10 || MM==12)                //������С�·�2�µ�������) 
            { 
                if(DD>1)    DD--; 
                else 
                { 
                    DD    =    30; 
                    MM    --; 
                } 
            } 
            else if(MM==3)    //�����ϸ�����2�·� 
            { 
                if((DD==1) && IsLeapYear(YY)==0)                    //�������� 
                { 
                    DD    =    28; 
                    MM    --; 
                } 
                else    DD--; 
            } 
            else if(MM==1)    //����1�·� 
            { 
                if(DD>1)    DD--; 
                else        //�����һ�� 
                {               
                    DD    =    31; 
                    MM    =    12; 
                    YY    --; 
                } 
            } 
        } 
    }         

    ConvertTime->year   =    YY;                //������ 
    ConvertTime->month    =    MM;                //������ 
    ConvertTime->date    =    DD;                //������ 
    ConvertTime->hour   =    hh;                //����ʱ 
    ConvertTime->min    =    mm;                //���·� 
    ConvertTime->sec    =    ss;                //������ 
}  



/*********************************************************end of file**************************************************/
