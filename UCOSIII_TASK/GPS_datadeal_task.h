/*
	�ļ�����      GPS_datadeal_task.h
	�ļ������� 	GPS_DATADEAL_task������ʵ��
	�޸�ʱ�䣺 2019��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/
#ifndef __GPS_DATADEAL_TASK_H_
#define __GPS_DATADEAL_TASK_H_


#include "includes.h"

#define SPEEDING_VALUE   30
//�������ȼ�
#define GPS_DATADEAL_TASK_PRIO		4u
//�����ջ��С	
#define GPS_DATADEAL_STK_SIZE 		128u
//������ƿ�
extern OS_TCB GPS_DATADEALTaskTCB;
//�����ջ	
extern CPU_STK GPS_DATADEAL_TASK_STK[GPS_DATADEAL_STK_SIZE];

void GPS_DATADEAL_task(void *p_arg);

#endif
