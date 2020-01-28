/*
	文件名：      ISD4004_recordtask.c
	文件描述： ISD4004_recordtask任务函数实现
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/
#include "ISD4004_recordtask.h"
#include "LED.h"
#include "sys.h"
#include "usart.h"
#include "ISD4004_task.h"
#include "busposmesg.h"
#include "task_config.h"
//任务控制块
OS_TCB ISD4004_RECORDTaskTCB;
//任务堆栈	
CPU_STK ISD4004_RECORD_TASK_STK[ISD4004_RECORD_STK_SIZE];

//static u16 VoiceAddr[POS_NUM] = {170,270,370,470,570,670,770,870,970,1070};
/******************************************************************************************************** 
**     函数名称:         ISD4004_RECORD_task
**    功能描述:        //ISD4004_RECORD任务函数
**    入口参数：       (void *p_arg)
**    出口参数:         无
**    其他说明：       无
********************************************************************************************************/ 
#if ISD4004_RECORDTaskTCB_CONFIG
void ISD4004_RECORD_task(void *p_arg)
{

	OS_ERR err;
	u8 i;
	
	p_arg = p_arg;
	prt("ISD4004_RECORD_task  start \r\n");
	ISD4004_Init();
	
	while(1)
	{
		//ISD4004_Playing(ISD4004_VADDR_8);
		ISD4004_RecordingTreat();
		ISD4004_PlayingTreat();
		ISD4004_Display();
		//OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err); //延时800ms	
//	for(i = 0;i <VOICE_PASSAGE_NUM; i++)
//	{
//			ISD4004_Playing(addr[i]);
//			
//	}
	}
}
#endif
