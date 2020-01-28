/*
	�ļ�����      start_task.h
	�ļ�������	 start_task����ʵ��
	�޸�ʱ�䣺 2019��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/
#ifndef START_TASK_H_
#define START_TASK_H_


#include "includes.h"

#define ISD4004_SYSTEMINIT_FLAG	 16
#define ISD4004_POSITIVE_DIRECTION_FLAG 17
#define ISD4004_REVERSE_DIRECTION_FLAG 18

extern OS_FLAG_GRP oledevent_flag;
extern OS_FLAG_GRP isd4004event_flag;
extern OS_SEM  sem_key_time;
extern OS_SEM  sem_oledandkey;
extern OS_SEM  sem_othertask;
extern OS_SEM  sem_isd4004task;
extern OS_SEM  sem_arrive;
#define FLAG_BIT(n)  (1<<(n))
//�������ȼ�
#define START_TASK_PRIO		3u
//�����ջ��С	
#define START_STK_SIZE 		512u
//������ƿ�
extern OS_TCB StartTaskTCB;
//�����ջ	
extern CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);


#endif
