/*
	文件名：      gettime_task.c
	文件描述： gettime_task任务函数实现
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/
#include "gettime_task.h"
#include "sys.h"
#include "usart.h"
#include "gps.h"
#include "start_task.h"
//任务控制块
OS_TCB GETTIMETaskTCB;
//任务堆栈	
CPU_STK GETTIME_TASK_STK[GETTIME_STK_SIZE];
local_time Local_Time;
vu32 time_cout;
void GetTime_TIM3Init(u16 msec);
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);
u8 RTC_Get(void);
u8 RTC_Get_Week(u16 year,u8 month,u8 day);
/******************************************************************************************************** 
**     函数名称:         GETTIME_task
**    功能描述:         GETTIME任务函数
**    入口参数：       (void *p_arg)
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void GETTIME_task(void *p_arg)
{
	OS_ERR err;
	u8 Get_Time_OK = 1;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	prt("GETTIME  start \r\n");
	GPS_Init();
	GetTime_TIM3Init(GETTIME_TIME3_FREQ);
	TIM_Cmd(TIM3, DISABLE);  
	while(1)
	{
		if(Get_Time_OK)
		{
			GPS_GetTime();
			OS_CRITICAL_ENTER();
			RTC_Set(GPS_Msg.LocalTime.year+2000,GPS_Msg.LocalTime.month,GPS_Msg.LocalTime.date,
					GPS_Msg.LocalTime.hour,GPS_Msg.LocalTime.min,GPS_Msg.LocalTime.sec);
			TIM_Cmd(TIM3, ENABLE);  //开启定时器TIM2计时
			USART_Cmd(GPS_USART, DISABLE); //把串口2关闭
			OS_CRITICAL_EXIT();
			Get_Time_OK = 0;
		}
		OS_CRITICAL_ENTER();
		RTC_Get();	
		OS_CRITICAL_EXIT();
		prt("TIME_COUT %d \r\n",time_cout);
		
		//prt("20%u年 %u月 %u日  \r\n",GPS_Msg.LocalTime.year,GPS_Msg.LocalTime.month,GPS_Msg.LocalTime.date);
		prt("%u时 %u分 %u秒 \r\n",Local_Time.hour,Local_Time.min,Local_Time.sec);
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时200ms
		
	}
}
		   
/******************************************************************************************************** 
**     函数名称:         GetTime_TIM3Init
**    功能描述:          定时器TIM3初始化
**                     功能1：录音计时，确定录音地址
						功能2: 用于时间计时
**    入口参数：       	 u16 msec，计时周期msec微妙
**    出口参数:          无
**    其他说明：		fl03普通定时器初始化，2 - 7Timer
						Tout= ((TIM_Period+1)*(TIM_Prescaler+1))/Tclk
********************************************************************************************************/ 
void GetTime_TIM3Init(u16 msec)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period  = msec*10-1;; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =7200-1; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
	TIM_Cmd(TIM3, DISABLE);  
					 
}

/******************************************************************************************************** 
**     函数名称:         TIM3_IRQHandler
**    功能描述:           定时器TIM3中断服务函数
**    入口参数：       	 无
**    出口参数:          无
********************************************************************************************************/ 
void TIM3_IRQHandler(void)   //TIM3中断
{
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
		{
			time_cout++;	
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx更新中断标志 
		}
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
OSIntExit();  											 
#endif
}
	
/******************************************************************************************************** 
**     函数名称:       RTC_Set
**    功能描述:        把输入的时钟转换为秒钟
**    入口参数：       u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec
**    出口参数:        返回值:0,成功;其他:错误代码.
**    其他说明：       以1970年1月1日为基准 1970~2099年为合法年份
********************************************************************************************************/ 										 
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; ////月份数据表 
//平年的月份日期表
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
	u16 t;
	u32 seccount=0;
	if(syear<1970||syear>2099)return 1;	   
	for(t=1970;t<syear;t++)	//把所有年份的秒钟相加
	{
		if(IsLeapYear(t))seccount+=31622400;//闰年的秒钟数
		else seccount+=31536000;			  //平年的秒钟数
	}
	smon-=1;
	for(t=0;t<smon;t++)	   //把前面月份的秒钟数相加
	{
		seccount+=(u32)mon_table[t]*86400;//月份秒钟数相加
		if(IsLeapYear(syear)&&t==1)seccount+=86400;//闰年2月份增加一天的秒钟数	   
	}
	seccount+=(u32)(sday-1)*86400;//把前面日期的秒钟数相加 
	seccount+=(u32)hour*3600;//小时秒钟数
    seccount+=(u32)min*60;	 //分钟秒钟数
	seccount+=sec;//最后的秒钟加上去
	time_cout = seccount;
	TIM_Cmd(TIM3, ENABLE);  
	
	return 0;	    
}


/******************************************************************************************************** 
**     函数名称:         RTC_Get
**    功能描述:         得到当前的时间
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
u8 RTC_Get(void)
{
	static u16 daycnt=0;
	u32 temp=0;
	u16 temp1=0;	  
 	temp=time_cout/86400;   //得到天数(秒钟数对应的)
	if(daycnt!=temp)//超过一天了
	{	  
		daycnt=temp;
		temp1=1970;	//从1970年开始
		while(temp>=365)
		{				 
			if(IsLeapYear(temp1))//是闰年
			{
				if(temp>=366)temp-=366;//闰年的秒钟数
				else {temp1++;break;}  
			}
			else temp-=365;	  //平年 
			temp1++;  
		}   
		Local_Time.year=temp1;//得到年份
		temp1=0;
		while(temp>=28)//超过了一个月
		{
			if(IsLeapYear(Local_Time.year)&&temp1==1)//当年是不是闰年/2月份
			{
				if(temp>=29)temp-=29;//闰年的秒钟数
				else break; 
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//平年
				else break;
			}
			temp1++;  
		}
		Local_Time.month=temp1+1;	//得到月份
		Local_Time.date=temp+1;  	//得到日期 
	}
	temp=time_cout%86400;     		//得到秒钟数   	   
	Local_Time.hour=temp/3600;     	//小时
	Local_Time.min=(temp%3600)/60; 	//分钟	
	Local_Time.sec=(temp%3600)%60; 	//秒钟
	Local_Time.week=RTC_Get_Week(Local_Time.year,Local_Time.month,Local_Time.date);//获取星期   
	return 0;
}	 

/******************************************************************************************************** 
**     函数名称:         RTC_Get_Week
**    功能描述:          获得星期  输入公历日期得到星期(只允许1901-2099年)
**    入口参数：       公历年月日
**    出口参数:          星期号
**    其他说明：       无
********************************************************************************************************/ 																					 
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{	
	u16 temp2;
	u8 yearH,yearL;
	
	yearH=year/100;	yearL=year%100; 
	// 如果为21世纪,年份数加100  
	if (yearH>19)yearL+=100;
	// 所过闰年数只算1900年之后的  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}			  

	