/*
	�ļ�����      GPS_DATADEAL_task.c
	�ļ������� GPS_DATADEAL_task������ʵ��
	�޸�ʱ�䣺 2019��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/
#include "GPS_datadeal_task.h"
#include "sys.h"
#include "usart.h"
#include "gps.h"
#include "isd4004_task.h"
#include "oled_task.h"
#include "start_task.h"

//������ƿ�
OS_TCB GPS_DATADEALTaskTCB;
//�����ջ	
CPU_STK GPS_DATADEAL_TASK_STK[GPS_DATADEAL_STK_SIZE];


/******************************************************************************************************** 
**     ��������:         GPS_DATADEAL_task
**    ��������:         GPS_DATADEAL������
**    ��ڲ�����       (void *p_arg)
**    ���ڲ���:          ��
**    ����˵����       ��
********************************************************************************************************/ 
void GPS_DATADEAL_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	prt("GPS_DATADEAL  start \r\n");
	//GPS_Init();
	while(1)
	{
		prt("GPS_DATADEAL 12345678 \r\n ");
		GPS_AnalysisData();
		OS_CRITICAL_ENTER(); //�ٽ���
		if(GPS_Msg.speed_val > SPEEDING_VALUE)
		{
			OSFlagPost(&oledevent_flag,FLAG_BIT(OLED_SPEEDING_FLAG),OS_OPT_POST_FLAG_SET,&err);
			OSFlagPost(&isd4004event_flag,FLAG_BIT(ISD4004_SPEEDING_FLAG),OS_OPT_POST_FLAG_SET,&err);
		}
			
		OS_CRITICAL_EXIT();
		OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ200ms
	}
}

	
	
