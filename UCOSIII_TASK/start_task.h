/*
	文件名：      start_task.h
	文件描述：	 start_task函数实现
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/
#ifndef START_TASK_H_
#define START_TASK_H_


#include "includes.h"

#define ISD4004_SYSTEMINIT_FLAG	 16
#define ISD4004_POSITIVE_DIRECTION_FLAG 17
#define ISD4004_REVERSE_DIRECTION_FLAG 18

extern OS_FLAG_GRP oledevent_flag;
extern OS_FLAG_GRP isd4004event_flag;
extern OS_SEM  sem_key_time;
extern OS_SEM  sem_oledandkey;
extern OS_SEM  sem_othertask;
extern OS_SEM  sem_isd4004task;
extern OS_SEM  sem_arrive;
#define FLAG_BIT(n)  (1<<(n))
//任务优先级
#define START_TASK_PRIO		3u
//任务堆栈大小	
#define START_STK_SIZE 		512u
//任务控制块
extern OS_TCB StartTaskTCB;
//任务堆栈	
extern CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);


#endif
