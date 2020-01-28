/*
	文件名：      DHT11_task.c
	文件描述： DHT11_task任务函数实现
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/
#include "DHT11_task.h"
#include "sys.h"
#include "usart.h"
#include <string.h>
#include "start_task.h"
//任务控制块
OS_TCB DHT11TaskTCB;
//任务堆栈	
CPU_STK DHT11_TASK_STK[DHT11_STK_SIZE];
vu8 DHT11_OK_FLAG = 0;
char temp_buf[4];
char humity_buf[5];


/******************************************************************************************************** 
**     函数名称:         DHT11_task
**    功能描述:          //DHT11任务函数
**    入口参数：       (void *p_arg)
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void DHT11_task(void *p_arg)
{
	CPU_SR_ALLOC();
	DHT11_Data DHT11_data;
	OS_ERR err;
	u8 ret;
	p_arg = p_arg;
	prt("DHT11  start \r\n");
	while(DHT11_Init() == ERROR)
	{
		prt("DHT11  time out ");
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms
	}
	
	while(1)
	{
		//memset(&DHT11_data,0,sizeof(DHT11_Data));
		 if(DHT11_Read_Data(&DHT11_data) == ERROR)
			DHT11_OK_FLAG = 0;
		else DHT11_OK_FLAG = 1;
		//prt("wendu:%.3f,shidu:%.3f \r\n",DHT11_data.wendu,DHT11_data.shidu);
		OS_CRITICAL_ENTER();
		sprintf(temp_buf,"%.0f",DHT11_data.wendu);
		sprintf(humity_buf,"%.0f%%",DHT11_data.shidu);
		OS_CRITICAL_EXIT();	
		//prt("dht11 task");
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms
		
		//OSTimeDlyHMSM(0,0,1,999,OS_OPT_TIME_HMSM_STRICT,&err); //延时200ms
	}
}
