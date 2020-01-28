/*
	文件名：      GPS_datadeal_task.h
	文件描述： 	GPS_DATADEAL_task任务函数实现
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/
#ifndef __GPS_DATADEAL_TASK_H_
#define __GPS_DATADEAL_TASK_H_


#include "includes.h"

#define SPEEDING_VALUE   30
//任务优先级
#define GPS_DATADEAL_TASK_PRIO		4u
//任务堆栈大小	
#define GPS_DATADEAL_STK_SIZE 		128u
//任务控制块
extern OS_TCB GPS_DATADEALTaskTCB;
//任务堆栈	
extern CPU_STK GPS_DATADEAL_TASK_STK[GPS_DATADEAL_STK_SIZE];

void GPS_DATADEAL_task(void *p_arg);

#endif
