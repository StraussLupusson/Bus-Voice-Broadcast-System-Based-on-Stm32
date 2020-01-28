/*
	文件名：      key_task.c
	文件描述： key_task函数实现
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/
#include "key_task.h"
#include "sys.h"
#include "usart.h"
#include "start_task.h"
#include "key.h"
#include "ISD4004.h"
#include "remote.h"
//任务控制块
OS_TCB KEYTaskTCB;
//任务堆栈	
CPU_STK KEY_TASK_STK[KEY_STK_SIZE];

vu8 key_time = 1;
/******************************************************************************************************** 
**     函数名称:         KEY_task
**    功能描述:          按键任务函数
**    入口参数：       (void *p_arg)
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void KEY_task(void *p_arg)
{
	vu8 key = 0,remotekey = 0;
	vu8 key_cout = 0,key_time1 = 1;
	OS_ERR err; 
	p_arg = p_arg;
	prt("KEY_task  start \r\n");
	
	KEY_Init();
	Remote_Init();
	while(1)
	{
		key = KEY_Scan();
		remotekey = Remote_Scan(SINGLE_PRES);
		if(remotekey)key = remotekey;
		if(key)
		{
			OSSemPend(&sem_oledandkey,0,OS_OPT_PEND_BLOCKING,NULL,&err);//确保显示过程中不能按键
			if(key == KEY0_PRES || key == LEFT)//前一站
			{
				if(!key_cout) //首次按下按键
					key_time1 = key_time;//保存按键次数
				if(key_time>1)
				{
					Alter_key_time(&key_time,key_time-1);
					OSFlagPost(&oledevent_flag,FLAG_BIT(key_time+POS_NUM),OS_OPT_POST_FLAG_SET,&err);	
				}else {
					Alter_key_time(&key_time,9);
					OSFlagPost(&oledevent_flag,FLAG_BIT(key_time+POS_NUM),OS_OPT_POST_FLAG_SET,&err);	
				}
				prt("key_time:%u \r\n",key_time);
				key_cout++;	
			}
			else if(key == KEY1_PRES || key == RIGHT)  //下一站
			{
				if(!key_cout)//首次按下按键
					key_time1 = key_time;
				if(key_time<POS_NUM)
				{
					Alter_key_time(&key_time,key_time+1);
					OSFlagPost(&oledevent_flag,FLAG_BIT(key_time+POS_NUM),OS_OPT_POST_FLAG_SET,&err);		
				}else {
					Alter_key_time(&key_time,1);
					OSFlagPost(&oledevent_flag,FLAG_BIT(key_time+POS_NUM),OS_OPT_POST_FLAG_SET,&err);	
				}
				prt("key_time:%u \r\n",key_time);
				key_cout++;
			}
			else if(key == KEY2_PRES || key == STOP)  //确认
			{
				prt("confirm");
				//上报站点
				OSFlagPost(&oledevent_flag,FLAG_BIT(key_time-1),OS_OPT_POST_FLAG_SET,&err);
				ISD4004_StopPlaying();
				OSFlagPost(&isd4004event_flag,FLAG_BIT(key_time-1),OS_OPT_POST_FLAG_SET,&err);
				OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s	
				key_cout = 0;
			}			
			else if(key == KEY3_PRES || key == EQ)  //按键取消
			{
				if(key_cout)
				{
					prt("cancel");
					Alter_key_time(&key_time,key_time1); //把按键值恢复
					key_cout = 0;
					OSFlagPost(&oledevent_flag,FLAG_BIT(KEY_CANCEL),OS_OPT_POST_FLAG_SET,&err);
				}
				else //当没有选择站点时，取消键复用为功能键
				{
					OSFlagPost(&oledevent_flag,FLAG_BIT(KEY_SETING),OS_OPT_POST_FLAG_SET,&err);
				}
			}
			key = 0;
			remotekey = 0;
			OSSemPost(&sem_oledandkey,OS_OPT_POST_1,&err);
		}
			
	}			
}
/******************************************************************************************************** 
**     函数名称:         Alter_key_time
**    功能描述:          改变按键次数
**    入口参数：       vu8 *time,int val
**    出口参数:          无
**    其他说明：       vu8 *time按键变量指针，int val要改变成的值
********************************************************************************************************/ 
void Alter_key_time(vu8 *time,int val)
{
	OS_ERR err; 
	OSSemPend(&sem_key_time,0,OS_OPT_PEND_BLOCKING,NULL,&err);
	(*time) = val;
	OSSemPost(&sem_key_time,OS_OPT_POST_1,&err);
}
/******************************************************************************************************** 
**     函数名称:         ReCreateKEYTask
**    功能描述:         重新创建KEYTask
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void ReCreateKEYTask(void)
{
	OS_ERR err;
	OSTaskCreate((OS_TCB 	* )&KEYTaskTCB,		//其他任务
				 (CPU_CHAR	* )"KEYTaskTCB task", 		
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
}
