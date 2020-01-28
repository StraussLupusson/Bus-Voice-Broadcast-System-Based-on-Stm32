/*
	�ļ�����      MPU6050_task.h
	�ļ������� MPU6050_task������ʵ��
	�޸�ʱ�䣺 2019��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/
#ifndef _MPU6050_TASK_H_
#define _MPU6050_TASK_H_


#include "includes.h"

#define ACQUISITION_INTERVAL 300  //�ɼ�ʱ��
//�������ȼ�
#define MPU6050_TASK_PRIO		4u
//�����ջ��С	
#define MPU6050_STK_SIZE 		128u
//������ƿ�
extern OS_TCB MPU6050TaskTCB;
//�����ջ	
extern CPU_STK MPU6050_TASK_STK[MPU6050_STK_SIZE];

void MPU6050_task(void *p_arg);

typedef struct {
			int pith;
			int roll;
			int yaw;
}bus_attitude;
 extern bus_attitude Bus_Attitude;
#endif
