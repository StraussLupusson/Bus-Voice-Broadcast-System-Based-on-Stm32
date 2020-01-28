/*
	文件名：      OTHER_task.h
	文件描述： OTHER_task任务函数实现
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/
#ifndef __OTHER_TASK_H_
#define __OTHER_TASK_H_


#include "includes.h"

//任务优先级
#define OTHER_TASK_PRIO		4u
//任务堆栈大小	
#define OTHER_STK_SIZE 		128u
//任务控制块
extern OS_TCB OTHERTaskTCB;
//任务堆栈	
extern CPU_STK OTHER_TASK_STK[OTHER_STK_SIZE];

void OTHER_task(void *p_arg);
void ReCreateOTHERTask(void);
extern u8 syc ;
#endif
