/*
	�ļ�����      GPS.c
	�ļ������� STM32F03C8T6---GPS Driver
	�޸�ʱ�䣺 2018��4��21��
	���ߣ�	     ������
	����˵���� ��
	
*/
#ifndef __GPS_H_
#define	__GPS_H_

#include "sys.h"
#include "gps_usart2.h"

/*******************GPS NMEA-0183Э����Ҫ�����ṹ�嶨�� ******************************************/

//UTCʱ����Ϣ
__packed typedef struct  
{										    
 	u16 year;	//���
	u8 month;	//�·�
	u8 date;	//����
	u8 hour; 	//Сʱ
	u8 min; 	//����
	u8 sec; 	//����
}nmea_utc_time;   	   
//NMEA 0183 Э����������ݴ�Žṹ��
__packed typedef struct  
{										    
 	u8 svnum;					//�ɼ�GPS������
	nmea_utc_time utc;			//UTCʱ��
	nmea_utc_time LocalTime;		//����ʱ��
	double latitude_val;				//γ�� 
	u8 latitude[11];				//γ�� 
	u8 nshemi;					//��γ/	  
	double longitude_val;			    //���� 
	u8 longitude[12];			    //���� 
	u8 ewhemi;					//����/����,E:����;W:����
	char gpssta[15];					//GPS״̬:0,δ��λ;1,�ǲ�ֶ�λ;2,��ֶ�λ;6,���ڹ���.				  
	u8 altitude[5];			 	//���θ߶�
	double altitude_val;
	u8 speed[6];					//��������
	double speed_val;
}nmea_msg;
/*****************************************  end  ***************************************************/
extern vu8 GPGGA_OK_FLAG;
extern vu8 GPRMC_OK_FLAG ;

extern nmea_msg GPS_Msg;  /*����GPS��Ϣ����*/
#define GPS_Init GPS_USART_Init

#define POS_NUM 9			//վ����Ŀ
#define PRECISION 0.00005  // ��λ����
u8 IsLeapYear(u8 iYear) ;//�ж�ƽ����
void GPS_GetTime(void);
void GPS_AnalysisData(void);
u8 GPS_GetPos(double latitude,double longitude,const double busline[POS_NUM][2]) ;

#endif 
