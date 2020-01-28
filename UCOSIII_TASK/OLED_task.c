/*
	文件名：      OLED_task.c
	文件描述： OLED_task任务函数实现
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/
#include "OLED_task.h"
#include "OTHER_task.h"
#include "oled.h"
#include "sys.h"
#include "gps.h"
#include "usart.h"
#include "start_task.h"
#include "key_task.h"
#include "remote.h"
#include "dht11_task.h"
#include "busposmesg.h"
#include "gettime_task.h"
#include "led.h"
#include "key.h"
#include "bmp.h"
#include "task_config.h"
#include "GPS_datadeal_task.h"
#include "GPS_task.h"
#include "MPU6050_task.h"
char time[10];
char date1[10];
	//char speed[10];
	struct date{
		u8 year;
		u8 mon;
		u8 day;
	} local_date;
vu8 languagesetting = DEFAULT_LANGUAGE;
vu8 direction = POSITIVE_DIRCT; //列车方向
OS_TCB OLEDTaskTCB;
//任务堆栈	
CPU_STK OLED_TASK_STK[OLED_STK_SIZE];
//内部函数声明
void ShowTitle(void);
void ShowUpDownPos(u8 pos);
void Show_Msg(u8 kind);
void ShowTime(void);
void ShowTempHumity(void);
void ShowSlogan(void);
void ShowOtherEventMsg(u8 kind);
void System_Init(void);
void Setting(void);
void SetTime(void);
void SetLanguage(void);
void Time_and_Language_Set(void);
void GPS_Open(void);
void GPS_Close(void);
void ShowGPSData(void);
void GPS_Set(void);
void MPU_Open(void);
void MPU_Close(void);
void MPU_Set(void);
void TEMP_task(void *p_arg);
/******************************************************************************************************** 
**     函数名称:         OLED_task
**    功能描述:          //OLED任务函数
**    入口参数：       (void *p_arg)
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void OLED_task(void *p_arg)
{
	u8 i;
	CPU_SR_ALLOC();
	OS_FLAGS flag = 0;
	OS_ERR err;
	p_arg = p_arg;
	prt("OLED_task  start \r\n");
	OLED_Init();
	LED_Init();
	System_Init();
	while(1)
	{
		flag = OSFlagPend((OS_FLAG_GRP*)&oledevent_flag,
				   (OS_FLAGS	)0xFFFFFFFF,
		     	  (OS_TICK     )0,
				   (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ANY|OS_OPT_PEND_FLAG_CONSUME|OS_OPT_PEND_BLOCKING,
				   (CPU_TS*     )0,
				   (OS_ERR*	    )&err);
		//prt("OLED_task get the flag");
		if(flag)
		{
				for(i = 0;i <32; i++)
			{
				
				if(flag == FLAG_BIT(i))
				{
					OSSemPend(&sem_oledandkey,0,OS_OPT_PEND_BLOCKING,NULL,&err);
					if(i<CANCEL_SELECT  )
						OSTaskDel((OS_TCB*)&OTHERTaskTCB,&err);		//删除其他任务
					if(i<= STA10_REACH || i == OLED_ROLL_TOO_FLAG || i == OLED_SPEEDING_FLAG) LED0 = 0;
					//清除屏幕 对不同事件做选择性清屏
					if(i <= CANCEL_SELECT)
					{
						OLED_Fill(0,128,0);
						OLED_Fill(0,128,1);
						OLED_Fill(0,128,2);
					}
					if(i > CANCEL_SELECT && i != OLED_OTHER1_FLAG && i != KEY_SETING_FLAG)
					{
						OLED_Fill(0,128,0);
						OLED_Fill(0,128,1);
					}
					if(i<= STA10_REACH)
					{
						ShowTitle(); //显示标题
					}
					if(i>STA10_REACH && i<CANCEL_SELECT)
					{
						if(languagesetting !=  ENGLISH)
						{
							OLED_ShowCHineseString(32,0,zdxz,5); // 站点选择：
						}
						else
						{
							OLED_ShowString(20,0,(u8*)"Select Pos:"); // 站点选择：
						}
					}
						
					if( i == KEY_SETING_FLAG)
					{
						OLED_Clear();
						Setting();
					}
						
					if(i >= OLED_OTHER1_FLAG && i <= OLED_OTHER4_FLAG ) //其他任务事件触发
					{
							ShowOtherEventMsg(i);
					}
					if(i <	OLED_OTHER1_FLAG && i != CANCEL_SELECT )
					{
							Show_Msg(i); //显示主要信息
					}
					if(i <= CANCEL_SELECT)
					{
							ShowUpDownPos(key_time); //显示上下站点
					}
					//重新创建其他任务
					if(i<SELECT_STA1 || i == CANCEL_SELECT  )  
					{
						if(i<=STA10_REACH)
						{
							OSTimeDlyHMSM(0,0,5,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时5
							if(direction == POSITIVE_DIRCT)  //正向
							{
								if(key_time<POS_NUM)
									Alter_key_time(&key_time,key_time+1); //把按键值加1
								else {direction = REVERSE_DIRCT;
									Alter_key_time(&key_time,key_time-1); //把按键值-1
								}
							}
							else if(direction == REVERSE_DIRCT) //反向
							{
								if(key_time>1)
									Alter_key_time(&key_time,key_time-1); //把按键值-1
								else {direction = POSITIVE_DIRCT;
									Alter_key_time(&key_time,key_time+1); //把按键值加1
								}
							}
							OSSemPend(&sem_arrive,0,OS_OPT_PEND_BLOCKING,NULL,&err);	
							ShowTitle();
						}
						OS_CRITICAL_ENTER();	
						ReCreateOTHERTask();						
						OS_CRITICAL_EXIT();
						syc = 0;
					}	
					LED0 = 1;
					OSSemPost(&sem_oledandkey,OS_OPT_POST_1,&err);
					break;
				}
				else
					continue;
			
			}
			flag = 0;
		}
								
		//OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_HMSM_STRICT,&err); //延时800ms	
		
	}
}
/******************************************************************************************************** 
**     函数名称:         System_Init
**    功能描述:         显示系统信息,初始化系统
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void System_Init(void)
{
	OS_ERR err;
	OSFlagPost(&isd4004event_flag,FLAG_BIT(ISD4004_SYSTEMINIT_FLAG),OS_OPT_POST_FLAG_SET,&err);
	if(languagesetting!= ENGLISH)
	{
		OLED_ShowCHineseString(0,0,gj,2);  //公交
		OLED_ShowCHineseString(32,0,productionmsg,6);  //语音播报系统
		OLED_ShowCHineseString(16,1,schoolname,4);
		OLED_ShowNum(80,1,1153,4);
		OLED_ShowCHineseString(40,2,author,3);
		OLED_ShowCHineseString(16,3,taitou,6);

	}
	else {
			OLED_ShowString(0,0,(u8*)"Bus Voice Broadc");
			OLED_ShowString(0,1,(u8*)"ast System");
			OLED_ShowString(32,2,(u8*)"Author:");
			OLED_ShowString(60,3,(u8*)"Strauss");
	}
	
	OSTimeDlyHMSM(0,0,3,50,OS_OPT_TIME_HMSM_STRICT,&err); //延时2s
	OLED_Clear();
	ShowTitle();
	if(languagesetting!= ENGLISH)
	{
		OLED_ShowCHineseString(80,2,pos_cname[1],4);
		OLED_ShowCHineseString(0,3,syz,3);//上一站
		OLED_ShowCHineseString(80,3,xyz,3);//下一站

	}
	else {
			OLED_ShowString(80,2,(u8*)"Haiwei");
			OLED_ShowString(0,3,(u8 *)"Prev");
			OLED_ShowString(96,3,(u8 *)"Next");
	}
	OSTimeDlyHMSM(0,0,2,50,OS_OPT_TIME_HMSM_STRICT,&err); //延时
	OLED_Fill(0,128,0);
}


/******************************************************************************************************** 
**     函数名称:         Show_Msg
**    功能描述:         显示信息
**    入口参数：       u8 kind   //显示信息的种类
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void Show_Msg(u8 kind)
{	
	OS_ERR err;
	switch(kind)
	{
		//到站显示
		case STA1_REACH: 
				if(languagesetting !=  ENGLISH)
				{
					OLED_ShowCHineseString(8,1,pos_cname[0],4);
					OLED_ShowCHineseString(72,1,daole,3);
				}
				else OLED_ShowString(0,1,(u8*)"Ocean University");
				break;
		case STA2_REACH:
				if(languagesetting !=  ENGLISH)
				{
					OLED_ShowCHineseString(16,1,pos_cname[1],3);
					OLED_ShowCHineseString(64,1,daole,3);		
				}
				else OLED_ShowString(0,1,(u8*)" Haiwei Village.");						
				break;

		case STA3_REACH:
				if(languagesetting !=  ENGLISH)
				{
					OLED_ShowCHineseString(16,1,pos_cname[2],3);	
				OLED_ShowCHineseString(64,1,daole,3);			
				}
				else OLED_ShowString(0,1,(u8*)" Lindong Village.");							
				break;
				
		case STA4_REACH:
				if(languagesetting !=  ENGLISH)
				{
					OLED_ShowCHineseString(16,1,pos_cname[3],3);
					OLED_ShowCHineseString(64,1,daole,3);				
				}
				else OLED_ShowString(0,1,(u8*)" Mulan Village.");							
				break;
				
				
		case STA5_REACH:
				if(languagesetting !=  ENGLISH)
				{
					OLED_ShowCHineseString(16,1,pos_cname[4],3);	
					OLED_ShowCHineseString(64,1,daole,3);			
				}
				else OLED_ShowString(0,1,(u8*)" Baoman Village.");										
				break;
				
		case STA6_REACH:
				if(languagesetting !=  ENGLISH)
				{
					OLED_ShowCHineseString(16,1,pos_cname[5],3);	
					OLED_ShowCHineseString(64,1,daole,3);			
				}
				else OLED_ShowString(0,1,(u8*)" Stone Village.");	
												
				break;
				
		case STA7_REACH:
				if(languagesetting !=  ENGLISH)
				{
					OLED_ShowCHineseString(16,1,pos_cname[6],3);	
					OLED_ShowCHineseString(64,1,daole,3);				
				}
				else OLED_ShowString(0,1,(u8*)" LongHua Village.");	
												
				break;
			
		case STA8_REACH:
				if(languagesetting !=  ENGLISH)
				{
					OLED_ShowCHineseString(8,1,pos_cname[7],4);
					OLED_ShowCHineseString(72,1,daole,3);				
				}
				else OLED_ShowString(0,1,(u8*)"lake light Road.");	
									
				break;
				
		case STA9_REACH:
				if(languagesetting !=  ENGLISH)
				{					
					OLED_ShowCHineseString(16,1,pos_cname[8],4);	
					OLED_ShowCHineseString(64,1,daole,3);				
				}
				else OLED_ShowString(16,1,(u8*)" NanZhan Sta.");	
				
				break;
			
		//case STA10_REACH:
				//OLED_ShowCHineseString(16,1,pos_cname[9],6);
				//break;
				
		//站点按键选择 居中显示
		case SELECT_STA1:
					if(languagesetting !=  ENGLISH)
						OLED_ShowCHineseString(32,1,pos_cname[0],4);
					else OLED_ShowString(0,1,(u8*)"Ocean University");
					break;   
		case SELECT_STA2:
				if(languagesetting !=  ENGLISH)
					OLED_ShowCHineseString(40,1,pos_cname[1],3);
				else OLED_ShowString(0,1,(u8*)" Haiwei Village.");		
				break;
		case SELECT_STA3:
				if(languagesetting !=  ENGLISH)
				OLED_ShowCHineseString(40,1,pos_cname[2],3);
				else OLED_ShowString(0,1,(u8*)" Lindong Village");
				break;
		case SELECT_STA4:
				if(languagesetting !=  ENGLISH)
				OLED_ShowCHineseString(40,1,pos_cname[3],3);
				else OLED_ShowString(0,1,(u8*)" Mulan Village.");		
				break;
		case SELECT_STA5:
				if(languagesetting !=  ENGLISH)
				OLED_ShowCHineseString(40,1,pos_cname[4],3);
				else OLED_ShowString(0,1,(u8*)" Baoman Village.");		
				break;
		case SELECT_STA6:
				if(languagesetting !=  ENGLISH)
				OLED_ShowCHineseString(40,1,pos_cname[5],3);
				else OLED_ShowString(0,1,(u8*)" Stone Village.");	
				break;
		case SELECT_STA7:
				if(languagesetting !=  ENGLISH)
				OLED_ShowCHineseString(40,1,pos_cname[6],3);	
				else OLED_ShowString(0,1,(u8*)" LongHua Village");	
				break;	
		case SELECT_STA8:
				if(languagesetting !=  ENGLISH)
				OLED_ShowCHineseString(32,1,pos_cname[7],4);
				else OLED_ShowString(0,1,(u8*)" lakelight Road.");	
				break;
		case SELECT_STA9:
				if(languagesetting !=  ENGLISH)
				OLED_ShowCHineseString(40,1,pos_cname[8],4);
				else OLED_ShowString(16,1,(u8*)" NanZhan Sta.");
				break;
		//case SELECT_STA10:
				//OLED_ShowCHineseString(40,1,pos_cname[9],3);
				//break;
		//mpu信息显示
		case OLED_TURN_LEFT_FLAG:
					if(languagesetting !=  ENGLISH)
					OLED_ShowCHineseString(48,0,zuozhuan,2);  
					else OLED_ShowString(0,0,(u8*)"   Turn Left");
					OSTimeDlyHMSM(0,0,1,50,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
					OSSemPost(&sem_othertask,OS_OPT_POST_1,&err);
					break;
		case OLED_TURN_RIGHT_FLAG:
					if(languagesetting !=  ENGLISH)
					OLED_ShowCHineseString(48,0,youzhuan,2);  
					else OLED_ShowString(0,0,(u8*)"  Turn Right");   
					OSTimeDlyHMSM(0,0,1,50,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
					OSSemPost(&sem_othertask,OS_OPT_POST_1,&err);
					break;
		case OLED_ROLL_TOO_FLAG:
					if(languagesetting !=  ENGLISH)
					OLED_ShowCHineseString(32,0,dianbo,4);
					else OLED_ShowString(0,0,(u8*)"    Bumping! ");
					OSTimeDlyHMSM(0,0,1,50,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
					OSSemPost(&sem_othertask,OS_OPT_POST_1,&err);
					break;	
		case OLED_SPEEDING_FLAG:
					if(languagesetting !=  ENGLISH)
					OLED_ShowCHineseString(32,0,chaosu,4);  
					else OLED_ShowString(0,0,(u8*)"    Speeding! ");
					OSTimeDlyHMSM(0,0,2,50,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
					OSSemPost(&sem_othertask,OS_OPT_POST_1,&err);
					break;	
		
	}

}

/******************************************************************************************************** 
**     函数名称:         ShowTitle
**    功能描述:         显示title
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void ShowTitle(void)
{
	if(languagesetting !=  ENGLISH)
	{
		OLED_ShowCHineseString(4,0,title,4);  //显示路线信息
		OLED_ShowNum(64,0,114,3);
		OLED_ShowCHineseString(88,0,gj,2);  //公交
	}
	else
	{
		OLED_ShowString(4,0,(u8*)"This is Bus 114");  //显示路线信息
		
	}
	
}
/******************************************************************************************************** 
**     函数名称:         Showudpos
**    功能描述:          显示上下站信息函数
**    入口参数：       u8 pos
**    出口参数:          无
**    其他说明：       传入当前站点
********************************************************************************************************/ 
void ShowUpDownPos(u8 pos)
{
	switch(pos) //显示上下站信息
	{
		case 1 : 
				if(languagesetting !=  ENGLISH)
				{
					if(direction == POSITIVE_DIRCT)  //正向
					{
						OLED_ShowCHineseString(80,2,pos_cname[1],3);
					}
					else if(direction == REVERSE_DIRCT)
					{
						OLED_ShowCHineseString(0,2,pos_cname[1],3);
					}
				}
				else{
						if(direction == POSITIVE_DIRCT)  //正向
					{
						OLED_ShowString(80,2,(u8*)"Haiwei");
					}
					else if(direction == REVERSE_DIRCT)
					{
						OLED_ShowString(0,2,(u8*)"Haiwei");
					}
					
				}
				break;  
				
		case 2 : 
				if(languagesetting !=  ENGLISH)
				{
					if(direction == POSITIVE_DIRCT)  //正向
					{
						OLED_ShowCHineseString(0,2,pos_cname[0],4);
						OLED_ShowCHineseString(80,2,pos_cname[2],3);
					}
					else if(direction == REVERSE_DIRCT)
					{
						OLED_ShowCHineseString(0,2,pos_cname[2],3);
						OLED_ShowCHineseString(64,2,pos_cname[0],4);
					}
				}else{
							if(direction == POSITIVE_DIRCT)  //正向
						{
							OLED_ShowString(0,2,(u8*)"Ocean");
							OLED_ShowString(72,2,(u8*)"Lindong");
						}
						else if(direction == REVERSE_DIRCT)
						{
							OLED_ShowString(0,2,(u8*)"Lindong");
							OLED_ShowString(88,2,(u8*)"Ocean");
						}
				}
				break;   
				
		case 3 : 
				if(languagesetting !=  ENGLISH)
				{
					if(direction == POSITIVE_DIRCT)  //正向
						{
							OLED_ShowCHineseString(0,2,pos_cname[1],3);
							OLED_ShowCHineseString(80,2,pos_cname[3],3);
						}
						else if(direction == REVERSE_DIRCT)
						{
							OLED_ShowCHineseString(0,2,pos_cname[3],3);
							OLED_ShowCHineseString(80,2,pos_cname[1],3);
						}
					
				}else{
						if(direction == POSITIVE_DIRCT)  //正向
						{
							OLED_ShowString(0,2,(u8*)"Haiwei");
							OLED_ShowString(88,2,(u8*)"Mulan");
						}
						else if(direction == REVERSE_DIRCT)
						{
							OLED_ShowString(0,2,(u8*)"Mulan");
							OLED_ShowString(80,2,(u8*)"Haiwei");
						}
					
				}
				break;  
				
		case 4 : 
				if(languagesetting !=  ENGLISH)
				{
					if(direction == POSITIVE_DIRCT)  //正向
					{
						OLED_ShowCHineseString(0,2,pos_cname[2],3);
						OLED_ShowCHineseString(80,2,pos_cname[4],3);
					}
					else if(direction == REVERSE_DIRCT)
					{
						OLED_ShowCHineseString(0,2,pos_cname[4],3);
						OLED_ShowCHineseString(80,2,pos_cname[2],3);
					}

				}else{
						if(direction == POSITIVE_DIRCT)  //正向
						{
							OLED_ShowString(0,2,(u8*)"Lindong");
							OLED_ShowString(80,2,(u8*)"Baoman");
						}
						else if(direction == REVERSE_DIRCT)
						{
							OLED_ShowString(0,2,(u8*)"Baoman");
							OLED_ShowString(72,2,(u8*)"Lindong");
						}
					
				}
				break; 
				
		case 5 : 
				if(languagesetting !=  ENGLISH)
				{
					if(direction == POSITIVE_DIRCT)  //正向
						{
							OLED_ShowCHineseString(0,2,pos_cname[3],3);
							OLED_ShowCHineseString(80,2,pos_cname[5],3);
						}
						else if(direction == REVERSE_DIRCT)
						{
							OLED_ShowCHineseString(0,2,pos_cname[5],3);
							OLED_ShowCHineseString(80,2,pos_cname[3],3);
						}
					
				}else{
						if(direction == POSITIVE_DIRCT)  //正向
						{
							OLED_ShowString(0,2,(u8*)"Mulan");
							OLED_ShowString(88,2,(u8*)"Stone");
						}
						else if(direction == REVERSE_DIRCT)
						{
							OLED_ShowString(0,2,(u8*)"Stone");
							OLED_ShowString(88,2,(u8*)"Mulan");
						}
					
				}
				break; 
				   
		case 6 : 
				if(languagesetting !=  ENGLISH)
				{
					if(direction == POSITIVE_DIRCT)  //正向
						{
							OLED_ShowCHineseString(0,2,pos_cname[4],3);
							OLED_ShowCHineseString(80,2,pos_cname[6],3);
						}
						else if(direction == REVERSE_DIRCT)
						{
							OLED_ShowCHineseString(0,2,pos_cname[6],3);
							OLED_ShowCHineseString(80,2,pos_cname[4],3);
						}
					
				}else{
						if(direction == POSITIVE_DIRCT)  //正向
						{
							OLED_ShowString(0,2,(u8*)"Baoman");
							OLED_ShowString(72,2,(u8*)"LongHua");
						}
						else if(direction == REVERSE_DIRCT)
						{
							OLED_ShowString(0,2,(u8*)"LongHua");
							OLED_ShowString(80,2,(u8*)"Baoman");
						}	
				}
				break; 
				
				 
		case 7 : 
				if(languagesetting !=  ENGLISH)
				{
					if(direction == POSITIVE_DIRCT)  //正向
						{
							OLED_ShowCHineseString(0,2,pos_cname[5],3);
							OLED_ShowCHineseString(64,2,pos_cname[7],4);
						}
						else if(direction == REVERSE_DIRCT)
						{
							OLED_ShowCHineseString(0,2,pos_cname[7],4);
							OLED_ShowCHineseString(80,2,pos_cname[5],3);
						}	
				}else{
						if(direction == POSITIVE_DIRCT)  //正向
						{
							OLED_ShowString(0,2,(u8*)"Stone");
							OLED_ShowString(72,2,(u8*)"Huguang");
						}
						else if(direction == REVERSE_DIRCT)
						{
							OLED_ShowString(0,2,(u8*)"Huguang");
							OLED_ShowString(88,2,(u8*)"Stone");
						}	
					
				}
				break; 
				
		case 8 : 
				if(languagesetting !=  ENGLISH)
				{
					if(direction == POSITIVE_DIRCT)  //正向
						{
							OLED_ShowCHineseString(0,2,pos_cname[6],3);
							OLED_ShowCHineseString(64,2,pos_cname[8],4);
						}
						else if(direction == REVERSE_DIRCT)
						{
							OLED_ShowCHineseString(0,2,pos_cname[8],4);
							OLED_ShowCHineseString(80,2,pos_cname[6],3);
						}	
					
				}else{
						if(direction == POSITIVE_DIRCT)  //正向
						{
							OLED_ShowString(0,2,(u8*)"LongHua");
							OLED_ShowString(72,2,(u8*)"NanZhan");
						}
						else if(direction == REVERSE_DIRCT)
						{
							OLED_ShowString(0,2,(u8*)"NanZhan");
							OLED_ShowString(72,2,(u8*)"LongHua");
						}	
					
				}
				break;
				
				
		case 9 :
				if(languagesetting !=  ENGLISH)
				{	
					if(direction == POSITIVE_DIRCT)  //正向
					{
						OLED_ShowCHineseString(0,2,pos_cname[8],4);
					}
					else if(direction == REVERSE_DIRCT)
					{
						OLED_ShowCHineseString(64,2,pos_cname[8],4);
					}
				}
				else{
						if(direction == POSITIVE_DIRCT)  //正向
						{
							OLED_ShowString(0,2,(u8*)"Huguang");
						}
						else if(direction == REVERSE_DIRCT)
						{
							OLED_ShowString(72,2,(u8*)"Huguang");
						}
					
				}
				break;  
		//case 10 : OLED_ShowCHineseString(0,2,pos_cname[8],3);
				//break;   
				
	}
}
/******************************************************************************************************** 
**     函数名称:         ShowTime
**    功能描述:          显示时间日期
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void ShowTime(void)
{
	CPU_SR_ALLOC();
	if(syc == 0) //时间事件，保证清屏操作只有一次
		{
			OLED_Fill(0,128,0);
			OLED_Fill(0,128,1);
		}
		OS_CRITICAL_ENTER();
		sprintf(date1,"%u:%u:%u",Local_Time.year+1792,Local_Time.month,Local_Time.date);
		sprintf(time,"%u:%u:%u",Local_Time.hour,Local_Time.min,Local_Time.sec);
		OS_CRITICAL_EXIT();
		
		OS_CRITICAL_ENTER();	
		local_date.year = Local_Time.year;
		local_date.mon = Local_Time.month;
		local_date.day = Local_Time.date;
		OS_CRITICAL_EXIT();
		if(languagesetting !=  ENGLISH)
		{
			OLED_ShowNum(8,0,local_date.year+1792,4);
			OLED_ShowCHinese(40,0,nyr,0);
			OLED_ShowNum(56,0,local_date.mon,2);
			OLED_ShowCHinese(72,0,nyr,1);
			OLED_ShowNum(88,0,local_date.day,2);
			OLED_ShowCHinese(104,0,nyr,2);
		}
		 else
		{
			OLED_ShowString(0,0,(u8*)date1);
		}
		
		OS_CRITICAL_ENTER();
		if(Local_Time.sec < 2 ) //59到1s清屏
			OLED_Fill(96,128,1);
		OS_CRITICAL_EXIT();
		if(languagesetting !=  ENGLISH)
		{
			OLED_ShowCHineseString(8,1,shijian,3);
		}
		 else
		{
			OLED_ShowString(0,1,(u8*)"Time:");
		}
		OLED_ShowString(56,1,(u8*)time);
}
/******************************************************************************************************** 
**     函数名称:         ShowTempHumity
**    功能描述:          显示温度湿度
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void ShowTempHumity(void)
{
	if(languagesetting !=  ENGLISH)
		{
			OLED_ShowCHineseString(0,0,wd,3);	
			OLED_ShowString(48,0,(u8*)temp_buf);
			OLED_ShowCHinese(72,0,wd,1);	
			OLED_ShowCHineseString(0,1,sd,3);	
			OLED_ShowString(48,1,(u8*)humity_buf);	
		}
		 else
		{
			OLED_ShowString(0,0,(u8*)"Temp:");
			OLED_ShowString(64,0,(u8*)temp_buf);
			OLED_ShowString(0,1,(u8*)"Humity:");
			OLED_ShowString(64,1,(u8*)humity_buf);	
		}	
}
/******************************************************************************************************** 
**     函数名称:         ShowSlogan
**    功能描述:          显示宣传语
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void ShowSlogan(void)
{
	if(languagesetting !=  ENGLISH)
		{
			OLED_ShowCHineseString(0,0,zlay1,8);
			OLED_ShowCHineseString(0,1,zlay2,8);
			
		}
		 else
		{ 
			OLED_ShowString(0,0,(u8*)"give a seat for");
			OLED_ShowString(0,1,(u8*)"those in need");
		}	
}
/******************************************************************************************************** 
**     函数名称:         ShowOtherEventMsg
**    功能描述:          OTHER任务函数事件触发显示函数
**    入口参数：       u8 kind //其他事件的种类
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void ShowOtherEventMsg(u8 kind)
{
	OS_ERR err;
	if( kind == OLED_OTHER1_FLAG) //第一种其他事件
	{
		ShowTime();//显示时间
		OSSemPost(&sem_othertask,OS_OPT_POST_1,&err);
	}
	else if(kind == OLED_OTHER2_FLAG)//第二种其他事件，温湿度事件
	{
		ShowTempHumity();
		OSSemPost(&sem_othertask,OS_OPT_POST_1,&err);
	}
	else if(kind == OLED_OTHER3_FLAG )//宣传语事件
	{
		ShowSlogan();
		OSSemPost(&sem_othertask,OS_OPT_POST_1,&err);	
	}

	else if(kind == OLED_OTHER4_FLAG )//路线信息事件
	{
		ShowTitle(); //显示标题
		OSSemPost(&sem_othertask,OS_OPT_POST_1,&err);
	}

}
/******************************************************************************************************** 
**     函数名称:         Setting
**    功能描述:          设置界面
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void Setting(void)
{
	CPU_SR_ALLOC();
	vu8 key1 = 0,rkey = 0;
	u8 cout =1;
	OS_ERR err; 
	OSTaskDel((OS_TCB*)&OTHERTaskTCB,&err);		//挂起其他任务
	OSTaskDel((OS_TCB*)&KEYTaskTCB,&err);		//挂起其他任务
	if(languagesetting!= ENGLISH)
	{
		OLED_ShowCHineseString(48,0,shizhe,2); // 
		OLED_ShowCHineseString(32,1,timeandlanguage,5); //
		OLED_ShowString(32,2,(u8*)"GPS"); // 
		OLED_ShowCHineseString(56,2,dingwei,2); //
		OLED_ShowCHineseString(32,3,tuoluoyi,3); // 
		OLED_ShowCHineseString(80,3,shizhe,2); // 
		
	}else{
		OLED_ShowString(36,0,(u8 *)"SETTING");
		OLED_ShowString(32,1,(u8 *)"Time&&Lage");
		OLED_ShowString(32,2,(u8 *)"GPS Setting");
		OLED_ShowString(32,3,(u8 *)"MPU Setting");
	}
	OLED_DrawBMP(16,2,32,4,xuanzebmp);
	while(1)
	{
		key1 = KEY_Scan();
		rkey = Remote_Scan(SINGLE_PRES);
		if(rkey) key1 = rkey;
		if(key1)
		{
			if(key1 == KEY0_PRES || key1 == LEFT)   //上
			{
				if(cout>1) cout--;
					else cout =3;
			}
			else if(key1 == KEY1_PRES || key1 == RIGHT)  //下一
			{
				if(cout<3) cout++;	
				else cout = 1;
			}
			else if(key1 == KEY2_PRES || key1 == STOP)  //确认
			{
				prt(" setting confirm \r\n ");
				//OLED_Clear();
				switch(cout)
				{
					case 1:
					Time_and_Language_Set();
					break;
					case 2:
					GPS_Set();
					break;
					case 3:
					MPU_Set();				
					break;
				}
			}			
			else if(key1 == KEY3_PRES || key1 == EQ)  //按键取消
			{
				prt(" setting cancel \r\n");
				OLED_Clear();
				if(languagesetting!= ENGLISH)
				{
						OLED_ShowCHinese(32,1,set,2); // 设
						OLED_ShowCHinese(48,1,set,3); // 置
						OLED_ShowCHinese(64,1,set,6); // 
						OLED_ShowCHinese(80,1,set,7); // 
				}else{
						OLED_ShowString(12,1,(u8 *)"Setup Cancel!");
					}
				OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s					
				break;
			}
			switch(cout)
				{
					case 1:
					OLED_Fill(16,32,2);
					OLED_Fill(16,32,3);
					OLED_DrawBMP(16,2,32,4,xuanzebmp);
					break;
					case 2:
					OLED_Fill(16,32,1);
					OLED_Fill(16,32,3);
					OLED_DrawBMP(16,4,32,6,xuanzebmp);
					break;
					case 3:
					OLED_Fill(16,32,1);
					OLED_Fill(16,32,2);
					OLED_DrawBMP(16,6,32,8,xuanzebmp);
					break;
				}
			key1 = 0;
			rkey = 0;
		}
	}
	OLED_Clear();
	if(languagesetting!= ENGLISH)
	{
		OLED_ShowCHineseString(0,3,syz,3);//上一站
		OLED_ShowCHineseString(80,3,xyz,3);//下一站

	}
	else {
			OLED_ShowString(0,3,(u8 *)"Prev");
			OLED_ShowString(96,3,(u8 *)"Next");
	}
	ShowUpDownPos(key_time);
	OS_CRITICAL_ENTER();	
	ReCreateOTHERTask();
	ReCreateKEYTask();
	OS_CRITICAL_EXIT();
	syc = 0;
}

/******************************************************************************************************** 
**     函数名称:         Time_and_Language_Set
**    功能描述:          设置语言和显示时间
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void Time_and_Language_Set(void)
{
	vu8 key1 = 0,rkey = 0;
	u8 cout =1;
	OS_ERR err; 
	prt(" Time_and_Language_Set \r\n ");
	OLED_Clear();
	if(languagesetting!= ENGLISH)
	{
		OLED_ShowCHineseString(24,0,timeandlanguage,5); //
		OLED_ShowCHineseString(32,1,shijian,2); // shijian
		OLED_ShowCHineseString(32,2,set,2); // 语言
		
	}else{
		OLED_ShowString(24,0,(u8 *)"Time&&Lage");
		OLED_ShowString(32,1,(u8 *)"Time");
		OLED_ShowString(32,2,(u8 *)"Language");
	}
	OLED_DrawBMP(16,2,32,4,xuanzebmp);
	while(1)
	{
		key1 = KEY_Scan();
		rkey = Remote_Scan(SINGLE_PRES);
		if(rkey) key1 = rkey;
		if(key1)
		{
			if(key1 == KEY0_PRES || key1 == LEFT)   //上
			{
				if(cout>1) cout--;
					else cout =2;
			}
			else if(key1 == KEY1_PRES || key1 == RIGHT)  //下一
			{
				if(cout<2) cout++;	
				else cout = 1;
			}
			else if(key1 == KEY2_PRES || key1 == STOP)  //确认
			{
				prt(" Time_and_Language_Set confirm \r\n ");
				switch(cout)
				{
					case 1:
					SetTime();
					break;
					case 2:
					SetLanguage();
					break;
				}
			}			
			else if(key1 == KEY3_PRES || key1 == EQ)  //按键取消
			{
				prt(" Time_and_Language_Set cancel \r\n");
				OLED_Clear();
				if(languagesetting!= ENGLISH)
				{
					OLED_ShowCHineseString(48,0,shizhe,2); // 
					OLED_ShowCHineseString(32,1,timeandlanguage,5); //
					OLED_ShowString(32,2,(u8*)"GPS"); // 
					OLED_ShowCHineseString(56,2,dingwei,2); //
					OLED_ShowCHineseString(32,3,tuoluoyi,3); // 
					OLED_ShowCHineseString(80,3,shizhe,2); // 
					
				}else{
					OLED_ShowString(36,0,(u8 *)"SETTING");
					OLED_ShowString(32,1,(u8 *)"Time&&Lage");
					OLED_ShowString(32,2,(u8 *)"GPS Setting");
					OLED_ShowString(32,3,(u8 *)"MPU Setting");
				}
				OLED_DrawBMP(16,2,32,4,xuanzebmp);			
				break;
			}
			switch(cout)
				{
					case 1:
					OLED_Fill(16,32,2);
					OLED_Fill(16,32,3);
					OLED_DrawBMP(16,2,32,4,xuanzebmp);
					break;
					case 2:
					OLED_Fill(16,32,1);
					OLED_Fill(16,32,3);
					OLED_DrawBMP(16,4,32,6,xuanzebmp);
					break;
				}
			key1 = 0;
			rkey = 0;
		}
	}
}
static u8 flag;
void TEMP_task(void *p_arg)
{
	vu8 key1 = 0,rkey = 0;
	while(1)
	{
		key1 = KEY_Scan();
		rkey = Remote_Scan(SINGLE_PRES);
		if(rkey) key1 = rkey;
		if(key1)
		{
			 if(key1 == KEY3_PRES || key1 == EQ)  //按键取消
			{
				prt(" TEMP_task  cancel \r\n");		
				flag = 100;
			}
			key1 = 0;
			rkey = 0;
		}
	}
}
/******************************************************************************************************** 
**     函数名称:         SetTime
**    功能描述:         设置时间
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void SetTime(void)
{
	vu8 key1 = 0,rkey = 0;
	OS_ERR err; 
	CPU_SR_ALLOC();
	OS_TCB TEMPTaskTCB;
	//任务堆栈	
	CPU_STK TEMP_TASK_STK[64];
	prt(" SetTime \r\n ");
	
	OS_CRITICAL_ENTER();
	OSTaskCreate((OS_TCB 	* )&TEMPTaskTCB,
	 (CPU_CHAR	* )"TEMP task", 		
					 (OS_TASK_PTR )TEMP_task, 			
					 (void		* )0,					
					 (OS_PRIO	  )4u,     
					 (CPU_STK   * )&TEMP_TASK_STK[0],	
					 (CPU_STK_SIZE)64/10,	
					 (CPU_STK_SIZE)64,		
					 (OS_MSG_QTY  )0,					
					 (OS_TICK	  )0,					
					 (void   	* )0,					
					 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
					 (OS_ERR 	* )&err);
	OS_CRITICAL_EXIT();
	OLED_Clear();
	if(languagesetting!= ENGLISH)
	{
		OLED_ShowCHineseString(24,0,timeandlanguage,3); //
		OLED_ShowCHineseString(72,0,riqi,2); // shijian
		
	}else{
		OLED_ShowString(24,0,(u8 *)"Time&&Date");
	}
	while(1)
	{
		OS_CRITICAL_ENTER();
		sprintf(date1,"%u:%u:%u",Local_Time.year+1792,Local_Time.month,Local_Time.date);
		sprintf(time,"%u:%u:%u",Local_Time.hour,Local_Time.min,Local_Time.sec);
		OS_CRITICAL_EXIT();
		
		OS_CRITICAL_ENTER();	
		local_date.year = Local_Time.year;
		local_date.mon = Local_Time.month;
		local_date.day = Local_Time.date;
		OS_CRITICAL_EXIT();
		if(languagesetting !=  ENGLISH)
		{
			OLED_ShowNum(8,1,local_date.year+1792,4);
			OLED_ShowCHinese(40,1,nyr,0);
			OLED_ShowNum(56,1,local_date.mon,2);
			OLED_ShowCHinese(72,1,nyr,1);
			OLED_ShowNum(88,1,local_date.day,2);
			OLED_ShowCHinese(104,1,nyr,2);
		}
		 else
		{
			OLED_ShowString(0,1,(u8*)date1);
		}
		OS_CRITICAL_ENTER();
		if(Local_Time.sec < 2 ) //59到1s清屏
			OLED_Fill(96,128,2);
		OS_CRITICAL_EXIT();
		if(languagesetting !=  ENGLISH)
		{
			OLED_ShowCHineseString(8,2,shijian,3);
		}
		 else
		{
			OLED_ShowString(0,2,(u8*)"Time:");
		}
		OLED_ShowString(56,2,(u8*)time);
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
		
		if(flag == 100)
		{
			flag = 0;
			OLED_Clear();
			if(languagesetting!= ENGLISH)
			{
				OLED_ShowCHineseString(24,0,timeandlanguage,5); //
				OLED_ShowCHineseString(32,1,shijian,2); // shijian
				OLED_ShowCHineseString(32,2,set,2); // 语言
				
			}else{
				OLED_ShowString(24,0,(u8 *)"Time&&Lage");
				OLED_ShowString(32,1,(u8 *)"Time");
				OLED_ShowString(32,2,(u8 *)"Language");
			}
			OLED_DrawBMP(16,2,32,4,xuanzebmp);	
			
			OS_CRITICAL_ENTER();
			OSTaskDel((OS_TCB*)&TEMPTaskTCB,&err);		//删除其他任务
			OS_CRITICAL_EXIT();
			break;
		}
	}
	
}
/******************************************************************************************************** 
**     函数名称:         SetLanguage
**    功能描述:          系统语言设置
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void SetLanguage(void)
{
	CPU_SR_ALLOC();
	vu8 key1 = 0,rkey = 0;
	u8 cout =1;
	OS_ERR err; 
	OLED_Clear();
	if(languagesetting!= ENGLISH)
	{
		OLED_ShowCHineseString(32,0,set,4); // 语言设置：
		OLED_ShowCHinese(48,1,yuyan,0); // 中
		OLED_ShowCHinese(64,1,yuyan,1); // 文
		OLED_ShowCHinese(48,2,yuyan,2); // 英
		OLED_ShowCHinese(64,2,yuyan,3); // 语
		OLED_ShowCHinese(48,3,yuyan,4); // 混
		OLED_ShowCHinese(64,3,yuyan,5); // 合
	}else{
		OLED_ShowString(28,0,(u8 *)"langulage");
		OLED_ShowString(48,1,(u8 *)"CHinese");
		OLED_ShowString(48,2,(u8 *)"English");
		OLED_ShowString(48,3,(u8 *)"Mixing");
	}
	OLED_DrawBMP(16,2,32,4,xuanzebmp);
	while(1)
	{
		key1 = KEY_Scan();
		rkey = Remote_Scan(SINGLE_PRES);
		if(rkey) key1 = rkey;
		if(key1)
		{
			if(key1 == KEY0_PRES || key1 == LEFT)   //上
			{
				if(cout>1) cout--;
					else cout =3;
			}
			else if(key1 == KEY1_PRES || key1 == RIGHT)  //下一
			{
				if(cout<3) cout++;	
				else cout = 1;
			}
			else if(key1 == KEY2_PRES || key1 == STOP)  //确认
			{
				prt(" setting confirm");
				OLED_Clear();
				switch(cout)
				{
					case 1:
					if(languagesetting!= ENGLISH)
					{
						OLED_ShowCHinese(16,1,yuyan,0); // 中
						OLED_ShowCHinese(32,1,yuyan,1); // 文
						OLED_ShowCHinese(48,1,set,2); // 设
						OLED_ShowCHinese(64,1,set,3); // 置
						OLED_ShowCHinese(80,1,set,4); // 成
						OLED_ShowCHinese(96,1,set,5); // 功
					}else{
						OLED_ShowString(8,1,(u8 *)"CHinese Setup!");
					}
					languagesetting = 1;	
					
					break;
					case 2:
					if(languagesetting!= ENGLISH)
					{
						OLED_ShowCHinese(16,1,yuyan,2); // 
						OLED_ShowCHinese(32,1,yuyan,3); // 
						OLED_ShowCHinese(48,1,set,2); // 设
						OLED_ShowCHinese(64,1,set,3); // 置
						OLED_ShowCHinese(80,1,set,4); // 成
						OLED_ShowCHinese(96,1,set,5); // 功
					}else{
					
						OLED_ShowString(8,1,(u8 *)"English Setup!");
					}	
					languagesetting = 2;	
					break;
					case 3:
					
						if(languagesetting!= ENGLISH)
					{
						OLED_ShowCHinese(16,1,yuyan,4); // 
						OLED_ShowCHinese(32,1,yuyan,5); // 
						OLED_ShowCHinese(48,1,set,2); // 设
						OLED_ShowCHinese(64,1,set,3); // 置
						OLED_ShowCHinese(80,1,set,4); // 成
						OLED_ShowCHinese(96,1,set,5); // 功
					}else{
					
						OLED_ShowString(12,1,(u8 *)"Mixing Setup!");
					}	
					languagesetting = 3;						
					break;
				}
				OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
			}			
			 if(key1 == KEY3_PRES || key1 == EQ ||key1 == KEY2_PRES || key1 == STOP)  //按键取消
			{
				//prt(" language cancel");
				OLED_Clear();
				if(languagesetting!= ENGLISH)
				{
					OLED_ShowCHineseString(24,0,timeandlanguage,5); //
					OLED_ShowCHineseString(32,1,shijian,2); // shijian
					OLED_ShowCHineseString(32,2,set,2); // 语言
				}else{
					OLED_ShowString(24,0,(u8 *)"Time&&Lage");
					OLED_ShowString(32,1,(u8 *)"Time");
					OLED_ShowString(32,2,(u8 *)"Language");
				}
				OLED_DrawBMP(16,2,32,4,xuanzebmp);				
				break;
			}
			switch(cout)
				{
					case 1:
					OLED_Fill(16,32,2);
					OLED_Fill(16,32,3);
					OLED_DrawBMP(16,2,32,4,xuanzebmp);
					break;
					case 2:
					OLED_Fill(16,32,1);
					OLED_Fill(16,32,3);
					OLED_DrawBMP(16,4,32,6,xuanzebmp);
					break;
					case 3:
					OLED_Fill(16,32,1);
					OLED_Fill(16,32,2);
					OLED_DrawBMP(16,6,32,8,xuanzebmp);
					break;
				}
			key1 = 0;
			rkey = 0;

		}
	}
}
/******************************************************************************************************** 
**     函数名称:         GPS_Set
**    功能描述:          GPS定位设置
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void GPS_Set(void)
{
	vu8 key1 = 0,rkey = 0;
	u8 cout =1;
	OS_ERR err; 
	prt(" GPS_Set \r\n ");
	OLED_Clear();
	if(languagesetting!= ENGLISH)
	{
		OLED_ShowString(36,0,(u8 *)"GPS");
		OLED_ShowCHineseString(60,0,shizhe,2); // 设置
		OLED_ShowCHineseString(32,1,dakai,2); // 
		OLED_ShowCHineseString(32,2,guanbi,2); // 
		OLED_ShowCHineseString(32,3,jingweidu,5); // 
		
	}else{
		OLED_ShowString(20,0,(u8 *)"GPS Setting");
		OLED_ShowString(32,1,(u8 *)"Open");
		OLED_ShowString(32,2,(u8 *)"Close");
		OLED_ShowString(32,3,(u8 *)"GPS Data");
	}
	OLED_DrawBMP(16,2,32,4,xuanzebmp);
	while(1)
	{
		key1 = KEY_Scan();
		rkey = Remote_Scan(SINGLE_PRES);
		if(rkey) key1 = rkey;
		if(key1)
		{
			if(key1 == KEY0_PRES || key1 == LEFT)   //上
			{
				if(cout>1) cout--;
					else cout =3;
			}
			else if(key1 == KEY1_PRES || key1 == RIGHT)  //下一
			{
				if(cout<3) cout++;	
				else cout = 1;
			}
			else if(key1 == KEY2_PRES || key1 == STOP)  //确认
			{
				prt(" GPS confirm \r\n ");
				OLED_Clear();
				switch(cout)
				{
					case 1:
					GPS_Open();
					if(languagesetting!= ENGLISH)
					{
						OLED_ShowString(36,1,(u8 *)"GPS");
						OLED_ShowCHineseString(60,1,dakai,2); // 
					}else{
					
						OLED_ShowString(28,1,(u8 *)"GPS OPEN");
					}
					OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
					break;
					case 2:
					GPS_Close();
					if(languagesetting!= ENGLISH)
					{
						OLED_ShowString(36,1,(u8 *)"GPS");
						OLED_ShowCHineseString(60,1,guanbi,2); // 
					}else{
					
						OLED_ShowString(28,1,(u8 *)"GPS CLOSE");
					}
					OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
					break;
					case 3:
					ShowGPSData();
					break;
				}		
			}			
			 if(key1 == KEY3_PRES || key1 == EQ ||( key1 == KEY2_PRES && cout != 3)|| (key1 == STOP && cout != 3))  //按键取消
			{
				prt(" Time_and_Language_Set cancel \r\n");
				OLED_Clear();
				if(languagesetting!= ENGLISH)
				{
					OLED_ShowCHineseString(48,0,shizhe,2); // 
					OLED_ShowCHineseString(32,1,timeandlanguage,5); //
					OLED_ShowString(32,2,(u8*)"GPS"); // 
					OLED_ShowCHineseString(56,2,dingwei,2); //
					OLED_ShowCHineseString(32,3,tuoluoyi,3); // 
					OLED_ShowCHineseString(80,3,shizhe,2); // 
					
				}else{
					OLED_ShowString(36,0,(u8 *)"SETTING");
					OLED_ShowString(32,1,(u8 *)"Time&&Lage");
					OLED_ShowString(32,2,(u8 *)"GPS Setting");
					OLED_ShowString(32,3,(u8 *)"MPU Setting");
				}
				OLED_DrawBMP(16,2,32,4,xuanzebmp);			
				break;
			}
			switch(cout)
				{
					case 1:
					OLED_Fill(16,32,2);
					OLED_Fill(16,32,3);
					OLED_DrawBMP(16,2,32,4,xuanzebmp);
					break;
					case 2:
					OLED_Fill(16,32,1);
					OLED_Fill(16,32,3);
					OLED_DrawBMP(16,4,32,6,xuanzebmp);
					break;
					case 3:
					OLED_Fill(16,32,1);
					OLED_Fill(16,32,2);
					OLED_DrawBMP(16,6,32,8,xuanzebmp);
					break;
				}
			key1 = 0;
			rkey = 0;

		}
	}
}
/******************************************************************************************************** 
**     函数名称:         ShowGPSData
**    功能描述:         设置时间
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void ShowGPSData(void)
{
	vu8 key1 = 0,rkey = 0;
	OS_ERR err; 
	char latitude[12] = {0};
	char longtitude[12] = {0};
	CPU_SR_ALLOC();
	OS_TCB TEMPTaskTCB;
	//任务堆栈	
	CPU_STK TEMP_TASK_STK[64];
	
	OS_CRITICAL_ENTER();
	OSTaskCreate(  (OS_TCB      * )&TEMPTaskTCB,
	 (CPU_CHAR	* )"TEMP task", 		
					 (OS_TASK_PTR )TEMP_task, 			
					 (void		* )0,					
					 (OS_PRIO	  )4u,     
					 (CPU_STK   * )&TEMP_TASK_STK[0],	
					 (CPU_STK_SIZE)64/10,	
					 (CPU_STK_SIZE)64,		
					 (OS_MSG_QTY  )0,					
					 (OS_TICK	  )0,					
					 (void   	* )0,					
					 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
					 (OS_ERR 	* )&err);
	OS_CRITICAL_EXIT();
	if(languagesetting!= ENGLISH)
	{
		OLED_ShowCHineseString(24,0,jingweidu,5); //
		OLED_ShowCHineseString(0,1,jingdu,2); //
		OLED_ShowCHineseString(0,2,weidu,2); //
		
	}else{
		OLED_ShowString(32,0,(u8 *)"GPS Data");
		OLED_ShowString(0,1,(u8 *)"LONG");
		OLED_ShowString(0,2,(u8 *)"LAT");
	}
	OLED_ShowChar(32,1,':');
	OLED_ShowChar(32,2,':');
	while(1)
	{
		OS_CRITICAL_ENTER();
		if(GPGGA_OK_FLAG)
		{
			sprintf(latitude,"%.5f",GPS_Msg.latitude_val );
			sprintf(longtitude,"%.5f",GPS_Msg.longitude_val);
			GPGGA_OK_FLAG = 0;
		}
		OS_CRITICAL_EXIT();
		OLED_ShowString(40,1,(u8 *)longtitude);
		OLED_ShowString(40,2,(u8 *)latitude);
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
		
		if(flag == 100)
		{
			flag = 0;
			OLED_Clear();
			if(languagesetting!= ENGLISH)
			{
				OLED_ShowString(36,0,(u8 *)"GPS");
				OLED_ShowCHineseString(60,0,shizhe,2); // 设置
				OLED_ShowCHineseString(32,1,dakai,2); // 
				OLED_ShowCHineseString(32,2,guanbi,2); // 
				OLED_ShowCHineseString(32,3,jingweidu,5); // 
				
			}else{
				OLED_ShowString(20,0,(u8 *)"GPS Setting");
				OLED_ShowString(32,1,(u8 *)"Open");
				OLED_ShowString(32,2,(u8 *)"Close");
				OLED_ShowString(32,3,(u8 *)"GPS Data");
			}
			OLED_DrawBMP(16,2,32,4,xuanzebmp);	
			
			OS_CRITICAL_ENTER();
			OSTaskDel((OS_TCB*)&TEMPTaskTCB,&err);		//删除其他任务
			OS_CRITICAL_EXIT();
			break;
		}
	}	
}
/******************************************************************************************************** 
**     函数名称:         GPS_Open
**    功能描述:         打开GPS任务
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void GPS_Open(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	OS_CRITICAL_ENTER();
	OSTaskDel((OS_TCB*)&GPS_DATADEALTaskTCB,&err);		//删除其他任务
	OSTaskDel((OS_TCB*)&GPSTaskTCB,&err);		//删除其他任务
#if GPS_DATADEALTaskTCB_CONFIG	
	OSTaskCreate((OS_TCB 	* )&GPS_DATADEALTaskTCB,		   //处理GPS数据
			 (CPU_CHAR	* )"GPS_DATADEAL task", 		
							 (OS_TASK_PTR )GPS_DATADEAL_task, 			
							 (void		* )0,					
							 (OS_PRIO	  )GPS_DATADEAL_TASK_PRIO,     
							 (CPU_STK   * )&GPS_DATADEAL_TASK_STK[0],	
							 (CPU_STK_SIZE)GPS_DATADEAL_STK_SIZE/10,	
							 (CPU_STK_SIZE)GPS_DATADEAL_STK_SIZE,		
							 (OS_MSG_QTY  )0,					
							 (OS_TICK	  )0,					
							 (void   	* )0,					
							 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
							 (OS_ERR 	* )&err);	
#endif
#if GPSTaskTCB_CONFIG						
	OSTaskCreate((OS_TCB 	* )&GPSTaskTCB,		   //GPS报站任务
			 (CPU_CHAR	* )"GPS task", 		
							 (OS_TASK_PTR )GPS_task, 			
							 (void		* )0,					
							 (OS_PRIO	  )GPS_TASK_PRIO,     
							 (CPU_STK   * )&GPS_TASK_STK[0],	
							 (CPU_STK_SIZE)GPS_STK_SIZE/10,	
							 (CPU_STK_SIZE)GPS_STK_SIZE,		
							 (OS_MSG_QTY  )0,					
							 (OS_TICK	  )0,					
							 (void   	* )0,					
							 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
							 (OS_ERR 	* )&err);		
#endif
		OS_CRITICAL_EXIT();
}

/******************************************************************************************************** 
**     函数名称:         GPS_Close
**    功能描述:         关闭GPS任务
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void GPS_Close(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	OS_CRITICAL_ENTER();
	OSTaskDel((OS_TCB*)&GPS_DATADEALTaskTCB,&err);	//删除其他任务
	OSTaskDel((OS_TCB*)&GPSTaskTCB,&err);		//删除其他任务
	OS_CRITICAL_EXIT();
}
/******************************************************************************************************** 
**     函数名称:         MPU_Set
**    功能描述:         陀螺仪功能设置
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void MPU_Set(void)
{
	vu8 key1 = 0,rkey = 0;
	u8 cout =1;
	OS_ERR err; 
	prt(" GPS_Set \r\n ");
	OLED_Clear();
	if(languagesetting!= ENGLISH)
	{
		OLED_ShowCHineseString(24,0,tuoluoyi,3); // 
		OLED_ShowCHineseString(72,0,shizhe,2); // 设置
		OLED_ShowCHineseString(32,1,dakai,2); // 
		OLED_ShowCHineseString(32,2,guanbi,2); // 
		
	}else{
		OLED_ShowString(20,0,(u8 *)"MPU Setting");
		OLED_ShowString(32,1,(u8 *)"Open");
		OLED_ShowString(32,2,(u8 *)"Close");
	}
	OLED_DrawBMP(16,2,32,4,xuanzebmp);
	while(1)
	{
		key1 = KEY_Scan();
		rkey = Remote_Scan(SINGLE_PRES);
		if(rkey) key1 = rkey;
		if(key1)
		{
			if(key1 == KEY0_PRES || key1 == LEFT)   //上
			{
				if(cout>1) cout--;
					else cout =2;
			}
			else if(key1 == KEY1_PRES || key1 == RIGHT)  //下一
			{
				if(cout<2) cout++;	
				else cout = 1;
			}
			else if(key1 == KEY2_PRES || key1 == STOP)  //确认
			{
				prt(" mpu confirm \r\n ");
				OLED_Clear();
				switch(cout)
				{
					case 1:
					MPU_Open();
					if(languagesetting!= ENGLISH)
					{
						OLED_ShowCHineseString(24,1,tuoluoyi,3); // 
						OLED_ShowCHineseString(72,1,dakai,2); // 
					}else{
					
						OLED_ShowString(28,1,(u8 *)"MPU OPEN");
					}
					OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
					break;
					case 2:
					MPU_Close();
					if(languagesetting!= ENGLISH)
					{
						OLED_ShowCHineseString(24,1,tuoluoyi,3); // 
						OLED_ShowCHineseString(72,1,guanbi,2); // 
					}else{
					
						OLED_ShowString(28,1,(u8 *)"MPU CLOSE");
					}
					OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
					break;
				}
				
			}			
			 if(key1 == KEY3_PRES || key1 == EQ || key1 == KEY2_PRES || key1 == STOP)  //按键取消
			{
				prt(" MPU  cancel \r\n");
				OLED_Clear();
				if(languagesetting!= ENGLISH)
				{
					OLED_ShowCHineseString(48,0,shizhe,2); // 
					OLED_ShowCHineseString(32,1,timeandlanguage,5); //
					OLED_ShowString(32,2,(u8*)"GPS"); // 
					OLED_ShowCHineseString(56,2,dingwei,2); //
					OLED_ShowCHineseString(32,3,tuoluoyi,3); // 
					OLED_ShowCHineseString(80,3,shizhe,2); // 
					
				}else{
					OLED_ShowString(36,0,(u8 *)"SETTING");
					OLED_ShowString(32,1,(u8 *)"Time&&Lage");
					OLED_ShowString(32,2,(u8 *)"GPS Setting");
					OLED_ShowString(32,3,(u8 *)"MPU Setting");
				}
				OLED_DrawBMP(16,2,32,4,xuanzebmp);			
				break;
			}
			switch(cout)
				{
					case 1:
					OLED_Fill(16,32,2);
					OLED_Fill(16,32,3);
					OLED_DrawBMP(16,2,32,4,xuanzebmp);
					break;
					case 2:
					OLED_Fill(16,32,1);
					OLED_Fill(16,32,3);
					OLED_DrawBMP(16,4,32,6,xuanzebmp);
					break;
				}
			key1 = 0;
			rkey = 0;

		}
	}
}
/******************************************************************************************************** 
**     函数名称:         MPU_Open
**    功能描述:         打开陀螺仪任务
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void MPU_Open(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	OS_CRITICAL_ENTER();
	OSTaskDel((OS_TCB*)&MPU6050TaskTCB,&err);		//删除其他任务
#if MPU6050TaskTCB_CONFIG
				OSTaskCreate((OS_TCB 	* )&MPU6050TaskTCB,		//姿态角采集
				 (CPU_CHAR	* )"DHT11 task", 		
								 (OS_TASK_PTR )MPU6050_task, 			
								 (void		* )0,					
								 (OS_PRIO	  )MPU6050_TASK_PRIO,     
								 (CPU_STK   * )&MPU6050_TASK_STK[0],	
								 (CPU_STK_SIZE)MPU6050_STK_SIZE/10,	
								 (CPU_STK_SIZE)MPU6050_STK_SIZE,		
								 (OS_MSG_QTY  )0,					
								 (OS_TICK	  )0,					
								 (void   	* )0,					
								 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
								 (OS_ERR 	* )&err);	
#endif
		OS_CRITICAL_EXIT();
}

/******************************************************************************************************** 
**     函数名称:         MPU_Close
**    功能描述:         关闭陀螺仪任务
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void MPU_Close(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	OS_CRITICAL_ENTER();
	OSTaskDel((OS_TCB*)&MPU6050TaskTCB,&err);		//删除其他任务
	OS_CRITICAL_EXIT();
}