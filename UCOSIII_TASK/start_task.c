/*
	�ļ�����      start_task.c
	�ļ������� ϵͳ����ʼ����
	�޸�ʱ�䣺 2019��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/

#include "start_task.h"
#include "task_config.h"
#include "usart.h"
#include "key_task.h"
#include "ISD4004_task.h"
#include "ISD4004_recordtask.h"
#include "DHT11_task.h"
#include "OLED_task.h"
#include "gettime_task.h"
#include "other_task.h"
#include "wdg.h"

OS_FLAG_GRP isd4004event_flag;
OS_FLAG_GRP oledevent_flag;
OS_SEM  sem_key_time;
OS_SEM  sem_oledandkey;
OS_SEM  sem_othertask;
OS_SEM  sem_isd4004task;
OS_SEM  sem_arrive;
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];

/******************************************************************************************************** 
**     ��������:         start_task
**    ��������:          ��ʼ������
**    ��ڲ�����       	 void *p_arg
**    ���ڲ���:          ��
********************************************************************************************************/ 
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	CPU_Init();
#if 0
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif
#endif	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	OS_CRITICAL_ENTER();	//�����ٽ���
	prt("start_task  start \r\n");    
	
	OSFlagCreate((OS_FLAG_GRP *)&isd4004event_flag,        //isd4004�¼���־�飬���𲥱������¼���־
								(CPU_CHAR *)"isd4004event_flag ",
								(OS_FLAGS)0x00,
								(OS_ERR *)&err
								);
	OSFlagCreate((OS_FLAG_GRP *)&oledevent_flag,        //oled�¼�����־�飬����oled������ʾ��Ϣ�¼���־
								(CPU_CHAR *)"oledevent_flag ",
								(OS_FLAGS)0x00,
								(OS_ERR *)&err
								);
	
	OSSemCreate(&sem_key_time,"sem_key_time",1,&err);  // ���������ź���
	OSSemCreate(&sem_oledandkey,"sem_oledandkey",1,&err);  //��������ʾͬ��
	OSSemCreate(&sem_othertask,"sem_othertask",0,&err);   //OLED��������֮���ͬ��
	OSSemCreate(&sem_isd4004task,"sem_isd4004task",0,&err);   //ISD4004��������֮���ͬ��
	OSSemCreate(&sem_arrive,"sem_arrive",0,&err);   //��վ�����������֮���ͬ��
#if GETTIMETaskTCB_CONFIG
	OSTaskCreate((OS_TCB 	* )&GETTIMETaskTCB,		   //��ȡʱ��
			 (CPU_CHAR	* )"GETTIME task", 		
							 (OS_TASK_PTR )GETTIME_task, 			
							 (void		* )0,					
							 (OS_PRIO	  )GETTIME_TASK_PRIO,     
							 (CPU_STK   * )&GETTIME_TASK_STK[0],	
							 (CPU_STK_SIZE)GETTIME_STK_SIZE/10,	
							 (CPU_STK_SIZE)GETTIME_STK_SIZE,		
							 (OS_MSG_QTY  )0,					
							 (OS_TICK	  )0,					
							 (void   	* )0,					
							 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
							 (OS_ERR 	* )&err);	
#endif

#if KEYTaskTCB_CONFIG
	OSTaskCreate((OS_TCB 	* )&KEYTaskTCB,		      //������վ����
				 (CPU_CHAR	* )"KEY task", 		
								 (OS_TASK_PTR )KEY_task, 			
								 (void		* )0,					
								 (OS_PRIO	  )KEY_TASK_PRIO,     
								 (CPU_STK   * )&KEY_TASK_STK[0],	
								 (CPU_STK_SIZE)KEY_STK_SIZE/10,	
								 (CPU_STK_SIZE)KEY_STK_SIZE,		
								 (OS_MSG_QTY  )0,					
								 (OS_TICK	  )0,					
								 (void   	* )0,					
								 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
								 (OS_ERR 	* )&err);									 
#endif


#if OLEDTaskTCB_CONFIG				 
				 OSTaskCreate((OS_TCB 	* )&OLEDTaskTCB,		//��ʾ����
				 (CPU_CHAR	* )"OLED task", 		
								 (OS_TASK_PTR )OLED_task, 			
								 (void		* )0,					
								 (OS_PRIO	  )OLED_TASK_PRIO,     
								 (CPU_STK   * )&OLED_TASK_STK[0],	
								 (CPU_STK_SIZE)OLED_STK_SIZE/10,	
								 (CPU_STK_SIZE)OLED_STK_SIZE,		
								 (OS_MSG_QTY  )0,					
								 (OS_TICK	  )0,					
								 (void   	* )0,					
								 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
								 (OS_ERR 	* )&err);	
#endif

#if ISD4004TaskTCB_CONFIG				 
				 OSTaskCreate((OS_TCB 	* )&ISD4004TaskTCB,		//������������
				 (CPU_CHAR	* )"ISD4004 task", 		
								 (OS_TASK_PTR )ISD4004_task, 			
								 (void		* )0,					
								 (OS_PRIO	  )ISD4004_TASK_PRIO,     
								 (CPU_STK   * )&ISD4004_TASK_STK[0],	
								 (CPU_STK_SIZE)ISD4004_STK_SIZE/10,	
								 (CPU_STK_SIZE)ISD4004_STK_SIZE,		
								 (OS_MSG_QTY  )0,					
								 (OS_TICK	  )0,					
								 (void   	* )0,					
								 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
								 (OS_ERR 	* )&err);	
#endif

#if ISD4004_RECORDTaskTCB_CONFIG
OSTaskCreate((OS_TCB 	* )&ISD4004_RECORDTaskTCB,		 // ¼������
				 (CPU_CHAR	* )"ISD4004_RECORD task", 		
								 (OS_TASK_PTR )ISD4004_RECORD_task, 			
								 (void		* )0,					
								 (OS_PRIO	  )ISD4004_RECORD_TASK_PRIO,     
								 (CPU_STK   * )&ISD4004_RECORD_TASK_STK[0],	
								 (CPU_STK_SIZE)ISD4004_RECORD_STK_SIZE/10,	
								 (CPU_STK_SIZE)ISD4004_RECORD_STK_SIZE,		
								 (OS_MSG_QTY  )0,					
								 (OS_TICK	  )0,					
								 (void   	* )0,					
								 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
								 (OS_ERR 	* )&err);	
#endif

#if DHT11TaskTCB_CONFIG
				 OSTaskCreate((OS_TCB 	* )&DHT11TaskTCB,		//��ʪ�Ȳɼ�
				 (CPU_CHAR	* )"DHT11 task", 		
								 (OS_TASK_PTR )DHT11_task, 			
								 (void		* )0,					
								 (OS_PRIO	  )DHT11_TASK_PRIO,     
								 (CPU_STK   * )&DHT11_TASK_STK[0],	
								 (CPU_STK_SIZE)DHT11_STK_SIZE/10,	
								 (CPU_STK_SIZE)DHT11_STK_SIZE,		
								 (OS_MSG_QTY  )0,					
								 (OS_TICK	  )0,					
								 (void   	* )0,					
								 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
								 (OS_ERR 	* )&err);	
#endif


#if OTHERTaskTCB_CONFIG
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
#endif

	//OSTaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//���ٽ���
	//��ʼ����ʾ����
	IWDG_Init(); //��ʼ���������Ź�
	while(1)
	{
		IWDG_Feed();
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms
	}
}

