/*
	�ļ�����      key_task.c
	�ļ������� key_task������ʵ��
	�޸�ʱ�䣺 2019��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/
#ifndef KEY_TASK_H_
#define KEY_TASK_H_

#include "includes.h"
#define POS_NUM 9
extern vu8 key_time;
#define KEY_CANCEL 20
#define KEY_SETING 29
//�������ȼ�
#define KEY_TASK_PRIO		4u
//�����ջ��С	
#define KEY_STK_SIZE 		128u
//������ƿ�
extern OS_TCB KEYTaskTCB;
//�����ջ	
extern CPU_STK KEY_TASK_STK[KEY_STK_SIZE];

void KEY_task(void *p_arg);
void Alter_key_time(vu8 *time,int val);
void ReCreateKEYTask(void);
#endif
