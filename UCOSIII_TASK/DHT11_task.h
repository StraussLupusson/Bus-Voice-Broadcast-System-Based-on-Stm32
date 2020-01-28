/*
	文件名：      DHT11_task.h
	文件描述： DHT11_task任务函数实现
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/
#ifndef DHT11_TASK_H_
#define DHT11_TASK_H_


#include "includes.h"
#include "dht11.h"
//任务优先级
#define DHT11_TASK_PRIO		4u
//任务堆栈大小	
#define DHT11_STK_SIZE 		128u
//任务控制块
extern OS_TCB DHT11TaskTCB;
//任务堆栈	
extern CPU_STK DHT11_TASK_STK[DHT11_STK_SIZE];

void DHT11_task(void *p_arg);

#define NORMAL_TEMPERATURE   35
extern char temp_buf[4];
extern char humity_buf[5];
extern vu8 DHT11_OK_FLAG ;
#endif
