/*
	文件名：      ISD4004_RECORD_task.h
	文件描述： ISD4004_RECORD_task任务函数实现
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/
#ifndef ISD4004_RECORDTASK_H_
#define ISD4004_RECORDTASK_H_


#include "includes.h"

//任务优先级
#define ISD4004_RECORD_TASK_PRIO		4u
//任务堆栈大小	
#define ISD4004_RECORD_STK_SIZE 		128u
//任务控制块
extern OS_TCB ISD4004_RECORDTaskTCB;
//任务堆栈	
extern CPU_STK ISD4004_RECORD_TASK_STK[ISD4004_RECORD_STK_SIZE];

void ISD4004_RECORD_task(void *p_arg);

#define POS_NUM 10

#endif
