/*
	文件名：     ISD4004_task.c
	文件描述： 	ISD4004语音播报处理任务
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/

#include "ISD4004_task.h"
#include "key_task.h"
#include "sys.h"
#include "usart.h"
#include "start_task.h"
#include "busposmesg.h"
#include "task_config.h"
//任务控制块
OS_TCB ISD4004TaskTCB;
//任务堆栈	
CPU_STK ISD4004_TASK_STK[ISD4004_STK_SIZE];
/******************************************************************************************************** 
**     函数名称:         ISD4004_task
**    功能描述:          ISD4004语音播报任务函数
**    入口参数：       	 void *p_arg
**    出口参数:          无
********************************************************************************************************/ 
void ISD4004_task(void *p_arg)
{
	u8 i;
	OS_FLAGS flag = 0;
	OS_ERR err;
	p_arg = p_arg;
	prt("ISD4004_task  start \r\n");
	ISD4004_Init();
	while(1)
	{
		flag = OSFlagPend((OS_FLAG_GRP*)&isd4004event_flag,
				   (OS_FLAGS	)0xFFFFFFFF,
		     	   (OS_TICK     )0,
				   (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ANY|OS_OPT_PEND_FLAG_CONSUME|OS_OPT_PEND_BLOCKING,
				   (CPU_TS*     )0,
				   (OS_ERR*	    )&err);
		if(flag)
		{
				for(i = 0;i <32; i++)
			{
				if(flag == FLAG_BIT(i))
				{
					if(i<9) //到站放音
					{
						prt("sta: %u \r\n",i+1);
						OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); //延时800ms
					}
					Play_Voice(i);
					if(i<9)
						OSSemPost(&sem_arrive,OS_OPT_POST_1,&err);
					if(i == ISD4004_OTHER_FLAG 
						||i ==ISD4004_POSITIVE_DIRECTION_FLAG || i == ISD4004_REVERSE_DIRECTION_FLAG )
						OSSemPost(&sem_isd4004task,OS_OPT_POST_1,&err);
					break;
				}
				else
					continue;
			}
			flag = 0;
		}	
	}
}
/******************************************************************************************************** 
**     函数名称:         Play_Voice
**    功能描述:          播放指定站点的语音集合
**    入口参数：       u8 voice_kind   //语音事件种类
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void Play_Voice(u8 voice_kind)
{
	switch(voice_kind)
	{
		case ISD4004_STA1_REACH:
		if(languagesetting !=  ENGLISH)
		{
			ISD4004_Playing(ISD4004_VADDR_1);
			ISD4004_Playing(ISD4004_VADDR_19);
		}
		if(languagesetting !=  CHINESE)
		{
			ISD4004_Playing(ISD4004_VADDR_25);
			ISD4004_Playing(ISD4004_VADDR_43);
		}
			break;
		case ISD4004_STA2_REACH:
		if(languagesetting !=  ENGLISH)
		{
			ISD4004_Playing(ISD4004_VADDR_2);
			if(direction == POSITIVE_DIRCT)  //正向
				ISD4004_Playing(ISD4004_VADDR_12);
			else if(direction == REVERSE_DIRCT)  //反向
				 ISD4004_Playing(ISD4004_VADDR_10);
		}
		if(languagesetting !=  CHINESE)
		{
			ISD4004_Playing(ISD4004_VADDR_26);
			if(direction == POSITIVE_DIRCT)  //正向
				ISD4004_Playing(ISD4004_VADDR_36);
			else if(direction == REVERSE_DIRCT)  //反向
				 ISD4004_Playing(ISD4004_VADDR_34);
		}
			break;
			
		case ISD4004_STA3_REACH:
		if(languagesetting !=  ENGLISH)
		{
			ISD4004_Playing(ISD4004_VADDR_3);
			if(direction == POSITIVE_DIRCT)  //正向
				ISD4004_Playing(ISD4004_VADDR_13);
			else if(direction == REVERSE_DIRCT)  //反向
				 ISD4004_Playing(ISD4004_VADDR_11);
		}
		if(languagesetting !=  CHINESE)
		{
			ISD4004_Playing(ISD4004_VADDR_27);
			if(direction == POSITIVE_DIRCT)  //正向
				ISD4004_Playing(ISD4004_VADDR_37);
			else if(direction == REVERSE_DIRCT)  //反向
				 ISD4004_Playing(ISD4004_VADDR_35);
		}
			break;
			
		case ISD4004_STA4_REACH:
		if(languagesetting !=  ENGLISH)
		{
			ISD4004_Playing(ISD4004_VADDR_4);
			if(direction == POSITIVE_DIRCT)  //正向
				ISD4004_Playing(ISD4004_VADDR_14);
			else if(direction == REVERSE_DIRCT)  //反向
				 ISD4004_Playing(ISD4004_VADDR_12);
		}
		if(languagesetting !=  CHINESE)
		{
			ISD4004_Playing(ISD4004_VADDR_28);
			if(direction == POSITIVE_DIRCT)  //正向
				ISD4004_Playing(ISD4004_VADDR_38);
			else if(direction == REVERSE_DIRCT)  //反向
				 ISD4004_Playing(ISD4004_VADDR_36);
		}
			break;
		case ISD4004_STA5_REACH:
		if(languagesetting !=  ENGLISH)
		{
			ISD4004_Playing(ISD4004_VADDR_5);
			if(direction == POSITIVE_DIRCT)  //正向
				ISD4004_Playing(ISD4004_VADDR_15);
			else if(direction == REVERSE_DIRCT)  //反向
				 ISD4004_Playing(ISD4004_VADDR_13);
		}
		if(languagesetting !=  CHINESE)
		{
			ISD4004_Playing(ISD4004_VADDR_29);
			if(direction == POSITIVE_DIRCT)  //正向
				ISD4004_Playing(ISD4004_VADDR_39);
			else if(direction == REVERSE_DIRCT)  //反向
				 ISD4004_Playing(ISD4004_VADDR_37);
		}
			break;
		case ISD4004_STA6_REACH:
		if(languagesetting !=  ENGLISH)
		{
			ISD4004_Playing(ISD4004_VADDR_6);
			if(direction == POSITIVE_DIRCT)  //正向
				ISD4004_Playing(ISD4004_VADDR_16);
			else if(direction == REVERSE_DIRCT)  //反向
				 ISD4004_Playing(ISD4004_VADDR_14);
		}
		if(languagesetting !=  CHINESE)
		{
			ISD4004_Playing(ISD4004_VADDR_30);
			if(direction == POSITIVE_DIRCT)  //正向
				ISD4004_Playing(ISD4004_VADDR_40);
			else if(direction == REVERSE_DIRCT)  //反向
				 ISD4004_Playing(ISD4004_VADDR_38);
		}
			break;
		case ISD4004_STA7_REACH:
		if(languagesetting !=  ENGLISH)
		{
			ISD4004_Playing(ISD4004_VADDR_7);
			if(direction == POSITIVE_DIRCT)  //正向
				ISD4004_Playing(ISD4004_VADDR_17);
			else if(direction == REVERSE_DIRCT)  //反向
				 ISD4004_Playing(ISD4004_VADDR_15);
		}
		if(languagesetting !=  CHINESE)
		{
			ISD4004_Playing(ISD4004_VADDR_31);
			if(direction == POSITIVE_DIRCT)  //正向
				ISD4004_Playing(ISD4004_VADDR_41);
			else if(direction == REVERSE_DIRCT)  //反向
				 ISD4004_Playing(ISD4004_VADDR_39);
		}
			break;
		case ISD4004_STA8_REACH:
		if(languagesetting !=  ENGLISH)
		{
			ISD4004_Playing(ISD4004_VADDR_8);
			if(direction == POSITIVE_DIRCT)  //正向
				ISD4004_Playing(ISD4004_VADDR_18);
			else if(direction == REVERSE_DIRCT)  //反向
				 ISD4004_Playing(ISD4004_VADDR_16);
		}
		if(languagesetting !=  CHINESE)
		{
			ISD4004_Playing(ISD4004_VADDR_32);
			if(direction == POSITIVE_DIRCT)  //正向
				ISD4004_Playing(ISD4004_VADDR_42);
			else if(direction == REVERSE_DIRCT)  //反向
				 ISD4004_Playing(ISD4004_VADDR_40);
		}
			break;
		case ISD4004_STA9_REACH:
		if(languagesetting !=  ENGLISH)
		{
			ISD4004_Playing(ISD4004_VADDR_9);
			ISD4004_Playing(ISD4004_VADDR_19);
		}
		if(languagesetting !=  CHINESE)
		{
			ISD4004_Playing(ISD4004_VADDR_33);
			ISD4004_Playing(ISD4004_VADDR_43);
		}
			break;
		case ISD4004_SYSTEMINIT_FLAG:
		if(languagesetting !=  ENGLISH)
		{
			ISD4004_Playing(ISD4004_VADDR_23);
		}
		if(languagesetting !=  CHINESE)
		{
			ISD4004_Playing(ISD4004_VADDR_47);
		}
			break;
		case ISD4004_OTHER_FLAG://其他语音事件1（宣传语）
		if(languagesetting !=  ENGLISH)
		{
			ISD4004_Playing(ISD4004_VADDR_22);
		}
		if(languagesetting !=  CHINESE)
		{
			ISD4004_Playing(ISD4004_VADDR_46);
		}
			break;
		
		case ISD4004_TURN_RIGHT_FLAG:
		if(languagesetting !=  ENGLISH)
		{
			ISD4004_Playing(ISD4004_VADDR_24);
		}
		if(languagesetting !=  CHINESE)
		{
			ISD4004_Playing(ISD4004_VADDR_48);
		}
			break;
			
		case ISD4004_TURN_LEFT_FLAG:
		if(languagesetting !=  ENGLISH)
		{
			ISD4004_Playing(ISD4004_VADDR_24);
		}
		if(languagesetting !=  CHINESE)
		{
			ISD4004_Playing(ISD4004_VADDR_48);
		}
			break;
			
		case ISD4004_ROLL_TOO_FLAG:
		if(languagesetting !=  ENGLISH)
		{
			ISD4004_Playing(ISD4004_VADDR_24);
		}
		if(languagesetting !=  CHINESE)
		{
			ISD4004_Playing(ISD4004_VADDR_48);
		}
			break;
			
		case ISD4004_SPEEDING_FLAG:
		if(languagesetting !=  ENGLISH)
		{
			ISD4004_Playing(ISD4004_VADDR_24);
		}
		if(languagesetting !=  CHINESE)
		{
			ISD4004_Playing(ISD4004_VADDR_48);
		}
			break;
			
		case ISD4004_POSITIVE_DIRECTION_FLAG:
		if(languagesetting !=  ENGLISH)
		{
			ISD4004_Playing(ISD4004_VADDR_20);
		}
		if(languagesetting !=  CHINESE)
		{
			ISD4004_Playing(ISD4004_VADDR_44);
		}
			break;
			
		case ISD4004_REVERSE_DIRECTION_FLAG:
		if(languagesetting !=  ENGLISH)
		{
			ISD4004_Playing(ISD4004_VADDR_21);
		}
		if(languagesetting !=  CHINESE)
		{
			ISD4004_Playing(ISD4004_VADDR_45);
		}
			
			

	}
}
