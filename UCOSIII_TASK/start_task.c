/*
	文件名：      start_task.c
	文件描述： 系统任务开始任务
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/

#include "start_task.h"
#include "task_config.h"
#include "usart.h"
#include "key_task.h"
#include "ISD4004_task.h"
#include "ISD4004_recordtask.h"
#include "DHT11_task.h"
#include "OLED_task.h"
#include "gettime_task.h"
#include "other_task.h"
#include "wdg.h"

OS_FLAG_GRP isd4004event_flag;
OS_FLAG_GRP oledevent_flag;
OS_SEM  sem_key_time;
OS_SEM  sem_oledandkey;
OS_SEM  sem_othertask;
OS_SEM  sem_isd4004task;
OS_SEM  sem_arrive;
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];

/******************************************************************************************************** 
**     函数名称:         start_task
**    功能描述:          开始任务函数
**    入口参数：       	 void *p_arg
**    出口参数:          无
********************************************************************************************************/ 
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	CPU_Init();
#if 0
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
#endif	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	OS_CRITICAL_ENTER();	//进入临界区
	prt("start_task  start \r\n");    
	
	OSFlagCreate((OS_FLAG_GRP *)&isd4004event_flag,        //isd4004事件标志组，引起播报语音事件标志
								(CPU_CHAR *)"isd4004event_flag ",
								(OS_FLAGS)0x00,
								(OS_ERR *)&err
								);
	OSFlagCreate((OS_FLAG_GRP *)&oledevent_flag,        //oled事件件标志组，引起oled更新显示信息事件标志
								(CPU_CHAR *)"oledevent_flag ",
								(OS_FLAGS)0x00,
								(OS_ERR *)&err
								);
	
	OSSemCreate(&sem_key_time,"sem_key_time",1,&err);  // 按键次数信号量
	OSSemCreate(&sem_oledandkey,"sem_oledandkey",1,&err);  //按键和显示同步
	OSSemCreate(&sem_othertask,"sem_othertask",0,&err);   //OLED其他任务之间的同步
	OSSemCreate(&sem_isd4004task,"sem_isd4004task",0,&err);   //ISD4004其他任务之间的同步
	OSSemCreate(&sem_arrive,"sem_arrive",0,&err);   //到站后和其他任务之间的同步
#if GETTIMETaskTCB_CONFIG
	OSTaskCreate((OS_TCB 	* )&GETTIMETaskTCB,		   //获取时间
			 (CPU_CHAR	* )"GETTIME task", 		
							 (OS_TASK_PTR )GETTIME_task, 			
							 (void		* )0,					
							 (OS_PRIO	  )GETTIME_TASK_PRIO,     
							 (CPU_STK   * )&GETTIME_TASK_STK[0],	
							 (CPU_STK_SIZE)GETTIME_STK_SIZE/10,	
							 (CPU_STK_SIZE)GETTIME_STK_SIZE,		
							 (OS_MSG_QTY  )0,					
							 (OS_TICK	  )0,					
							 (void   	* )0,					
							 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
							 (OS_ERR 	* )&err);	
#endif

#if KEYTaskTCB_CONFIG
	OSTaskCreate((OS_TCB 	* )&KEYTaskTCB,		      //按键报站任务
				 (CPU_CHAR	* )"KEY task", 		
								 (OS_TASK_PTR )KEY_task, 			
								 (void		* )0,					
								 (OS_PRIO	  )KEY_TASK_PRIO,     
								 (CPU_STK   * )&KEY_TASK_STK[0],	
								 (CPU_STK_SIZE)KEY_STK_SIZE/10,	
								 (CPU_STK_SIZE)KEY_STK_SIZE,		
								 (OS_MSG_QTY  )0,					
								 (OS_TICK	  )0,					
								 (void   	* )0,					
								 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
								 (OS_ERR 	* )&err);									 
#endif


#if OLEDTaskTCB_CONFIG				 
				 OSTaskCreate((OS_TCB 	* )&OLEDTaskTCB,		//显示任务
				 (CPU_CHAR	* )"OLED task", 		
								 (OS_TASK_PTR )OLED_task, 			
								 (void		* )0,					
								 (OS_PRIO	  )OLED_TASK_PRIO,     
								 (CPU_STK   * )&OLED_TASK_STK[0],	
								 (CPU_STK_SIZE)OLED_STK_SIZE/10,	
								 (CPU_STK_SIZE)OLED_STK_SIZE,		
								 (OS_MSG_QTY  )0,					
								 (OS_TICK	  )0,					
								 (void   	* )0,					
								 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
								 (OS_ERR 	* )&err);	
#endif

#if ISD4004TaskTCB_CONFIG				 
				 OSTaskCreate((OS_TCB 	* )&ISD4004TaskTCB,		//语音播报任务
				 (CPU_CHAR	* )"ISD4004 task", 		
								 (OS_TASK_PTR )ISD4004_task, 			
								 (void		* )0,					
								 (OS_PRIO	  )ISD4004_TASK_PRIO,     
								 (CPU_STK   * )&ISD4004_TASK_STK[0],	
								 (CPU_STK_SIZE)ISD4004_STK_SIZE/10,	
								 (CPU_STK_SIZE)ISD4004_STK_SIZE,		
								 (OS_MSG_QTY  )0,					
								 (OS_TICK	  )0,					
								 (void   	* )0,					
								 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
								 (OS_ERR 	* )&err);	
#endif

#if ISD4004_RECORDTaskTCB_CONFIG
OSTaskCreate((OS_TCB 	* )&ISD4004_RECORDTaskTCB,		 // 录音任务
				 (CPU_CHAR	* )"ISD4004_RECORD task", 		
								 (OS_TASK_PTR )ISD4004_RECORD_task, 			
								 (void		* )0,					
								 (OS_PRIO	  )ISD4004_RECORD_TASK_PRIO,     
								 (CPU_STK   * )&ISD4004_RECORD_TASK_STK[0],	
								 (CPU_STK_SIZE)ISD4004_RECORD_STK_SIZE/10,	
								 (CPU_STK_SIZE)ISD4004_RECORD_STK_SIZE,		
								 (OS_MSG_QTY  )0,					
								 (OS_TICK	  )0,					
								 (void   	* )0,					
								 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
								 (OS_ERR 	* )&err);	
#endif

#if DHT11TaskTCB_CONFIG
				 OSTaskCreate((OS_TCB 	* )&DHT11TaskTCB,		//温湿度采集
				 (CPU_CHAR	* )"DHT11 task", 		
								 (OS_TASK_PTR )DHT11_task, 			
								 (void		* )0,					
								 (OS_PRIO	  )DHT11_TASK_PRIO,     
								 (CPU_STK   * )&DHT11_TASK_STK[0],	
								 (CPU_STK_SIZE)DHT11_STK_SIZE/10,	
								 (CPU_STK_SIZE)DHT11_STK_SIZE,		
								 (OS_MSG_QTY  )0,					
								 (OS_TICK	  )0,					
								 (void   	* )0,					
								 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
								 (OS_ERR 	* )&err);	
#endif


#if OTHERTaskTCB_CONFIG
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
#endif

	//OSTaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务			 
	OS_CRITICAL_EXIT();	//出临界区
	//初始化显示界面
	IWDG_Init(); //初始化独立看门狗
	while(1)
	{
		IWDG_Feed();
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms
	}
}

