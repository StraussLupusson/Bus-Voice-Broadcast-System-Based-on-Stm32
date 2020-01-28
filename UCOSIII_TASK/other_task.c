/*
	文件名：      other_task.c
	文件描述： other_task任务函数实现
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/
#include "other_task.h"
#include "sys.h"
#include "usart.h"
#include "start_task.h"
#include "oled_task.h"
#include "isd4004_task.h"
#include "dht11_task.h"
#include "gettime_task.h"
//任务控制块
OS_TCB OTHERTaskTCB;
//任务堆栈	
CPU_STK OTHER_TASK_STK[OTHER_STK_SIZE];

u8 syc ;
/******************************************************************************************************** 
**     函数名称:        OTHER_task
**    功能描述:         //OTHER任务函数
**    入口参数：       (void *p_arg)
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void OTHER_task(void *p_arg)
{
	u8 i = 0;
	OS_ERR err;
	p_arg = p_arg;
	prt("OTHER_task  start \r\n");
	while(1)
	{
		if(i<6)
		{
			OSFlagPost(&oledevent_flag,FLAG_BIT(OLED_OTHER1_FLAG),OS_OPT_POST_FLAG_SET,&err);
			OSSemPend(&sem_othertask,0,OS_OPT_PEND_BLOCKING,NULL,&err);	
			OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s					
			syc = 1;
			i++;
			if(i == 1 )
			{
				if(direction == POSITIVE_DIRCT)
					OSFlagPost(&isd4004event_flag,FLAG_BIT(ISD4004_POSITIVE_DIRECTION_FLAG),OS_OPT_POST_FLAG_SET,&err);
				else if(direction == REVERSE_DIRCT)
					OSFlagPost(&isd4004event_flag,FLAG_BIT(ISD4004_REVERSE_DIRECTION_FLAG),OS_OPT_POST_FLAG_SET,&err);
				
			}
		}
		else
		{	syc = 0;
			OSSemPend(&sem_isd4004task,0,OS_OPT_PEND_BLOCKING,NULL,&err);
			if(DHT11_OK_FLAG)
			{	//温湿度、文明乘车
				OSFlagPost(&oledevent_flag,FLAG_BIT(OLED_OTHER2_FLAG),OS_OPT_POST_FLAG_SET,&err);
				OSSemPend(&sem_othertask,0,OS_OPT_PEND_BLOCKING,NULL,&err);
				OSTimeDlyHMSM(0,0,5,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
				DHT11_OK_FLAG = 0;
			}
			//让座信息
			OSTimeDlyHMSM(0,0,1,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
			OSFlagPost(&oledevent_flag,FLAG_BIT(OLED_OTHER3_FLAG),OS_OPT_POST_FLAG_SET,&err);
			OSFlagPost(&isd4004event_flag,FLAG_BIT(ISD4004_OTHER_FLAG),OS_OPT_POST_FLAG_SET,&err);
			OSSemPend(&sem_othertask,0,OS_OPT_PEND_BLOCKING,NULL,&err);
			OSSemPend(&sem_isd4004task,0,OS_OPT_PEND_BLOCKING,NULL,&err);
			OSTimeDlyHMSM(0,0,5,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
			
			//班车信息
			OSFlagPost(&oledevent_flag,FLAG_BIT(OLED_OTHER4_FLAG),OS_OPT_POST_FLAG_SET,&err);
			OSSemPend(&sem_othertask,0,OS_OPT_PEND_BLOCKING,NULL,&err);
			OSTimeDlyHMSM(0,0,8,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
			i = 0;
		}		
					
	}
}
/******************************************************************************************************** 
**     函数名称:         ReCreateOTHERTask
**    功能描述:         重新创建OTHERTask任务
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void ReCreateOTHERTask(void)
{
	OS_ERR err;
	OSTaskCreate((OS_TCB 	* )&OTHERTaskTCB,		//其他任务
				 (CPU_CHAR	* )"OTHER task", 		
								 (OS_TASK_PTR )OTHER_task, 			
								 (void		* )0,					
								 (OS_PRIO	  )OTHER_TASK_PRIO,     
								 (CPU_STK   * )&OTHER_TASK_STK[0],	
								 (CPU_STK_SIZE)OTHER_STK_SIZE/10,	
								 (CPU_STK_SIZE)OTHER_STK_SIZE,		
								 (OS_MSG_QTY  )0,					
								 (OS_TICK	  )0,					
								 (void   	* )0,					
								 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
								 (OS_ERR 	* )&err);	
}