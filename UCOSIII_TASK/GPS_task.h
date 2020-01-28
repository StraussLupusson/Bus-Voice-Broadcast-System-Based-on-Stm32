/*
	文件名：      led.c
	文件描述： 任务函数实现
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/
#ifndef GPS_TASK_H_
#define GPS_TASK_H_


#include "includes.h"


//任务优先级
#define GPS_TASK_PRIO		4u
//任务堆栈大小	
#define GPS_STK_SIZE 		128u
//任务控制块
extern OS_TCB GPSTaskTCB;
//任务堆栈	
extern CPU_STK GPS_TASK_STK[GPS_STK_SIZE];

void GPS_task(void *p_arg);

#endif
