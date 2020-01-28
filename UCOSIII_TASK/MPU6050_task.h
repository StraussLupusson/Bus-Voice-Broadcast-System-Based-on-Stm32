/*
	文件名：      MPU6050_task.h
	文件描述： MPU6050_task任务函数实现
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/
#ifndef _MPU6050_TASK_H_
#define _MPU6050_TASK_H_


#include "includes.h"

#define ACQUISITION_INTERVAL 300  //采集时间
//任务优先级
#define MPU6050_TASK_PRIO		4u
//任务堆栈大小	
#define MPU6050_STK_SIZE 		128u
//任务控制块
extern OS_TCB MPU6050TaskTCB;
//任务堆栈	
extern CPU_STK MPU6050_TASK_STK[MPU6050_STK_SIZE];

void MPU6050_task(void *p_arg);

typedef struct {
			int pith;
			int roll;
			int yaw;
}bus_attitude;
 extern bus_attitude Bus_Attitude;
#endif
