/*
	文件名：      main.c
	文件描述： 	 主函数
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/

#include "sys.h"
#include "delay.h"
#include "task.h"
#include "usart.h"
/******************************************************************************************************** 
**     函数名称:         main
**    功能描述:          主函数
**    入口参数：       无
**    出口参数:        无
**    其他说明：       初始化串口，创建开始任务
********************************************************************************************************/ 
 int main(void)
 {	
	OS_ERR err;
	CPU_SR_ALLOC();
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	//uart_init(115200);
	delay_ms(1000);
	
	//printf("BVBS PROJECT START \r\n");
	OSInit(&err); 
	OS_CRITICAL_ENTER();
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);	
	
	OS_CRITICAL_EXIT();
	OSStart(&err);
	
	while(1)
	{
		
	}
 
 }
