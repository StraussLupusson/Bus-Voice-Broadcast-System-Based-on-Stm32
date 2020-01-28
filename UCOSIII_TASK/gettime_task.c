/*
	�ļ�����      gettime_task.c
	�ļ������� gettime_task������ʵ��
	�޸�ʱ�䣺 2019��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/
#include "gettime_task.h"
#include "sys.h"
#include "usart.h"
#include "gps.h"
#include "start_task.h"
//������ƿ�
OS_TCB GETTIMETaskTCB;
//�����ջ	
CPU_STK GETTIME_TASK_STK[GETTIME_STK_SIZE];
local_time Local_Time;
vu32 time_cout;
void GetTime_TIM3Init(u16 msec);
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);
u8 RTC_Get(void);
u8 RTC_Get_Week(u16 year,u8 month,u8 day);
/******************************************************************************************************** 
**     ��������:         GETTIME_task
**    ��������:         GETTIME������
**    ��ڲ�����       (void *p_arg)
**    ���ڲ���:          ��
**    ����˵����       ��
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
			TIM_Cmd(TIM3, ENABLE);  //������ʱ��TIM2��ʱ
			USART_Cmd(GPS_USART, DISABLE); //�Ѵ���2�ر�
			OS_CRITICAL_EXIT();
			Get_Time_OK = 0;
		}
		OS_CRITICAL_ENTER();
		RTC_Get();	
		OS_CRITICAL_EXIT();
		prt("TIME_COUT %d \r\n",time_cout);
		
		//prt("20%u�� %u�� %u��  \r\n",GPS_Msg.LocalTime.year,GPS_Msg.LocalTime.month,GPS_Msg.LocalTime.date);
		prt("%uʱ %u�� %u�� \r\n",Local_Time.hour,Local_Time.min,Local_Time.sec);
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ200ms
		
	}
}
		   
/******************************************************************************************************** 
**     ��������:         GetTime_TIM3Init
**    ��������:          ��ʱ��TIM3��ʼ��
**                     ����1��¼����ʱ��ȷ��¼����ַ
						����2: ����ʱ���ʱ
**    ��ڲ�����       	 u16 msec����ʱ����msec΢��
**    ���ڲ���:          ��
**    ����˵����		fl03��ͨ��ʱ����ʼ����2 - 7Timer
						Tout= ((TIM_Period+1)*(TIM_Prescaler+1))/Tclk
********************************************************************************************************/ 
void GetTime_TIM3Init(u16 msec)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period  = msec*10-1;; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =7200-1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
	TIM_Cmd(TIM3, DISABLE);  
					 
}

/******************************************************************************************************** 
**     ��������:         TIM3_IRQHandler
**    ��������:           ��ʱ��TIM3�жϷ�����
**    ��ڲ�����       	 ��
**    ���ڲ���:          ��
********************************************************************************************************/ 
void TIM3_IRQHandler(void)   //TIM3�ж�
{
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
		{
			time_cout++;	
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		}
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
OSIntExit();  											 
#endif
}
	
/******************************************************************************************************** 
**     ��������:       RTC_Set
**    ��������:        �������ʱ��ת��Ϊ����
**    ��ڲ�����       u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec
**    ���ڲ���:        ����ֵ:0,�ɹ�;����:�������.
**    ����˵����       ��1970��1��1��Ϊ��׼ 1970~2099��Ϊ�Ϸ����
********************************************************************************************************/ 										 
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; ////�·����ݱ� 
//ƽ����·����ڱ�
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
	u16 t;
	u32 seccount=0;
	if(syear<1970||syear>2099)return 1;	   
	for(t=1970;t<syear;t++)	//��������ݵ��������
	{
		if(IsLeapYear(t))seccount+=31622400;//�����������
		else seccount+=31536000;			  //ƽ���������
	}
	smon-=1;
	for(t=0;t<smon;t++)	   //��ǰ���·ݵ����������
	{
		seccount+=(u32)mon_table[t]*86400;//�·����������
		if(IsLeapYear(syear)&&t==1)seccount+=86400;//����2�·�����һ���������	   
	}
	seccount+=(u32)(sday-1)*86400;//��ǰ�����ڵ���������� 
	seccount+=(u32)hour*3600;//Сʱ������
    seccount+=(u32)min*60;	 //����������
	seccount+=sec;//�������Ӽ���ȥ
	time_cout = seccount;
	TIM_Cmd(TIM3, ENABLE);  
	
	return 0;	    
}


/******************************************************************************************************** 
**     ��������:         RTC_Get
**    ��������:         �õ���ǰ��ʱ��
**    ��ڲ�����       ��
**    ���ڲ���:          ��
**    ����˵����       ��
********************************************************************************************************/ 
u8 RTC_Get(void)
{
	static u16 daycnt=0;
	u32 temp=0;
	u16 temp1=0;	  
 	temp=time_cout/86400;   //�õ�����(��������Ӧ��)
	if(daycnt!=temp)//����һ����
	{	  
		daycnt=temp;
		temp1=1970;	//��1970�꿪ʼ
		while(temp>=365)
		{				 
			if(IsLeapYear(temp1))//������
			{
				if(temp>=366)temp-=366;//�����������
				else {temp1++;break;}  
			}
			else temp-=365;	  //ƽ�� 
			temp1++;  
		}   
		Local_Time.year=temp1;//�õ����
		temp1=0;
		while(temp>=28)//������һ����
		{
			if(IsLeapYear(Local_Time.year)&&temp1==1)//�����ǲ�������/2�·�
			{
				if(temp>=29)temp-=29;//�����������
				else break; 
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//ƽ��
				else break;
			}
			temp1++;  
		}
		Local_Time.month=temp1+1;	//�õ��·�
		Local_Time.date=temp+1;  	//�õ����� 
	}
	temp=time_cout%86400;     		//�õ�������   	   
	Local_Time.hour=temp/3600;     	//Сʱ
	Local_Time.min=(temp%3600)/60; 	//����	
	Local_Time.sec=(temp%3600)%60; 	//����
	Local_Time.week=RTC_Get_Week(Local_Time.year,Local_Time.month,Local_Time.date);//��ȡ����   
	return 0;
}	 

/******************************************************************************************************** 
**     ��������:         RTC_Get_Week
**    ��������:          �������  ���빫�����ڵõ�����(ֻ����1901-2099��)
**    ��ڲ�����       ����������
**    ���ڲ���:          ���ں�
**    ����˵����       ��
********************************************************************************************************/ 																					 
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{	
	u16 temp2;
	u8 yearH,yearL;
	
	yearH=year/100;	yearL=year%100; 
	// ���Ϊ21����,�������100  
	if (yearH>19)yearL+=100;
	// ����������ֻ��1900��֮���  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}			  

	