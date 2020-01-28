/*
	�ļ�����      ISD4004_RECORD_task.h
	�ļ������� ISD4004_RECORD_task������ʵ��
	�޸�ʱ�䣺 2019��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/
#ifndef ISD4004_RECORDTASK_H_
#define ISD4004_RECORDTASK_H_


#include "includes.h"

//�������ȼ�
#define ISD4004_RECORD_TASK_PRIO		4u
//�����ջ��С	
#define ISD4004_RECORD_STK_SIZE 		128u
//������ƿ�
extern OS_TCB ISD4004_RECORDTaskTCB;
//�����ջ	
extern CPU_STK ISD4004_RECORD_TASK_STK[ISD4004_RECORD_STK_SIZE];

void ISD4004_RECORD_task(void *p_arg);

#define POS_NUM 10

#endif
