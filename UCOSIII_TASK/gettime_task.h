/*
	�ļ�����      GETTIME_task.h
	�ļ������� 	GETTIME_task������ʵ��
	�޸�ʱ�䣺 2019��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/
#ifndef GETTIME_TASK_H_
#define GETTIME_TASK_H_


#include "includes.h"

#define GETTIME_TIME3_FREQ          1000   //1000ms
//UTCʱ����Ϣ
__packed typedef struct  
{										    
 	u16 year;	//���
	u8 month;	//�·�
	u8 date;	//����
	u8 hour; 	//Сʱ
	u8 min; 	//����
	u8 sec; 	//����
	u8 week;   //����
}local_time;  

//�������ȼ�
#define GETTIME_TASK_PRIO		4u
//�����ջ��С	
#define GETTIME_STK_SIZE 		128u
//������ƿ�
extern OS_TCB GETTIMETaskTCB;
//�����ջ	
extern CPU_STK GETTIME_TASK_STK[GETTIME_STK_SIZE];

void GETTIME_task(void *p_arg);
extern local_time Local_Time;
#endif
