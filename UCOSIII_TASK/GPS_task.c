/*
	�ļ�����      GPS_task.c
	�ļ������� GPS_task������ʵ��
	�޸�ʱ�䣺 2019��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/
#include "GPS_task.h"
#include "sys.h"
#include "usart.h"
#include "gps.h"
#include "start_task.h"
#include "ISD4004.h"
#include "busposmesg.h"
#include "key_task.h"
//������ƿ�
OS_TCB GPSTaskTCB;
//�����ջ	
CPU_STK GPS_TASK_STK[GPS_STK_SIZE];

/******************************************************************************************************** 
**     ��������:         GPS_task
**    ��������:          GPS������
**    ��ڲ�����       void *p_arg
**    ���ڲ���:          ��
**    ����˵����       ��
********************************************************************************************************/ 
void GPS_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	u8 pos = 0;
	p_arg = p_arg;
	printf("gps  start \r\n");
	GPS_Init(); 
	while(1)
	{
		GPS_AnalysisData();
		OS_CRITICAL_ENTER();
		if(GPGGA_OK_FLAG)
		{
			pos = GPS_GetPos(GPS_Msg.latitude_val,GPS_Msg.longitude_val,pos_gpsmesg);
		OS_CRITICAL_EXIT();
			if(pos != 0)
			{
				prt("ready OSFlagPost to ISD4004");
				if(key_time != pos)
					Alter_key_time(&key_time,pos);   // ����Ⱦ͸��°���վ��ֵ
				ISD4004_StopPlaying();
				OSFlagPost(&oledevent_flag,FLAG_BIT(pos-1),OS_OPT_POST_FLAG_SET,&err);
				OSFlagPost(&isd4004event_flag,FLAG_BIT(pos-1),OS_OPT_POST_FLAG_SET,&err);
				OSTimeDlyHMSM(0,0,15,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ200ms
				pos = 0;
			}
			OS_CRITICAL_ENTER();
			GPGGA_OK_FLAG = 0;
			OS_CRITICAL_EXIT();
		}
		OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ200ms
		
	}
}

	
	
