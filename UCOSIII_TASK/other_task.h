/*
	�ļ�����      OTHER_task.h
	�ļ������� OTHER_task������ʵ��
	�޸�ʱ�䣺 2019��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/
#ifndef __OTHER_TASK_H_
#define __OTHER_TASK_H_


#include "includes.h"

//�������ȼ�
#define OTHER_TASK_PRIO		4u
//�����ջ��С	
#define OTHER_STK_SIZE 		128u
//������ƿ�
extern OS_TCB OTHERTaskTCB;
//�����ջ	
extern CPU_STK OTHER_TASK_STK[OTHER_STK_SIZE];

void OTHER_task(void *p_arg);
void ReCreateOTHERTask(void);
extern u8 syc ;
#endif
