/*
	文件名：      GETTIME_task.h
	文件描述： 	GETTIME_task任务函数实现
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/
#ifndef GETTIME_TASK_H_
#define GETTIME_TASK_H_


#include "includes.h"

#define GETTIME_TIME3_FREQ          1000   //1000ms
//UTC时间信息
__packed typedef struct  
{										    
 	u16 year;	//年份
	u8 month;	//月份
	u8 date;	//日期
	u8 hour; 	//小时
	u8 min; 	//分钟
	u8 sec; 	//秒钟
	u8 week;   //星期
}local_time;  

//任务优先级
#define GETTIME_TASK_PRIO		4u
//任务堆栈大小	
#define GETTIME_STK_SIZE 		128u
//任务控制块
extern OS_TCB GETTIMETaskTCB;
//任务堆栈	
extern CPU_STK GETTIME_TASK_STK[GETTIME_STK_SIZE];

void GETTIME_task(void *p_arg);
extern local_time Local_Time;
#endif
