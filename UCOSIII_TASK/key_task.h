/*
	文件名：      key_task.c
	文件描述： key_task任务函数实现
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/
#ifndef KEY_TASK_H_
#define KEY_TASK_H_

#include "includes.h"
#define POS_NUM 9
extern vu8 key_time;
#define KEY_CANCEL 20
#define KEY_SETING 29
//任务优先级
#define KEY_TASK_PRIO		4u
//任务堆栈大小	
#define KEY_STK_SIZE 		128u
//任务控制块
extern OS_TCB KEYTaskTCB;
//任务堆栈	
extern CPU_STK KEY_TASK_STK[KEY_STK_SIZE];

void KEY_task(void *p_arg);
void Alter_key_time(vu8 *time,int val);
void ReCreateKEYTask(void);
#endif
