/*
	�ļ�����      led.c
	�ļ������� ������ʵ��
	�޸�ʱ�䣺 2019��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/
#ifndef GPS_TASK_H_
#define GPS_TASK_H_


#include "includes.h"


//�������ȼ�
#define GPS_TASK_PRIO		4u
//�����ջ��С	
#define GPS_STK_SIZE 		128u
//������ƿ�
extern OS_TCB GPSTaskTCB;
//�����ջ	
extern CPU_STK GPS_TASK_STK[GPS_STK_SIZE];

void GPS_task(void *p_arg);

#endif
