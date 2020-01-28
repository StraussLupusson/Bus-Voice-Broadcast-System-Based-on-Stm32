/*
	�ļ�����      DHT11_task.h
	�ļ������� DHT11_task������ʵ��
	�޸�ʱ�䣺 2019��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/
#ifndef DHT11_TASK_H_
#define DHT11_TASK_H_


#include "includes.h"
#include "dht11.h"
//�������ȼ�
#define DHT11_TASK_PRIO		4u
//�����ջ��С	
#define DHT11_STK_SIZE 		128u
//������ƿ�
extern OS_TCB DHT11TaskTCB;
//�����ջ	
extern CPU_STK DHT11_TASK_STK[DHT11_STK_SIZE];

void DHT11_task(void *p_arg);

#define NORMAL_TEMPERATURE   35
extern char temp_buf[4];
extern char humity_buf[5];
extern vu8 DHT11_OK_FLAG ;
#endif
