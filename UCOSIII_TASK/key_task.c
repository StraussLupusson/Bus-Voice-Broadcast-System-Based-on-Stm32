/*
	�ļ�����      key_task.c
	�ļ������� key_task����ʵ��
	�޸�ʱ�䣺 2019��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/
#include "key_task.h"
#include "sys.h"
#include "usart.h"
#include "start_task.h"
#include "key.h"
#include "ISD4004.h"
#include "remote.h"
//������ƿ�
OS_TCB KEYTaskTCB;
//�����ջ	
CPU_STK KEY_TASK_STK[KEY_STK_SIZE];

vu8 key_time = 1;
/******************************************************************************************************** 
**     ��������:         KEY_task
**    ��������:          ����������
**    ��ڲ�����       (void *p_arg)
**    ���ڲ���:          ��
**    ����˵����       ��
********************************************************************************************************/ 
void KEY_task(void *p_arg)
{
	vu8 key = 0,remotekey = 0;
	vu8 key_cout = 0,key_time1 = 1;
	OS_ERR err; 
	p_arg = p_arg;
	prt("KEY_task  start \r\n");
	
	KEY_Init();
	Remote_Init();
	while(1)
	{
		key = KEY_Scan();
		remotekey = Remote_Scan(SINGLE_PRES);
		if(remotekey)key = remotekey;
		if(key)
		{
			OSSemPend(&sem_oledandkey,0,OS_OPT_PEND_BLOCKING,NULL,&err);//ȷ����ʾ�����в��ܰ���
			if(key == KEY0_PRES || key == LEFT)//ǰһվ
			{
				if(!key_cout) //�״ΰ��°���
					key_time1 = key_time;//���水������
				if(key_time>1)
				{
					Alter_key_time(&key_time,key_time-1);
					OSFlagPost(&oledevent_flag,FLAG_BIT(key_time+POS_NUM),OS_OPT_POST_FLAG_SET,&err);	
				}else {
					Alter_key_time(&key_time,9);
					OSFlagPost(&oledevent_flag,FLAG_BIT(key_time+POS_NUM),OS_OPT_POST_FLAG_SET,&err);	
				}
				prt("key_time:%u \r\n",key_time);
				key_cout++;	
			}
			else if(key == KEY1_PRES || key == RIGHT)  //��һվ
			{
				if(!key_cout)//�״ΰ��°���
					key_time1 = key_time;
				if(key_time<POS_NUM)
				{
					Alter_key_time(&key_time,key_time+1);
					OSFlagPost(&oledevent_flag,FLAG_BIT(key_time+POS_NUM),OS_OPT_POST_FLAG_SET,&err);		
				}else {
					Alter_key_time(&key_time,1);
					OSFlagPost(&oledevent_flag,FLAG_BIT(key_time+POS_NUM),OS_OPT_POST_FLAG_SET,&err);	
				}
				prt("key_time:%u \r\n",key_time);
				key_cout++;
			}
			else if(key == KEY2_PRES || key == STOP)  //ȷ��
			{
				prt("confirm");
				//�ϱ�վ��
				OSFlagPost(&oledevent_flag,FLAG_BIT(key_time-1),OS_OPT_POST_FLAG_SET,&err);
				ISD4004_StopPlaying();
				OSFlagPost(&isd4004event_flag,FLAG_BIT(key_time-1),OS_OPT_POST_FLAG_SET,&err);
				OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s	
				key_cout = 0;
			}			
			else if(key == KEY3_PRES || key == EQ)  //����ȡ��
			{
				if(key_cout)
				{
					prt("cancel");
					Alter_key_time(&key_time,key_time1); //�Ѱ���ֵ�ָ�
					key_cout = 0;
					OSFlagPost(&oledevent_flag,FLAG_BIT(KEY_CANCEL),OS_OPT_POST_FLAG_SET,&err);
				}
				else //��û��ѡ��վ��ʱ��ȡ��������Ϊ���ܼ�
				{
					OSFlagPost(&oledevent_flag,FLAG_BIT(KEY_SETING),OS_OPT_POST_FLAG_SET,&err);
				}
			}
			key = 0;
			remotekey = 0;
			OSSemPost(&sem_oledandkey,OS_OPT_POST_1,&err);
		}
			
	}			
}
/******************************************************************************************************** 
**     ��������:         Alter_key_time
**    ��������:          �ı䰴������
**    ��ڲ�����       vu8 *time,int val
**    ���ڲ���:          ��
**    ����˵����       vu8 *time��������ָ�룬int valҪ�ı�ɵ�ֵ
********************************************************************************************************/ 
void Alter_key_time(vu8 *time,int val)
{
	OS_ERR err; 
	OSSemPend(&sem_key_time,0,OS_OPT_PEND_BLOCKING,NULL,&err);
	(*time) = val;
	OSSemPost(&sem_key_time,OS_OPT_POST_1,&err);
}
/******************************************************************************************************** 
**     ��������:         ReCreateKEYTask
**    ��������:         ���´���KEYTask
**    ��ڲ�����       ��
**    ���ڲ���:          ��
**    ����˵����       ��
********************************************************************************************************/ 
void ReCreateKEYTask(void)
{
	OS_ERR err;
	OSTaskCreate((OS_TCB 	* )&KEYTaskTCB,		//��������
				 (CPU_CHAR	* )"KEYTaskTCB task", 		
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
}
