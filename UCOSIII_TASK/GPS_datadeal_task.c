/*
	文件名：      GPS_DATADEAL_task.c
	文件描述： GPS_DATADEAL_task任务函数实现
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/
#include "GPS_datadeal_task.h"
#include "sys.h"
#include "usart.h"
#include "gps.h"
#include "isd4004_task.h"
#include "oled_task.h"
#include "start_task.h"

//任务控制块
OS_TCB GPS_DATADEALTaskTCB;
//任务堆栈	
CPU_STK GPS_DATADEAL_TASK_STK[GPS_DATADEAL_STK_SIZE];


/******************************************************************************************************** 
**     函数名称:         GPS_DATADEAL_task
**    功能描述:         GPS_DATADEAL任务函数
**    入口参数：       (void *p_arg)
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void GPS_DATADEAL_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	prt("GPS_DATADEAL  start \r\n");
	//GPS_Init();
	while(1)
	{
		prt("GPS_DATADEAL 12345678 \r\n ");
		GPS_AnalysisData();
		OS_CRITICAL_ENTER(); //临界区
		if(GPS_Msg.speed_val > SPEEDING_VALUE)
		{
			OSFlagPost(&oledevent_flag,FLAG_BIT(OLED_SPEEDING_FLAG),OS_OPT_POST_FLAG_SET,&err);
			OSFlagPost(&isd4004event_flag,FLAG_BIT(ISD4004_SPEEDING_FLAG),OS_OPT_POST_FLAG_SET,&err);
		}
			
		OS_CRITICAL_EXIT();
		OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err); //延时200ms
	}
}

	
	
