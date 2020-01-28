/*
	�ļ�����      other_task.c
	�ļ������� other_task������ʵ��
	�޸�ʱ�䣺 2019��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/
#include "other_task.h"
#include "sys.h"
#include "usart.h"
#include "start_task.h"
#include "oled_task.h"
#include "isd4004_task.h"
#include "dht11_task.h"
#include "gettime_task.h"
//������ƿ�
OS_TCB OTHERTaskTCB;
//�����ջ	
CPU_STK OTHER_TASK_STK[OTHER_STK_SIZE];

u8 syc ;
/******************************************************************************************************** 
**     ��������:        OTHER_task
**    ��������:         //OTHER������
**    ��ڲ�����       (void *p_arg)
**    ���ڲ���:          ��
**    ����˵����       ��
********************************************************************************************************/ 
void OTHER_task(void *p_arg)
{
	u8 i = 0;
	OS_ERR err;
	p_arg = p_arg;
	prt("OTHER_task  start \r\n");
	while(1)
	{
		if(i<6)
		{
			OSFlagPost(&oledevent_flag,FLAG_BIT(OLED_OTHER1_FLAG),OS_OPT_POST_FLAG_SET,&err);
			OSSemPend(&sem_othertask,0,OS_OPT_PEND_BLOCKING,NULL,&err);	
			OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s					
			syc = 1;
			i++;
			if(i == 1 )
			{
				if(direction == POSITIVE_DIRCT)
					OSFlagPost(&isd4004event_flag,FLAG_BIT(ISD4004_POSITIVE_DIRECTION_FLAG),OS_OPT_POST_FLAG_SET,&err);
				else if(direction == REVERSE_DIRCT)
					OSFlagPost(&isd4004event_flag,FLAG_BIT(ISD4004_REVERSE_DIRECTION_FLAG),OS_OPT_POST_FLAG_SET,&err);
				
			}
		}
		else
		{	syc = 0;
			OSSemPend(&sem_isd4004task,0,OS_OPT_PEND_BLOCKING,NULL,&err);
			if(DHT11_OK_FLAG)
			{	//��ʪ�ȡ������˳�
				OSFlagPost(&oledevent_flag,FLAG_BIT(OLED_OTHER2_FLAG),OS_OPT_POST_FLAG_SET,&err);
				OSSemPend(&sem_othertask,0,OS_OPT_PEND_BLOCKING,NULL,&err);
				OSTimeDlyHMSM(0,0,5,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
				DHT11_OK_FLAG = 0;
			}
			//������Ϣ
			OSTimeDlyHMSM(0,0,1,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
			OSFlagPost(&oledevent_flag,FLAG_BIT(OLED_OTHER3_FLAG),OS_OPT_POST_FLAG_SET,&err);
			OSFlagPost(&isd4004event_flag,FLAG_BIT(ISD4004_OTHER_FLAG),OS_OPT_POST_FLAG_SET,&err);
			OSSemPend(&sem_othertask,0,OS_OPT_PEND_BLOCKING,NULL,&err);
			OSSemPend(&sem_isd4004task,0,OS_OPT_PEND_BLOCKING,NULL,&err);
			OSTimeDlyHMSM(0,0,5,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
			
			//�೵��Ϣ
			OSFlagPost(&oledevent_flag,FLAG_BIT(OLED_OTHER4_FLAG),OS_OPT_POST_FLAG_SET,&err);
			OSSemPend(&sem_othertask,0,OS_OPT_PEND_BLOCKING,NULL,&err);
			OSTimeDlyHMSM(0,0,8,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
			i = 0;
		}		
					
	}
}
/******************************************************************************************************** 
**     ��������:         ReCreateOTHERTask
**    ��������:         ���´���OTHERTask����
**    ��ڲ�����       ��
**    ���ڲ���:          ��
**    ����˵����       ��
********************************************************************************************************/ 
void ReCreateOTHERTask(void)
{
	OS_ERR err;
	OSTaskCreate((OS_TCB 	* )&OTHERTaskTCB,		//��������
				 (CPU_CHAR	* )"OTHER task", 		
								 (OS_TASK_PTR )OTHER_task, 			
								 (void		* )0,					
								 (OS_PRIO	  )OTHER_TASK_PRIO,     
								 (CPU_STK   * )&OTHER_TASK_STK[0],	
								 (CPU_STK_SIZE)OTHER_STK_SIZE/10,	
								 (CPU_STK_SIZE)OTHER_STK_SIZE,		
								 (OS_MSG_QTY  )0,					
								 (OS_TICK	  )0,					
								 (void   	* )0,					
								 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
								 (OS_ERR 	* )&err);	
}