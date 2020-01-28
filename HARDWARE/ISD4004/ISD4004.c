/*
	文件名： ISD4004.c
	文件描述：STM32F03C8T6---ISD4004 Device Driver
	修改时间：2019年4月21日
	作者：汪武林
	其他说明：无
*/

#include "ISD4004.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "task_config.h"
//程序中的一些常量定义
vu32    time_total;
vu32    st_add,end_add=0;
vu32    adds[VOICE_PASSAGE_NUM];//25段语音的起始地址暂存
vu32    adde[VOICE_PASSAGE_NUM];//25段语音的结束地址暂时
vu8   count,count_flag,flag2,flag3,flag4;
vu8 request;

/******************************************************************************************************** 
**     函数名称:         ISD4004_Init
**    功能描述:          初始化ISD4004相关GPIO引脚
**    入口参数：       	 无
**    出口参数:          无
********************************************************************************************************/ 
void ISD4004_Init(void)
{
	//端口配置初始化
 	GPIO_InitTypeDef  GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(ISD4004_GPIO_CLK, ENABLE);	 //使能A端口时钟

	GPIO_InitStructure.GPIO_Pin =  ISD4004_SS_PIN | ISD4004_MOSI_PIN |ISD4004_SCLK_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ISD4004_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  ISD4004_INT_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	
	GPIO_Init(ISD4004_GPIO_PORT, &GPIO_InitStructure);
	
	//布线需要 PB0 接地
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//录放音按键初始化
//#if ISD4004_RECORDTaskTCB_CONFIG
	KEY_Init();
	flag3=0;
	flag4=0;
	time_total=ISD4004_START_ADDR*2;//录音地址从170开始，对应的单片机开始计时的时间就是340*0.1秒
	adds[0]=ISD4004_START_ADDR;
	count=0;
	ISD4004_TIM4Init(TIME4_FREQ);
	//#endif
}
/******************************************************************************************************** 
**     函数名称:         ISD4004_TIM4Init
**    功能描述:          定时器TIM4初始化
**                     功能1：录音计时，确定录音地址
						功能2: 用于时间计时
**    入口参数：       	 u16 msec，计时周期msec微妙
**    出口参数:          无
**    其他说明：		fl03普通定时器初始化，2 - 7Timer
						Tout= ((TIM_Period+1)*(TIM_Prescaler+1))/Tclk
********************************************************************************************************/ 

void ISD4004_TIM4Init(u16 msec)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能
	
	//定时器TIM4初始化
	TIM_TimeBaseStructure.TIM_Period  = msec*10-1;; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =7200-1; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM4中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM4, DISABLE);  
					 
}

//#if ISD4004_RECORDTaskTCB_CONFIG
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
/******************************************************************************************************** 
**     函数名称:         TIM4_IRQHandler
**    功能描述:           定时器TIM4中断服务函数
**    入口参数：       	 无
**    出口参数:          无
********************************************************************************************************/ 
void TIM4_IRQHandler(void)   //TIM4中断
{
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //检查TIM4更新中断发生与否
		{
			time_total++;
			if(time_total==4800)time_total=0; 	
			//prt("time 1s");
			TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //清除TIMx更新中断标志 
		
		}
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
OSIntExit();  											 
#endif
}
//#endif

/******************************************************************************************************** 
**     函数名称:         ISD4004_RecordingTreat
**    功能描述:         录音键处理程序
**    入口参数：       	 无
**    出口参数:          无
********************************************************************************************************/ 
#if ISD4004_RECORDTaskTCB_CONFIG
void ISD4004_RecordingTreat(void)
{
	//prt("ISD4004_RecordingTreat start\r\n");
   SW_RECORD=1;//置IO口为1，准备读入数据
   delay_us(1);
   if(SW_RECORD==0)
   {
      if(flag3==0)//录音键和放音键互锁，录音好后，禁止再次录音。如果要再次录音，那就要复位单片机，重新开始录音
      {
        if(count==0)//判断是否为上电或复位以来第一次按录音键
        {
           st_add= ISD4004_START_ADDR;
        }
        else
        {
          st_add=end_add + ISD4004_ADDR_INTERVAL; 
        }//每段语言间隔3个地址
        adds[count]=st_add;//每段语音的起始地址暂时
        if(count>=VOICE_PASSAGE_NUM)//判断语音段数时候超过25段
        {
           count=0;
           st_add = ISD4004_START_ADDR;
           time_total=ISD4004_START_ADDR*2;
        }
        ISD4004_powerup(); //AN键按下，ISD上电并延迟50ms
		ISD4004_stopwrdn();
		ISD4004_powerup(); 
        
    	ISD4004_setrec(st_add&0x00ff,st_add>>8); //从指定的地址
        if(INT==1)// 判定芯片有没有溢出
        {		
            ISD4004_rec(); //发送录音指令
        }
        time_total=st_add*2;//计时初始值计算
        TIM_Cmd(TIM4, ENABLE);  //使能TIMx	//开计时器
        while(SW_RECORD==0);//等待本次录音结束
        TIM_Cmd(TIM4, DISABLE);  //使能TIMx	//录音结束后停止计时
        ISD4004_stop(); //发送4004停止命令
        end_add=time_total/2+2;//计算语音的结束地址
        adde[count]=end_add;//本段语音结束地址暂存
        count++;//录音段数自加
        count_flag=count;//录音段数寄存
        flag2=1;
        flag4=1;//解锁放音键
      }
  }
}
#endif
/******************************************************************************************************** 
**     函数名称:         ISD4004_PlayingTreat
**    功能描述:         放音机处理程序
**    入口参数：       	 无
**    出口参数:          无
********************************************************************************************************/ 
#if ISD4004_RECORDTaskTCB_CONFIG
void ISD4004_PlayingTreat(void)
{
	//prt("ISD4004_PlayingTreat start\r\n");
  // uchar ovflog;
   AN=1;//准备读入数据
   delay_us(1);
   if(AN==0)//判断放音键是否动作
   {
		prt("AN==0\r\n");
     if(flag4==1)//互锁录音键
     {
		prt("flag4==1\r\n");
        if(flag2==1)//判断是否为录音好后的第一次放音
        {
		prt("flag2==1\r\n");
           count=0;//从第0段开始播放
        }
		
        ISD4004_powerup(); //AN键按下，ISD上电并延迟50ms
		ISD4004_stopwrdn();
		ISD4004_powerup(); 
        st_add=adds[count];//送当前语音的起始地址
 		ISD4004_setplay(st_add&0x00ff,st_add>>8); //发送setplay指令，从指定地址开始放音
		ISD4004_play(); //发送放音指令
		delay_us(20);
		prt("ISD4004_play \r\n");
		while(INT==1); //等待放音完毕的EOM中断信号
		prt("放音完毕 \r\n");
		ISD4004_stop(); //放音完毕，发送stop指令
		prt("发送stop指令 \r\n");
		while(AN==0); // 
		prt("while(AN==0) \r\n");
		ISD4004_stop();
        count++;//语音段数自加
        flag2=0;
        flag3=1;
        if(count>=count_flag)//如果播放到最后一段后还按加键，则从第一段重新播放
        {
             count=0;
        }
     
      }
   } 
}
#endif
/******************************************************************************************************** 
**     函数名称:         ISD4004_Recording
**    功能描述:          指定地址录音
**    入口参数：       	 u16 st_addr，指定录音开始地址
**    出口参数:          无
********************************************************************************************************/ 
#if ISD4004_RECORDTaskTCB_CONFIG
void ISD4004_Recording(u16 st_addr)
{
	u16 end_addr =0;
	SW_RECORD=1;//置IO口为1，准备读入数据
   delay_us(1);
   while(SW_RECORD ==1 );
   if(SW_RECORD==0)
   {
		ISD4004_powerup(); //AN键按下，ISD上电并延迟50ms
		ISD4004_stopwrdn();
		ISD4004_powerup(); 
        
    	ISD4004_setrec(st_add&0x00ff,st_add>>8); //从指定的地址
        if(INT==1)// 判定芯片有没有溢出
        {		
            ISD4004_rec(); //发送录音指令
        }
		st_addr+=ISD4004_ADDR_INTERVAL;
		time_total=st_addr*2;//计时初始值计算
        TIM_Cmd(TIM4, ENABLE);  //使能TIMx	//开计时器
        while(SW_RECORD==0);//等待本次录音结束
        TIM_Cmd(TIM4, DISABLE);  //使能TIMx	//录音结束后停止计时
		ISD4004_stop(); //发送4004停止命令
        end_addr =time_total/2+2;//计算语音的结束地址
		prt("end_addr:%u \r\n",end_addr);  // 打印结束地址
   }
}
#endif
/******************************************************************************************************** 
**     函数名称:         ISD4004_Playing1
**    功能描述:          指定地址放音1
**    入口参数：       	 u16 st_addr,u16 edd_addr，放音起始地址和结束地址
**    出口参数:          无
********************************************************************************************************/ 
void ISD4004_Playing1(u16 st_addr,u16 edd_addr)
{
		request = 0;
		ISD4004_powerup(); //AN键按下，ISD上电并延迟50ms
		ISD4004_stopwrdn();
		ISD4004_powerup(); 
 		ISD4004_setplay(st_addr&0x00ff,st_addr>>8); //发送setplay指令，从指定地址开始放音
		time_total = st_addr*2;
		TIM_Cmd(TIM4, ENABLE);  //使能TIMx	//开计时器
		ISD4004_play(); //发送放音指令
		delay_us(20);
		prt("ISD4004_play \r\n");
		while((time_total < edd_addr*2)&&(request == 0)); //等待放音完毕的EOM中断信号
		TIM_Cmd(TIM4, DISABLE);  //使能TIMx	//开计时器
		prt("放音完毕 \r\n");
		ISD4004_stop(); //放音完毕，发送stop指令 
}

/******************************************************************************************************** 
**     函数名称:         ISD4004_Playing
**    功能描述:          指定地址放音
**    入口参数：       	 u16 addr 放音起始地址
**    出口参数:          无
********************************************************************************************************/ 
void ISD4004_Playing(u16 addr)
{
		
		request = 0;
		ISD4004_powerup(); //AN键按下，ISD上电并延迟50ms
		ISD4004_stopwrdn();
		ISD4004_powerup(); 
 		ISD4004_setplay(addr&0x00ff,addr>>8); //发送setplay指令，从指定地址开始放音
		ISD4004_play(); //发送放音指令
		delay_us(20);
		prt("ISD4004_play \r\n");
		time_total = 0;
		TIM_Cmd(TIM4, ENABLE);  //使能TIMx	//开计时器
		while((INT==1) &&(request == 0))
		{
			if(time_total > 200)break;		
		} //等待放音完毕的EOM中断信号
		prt("放音完毕 \r\n");
		ISD4004_stop(); //放音完毕，发送stop指令 
		TIM_Cmd(TIM4, DISABLE);  //使能TIMx	//开计时器
}
/******************************************************************************************************** 
**     函数名称:         ISD4004_Display
**    功能描述:          显示处理程序初始化
**    入口参数：       	 无
**    出口参数:          无
********************************************************************************************************/ 
#if ISD4004_RECORDTaskTCB_CONFIG
void ISD4004_Display(void)
{
        uchar x,i;
//		const u16 addr[37] = {1501,196,243,290,340,387,435,490,550,1533,1572,1616,583,711,740,764,823,845,898,922,974,998,
//					1050,1074,1125,1149,1200,1232,1286,1313,1365,1445,
//					1660,1719,1762,1809,1839};
        if(flag3==1||flag4==1)//判断是否有录音过或者放音过
        {
          x=count-1;
          if(x==255){x=count_flag-1;}
        }
		//prt("显示当前段语音是第几段:%u\r\n",x);
        
//        if(flag3==0)//录音时显示本段语音的起始和结束地址
//        {
//			for(i = 0;i<VOICE_PASSAGE_NUM;i++)
//				prt("录音本段语音的起始和结束地址:%d--%d\r\n",adds[i],adde[i]);
//           
//        }
//        if(flag4==1)//放音时显示本段语音的起始和结束地址
//        {
//			prt("播放本段语音的起始和结束地址:%d--%d\r\n",adds[x],adde[x]);
//           
//        }
			if(KEY0 == 0)
			{
				for(i = 0;i<3;i++)
					prt("#define  ISD4004_VADDR_%u    %d  \r\n",i+1,adds[i]);
			}
		delay_ms(20);
}
#endif
/******************************************************************************************************** 
**     函数名称:         ISD4004_send
**    功能描述:         spi串行发送子程序，内部函数
**    入口参数：       	 uchar isdx  要发送的八位数据
**    出口参数:          无
********************************************************************************************************/ 
void ISD4004_send(uchar isdx)
{
	u8 i;
	SS=0;//ss=0,打开spi通信端
	SCLK=0;
	for(i=0;i<8;i++)//先发低位再发高位，依次发送。
	{
		if((isdx&0x01)==1)
			MOSI=1;
		else
			MOSI=0;
		isdx=isdx>>1;
		SCLK=1;
		delay_us(2);
		SCLK=0;
		delay_us(2);
	}
}
/******************************************************************************************************** 
**     函数名称:         ISD4004_stop
**    功能描述:          stop指令（停止当前操作）内部函数
**    入口参数：       	 无
**    出口参数:          无
********************************************************************************************************/ 
void ISD4004_stop(void)//
{
	delay_us(10);
	ISD4004_send(ISD4004_STOP_CMD);
	SS=1;
	delay_ms(50);
}
/******************************************************************************************************** 
**     函数名称:         ISD4004_powerup
**    功能描述:          发送上电指令
**    入口参数：       	 无
**    出口参数:          无
********************************************************************************************************/ 
void ISD4004_powerup(void)//
{
	delay_us(10);
	SS=0;
	ISD4004_send(ISD4004_POWERUP_CMD);
	SS=1;
	delay_ms(50);
}
/******************************************************************************************************** 
**     函数名称:         ISD4004_stopwrdn
**    功能描述:          发送掉电指令
**    入口参数：       	 无
**    出口参数:          无
********************************************************************************************************/ 
void ISD4004_stopwrdn(void)//
{
	delay_us(10);
	ISD4004_send(ISD4004_STOPWRDN_CMD);
	SS=1;
	delay_ms(50);
}

/******************************************************************************************************** 
**     函数名称:         ISD4004_rec
**    功能描述:          发送录音指令
**    入口参数：       	 无
**    出口参数:          无
********************************************************************************************************/ 
void ISD4004_rec(void)
{
	ISD4004_send(ISD4004_REC_CMD);
	SS=1;
}
/******************************************************************************************************** 
**     函数名称:         ISD4004_setrec
**    功能描述:         发送setrec指令
**    入口参数：       	 unsigned char adl,unsigned char adh，发送放音起始地址低位、起始地址高位
**    出口参数:          无
********************************************************************************************************/ 
void ISD4004_setrec(unsigned char adl,unsigned char adh)
{
	delay_ms(1);
	ISD4004_send(adl); //发送放音起始地址低位
	delay_us(2);
	ISD4004_send(adh); //发送放音起始地址高位
	delay_us(2);
	ISD4004_send(ISD4004_SETREC_CMD); //发ISD4004_setrec令字节
	SS=1;
}
/******************************************************************************************************** 
**     函数名称:         ISD4004_play
**    功能描述:          发送play指令
**    入口参数：       	 无
**    出口参数:          无
********************************************************************************************************/ 
void ISD4004_play(void)
{
	ISD4004_send(ISD4004_PLAY_CMD);
	SS=1;
}
/******************************************************************************************************** 
**     函数名称:         ISD4004_setplay
**    功能描述:          发送setplay指令
**    入口参数：       	 uchar adl,uchar adh 发送录音起始地址低位、起始地址高位
**    出口参数:          无
********************************************************************************************************/ 
void ISD4004_setplay(uchar adl,uchar adh)
{
	delay_ms(1);
	ISD4004_send(adl); //发送放音起始地址低位
	delay_us(2);
	ISD4004_send(adh); //发送放音起始地址高位
	delay_us(2);
	ISD4004_send(ISD4004_SETPLAY_CMD); //发送setplay指令字节
	SS=1;
}

/******************************************************************************************************** 
**     函数名称:         ISD4004_StopPlaying
**    功能描述:          停止当前放音
**    入口参数：       	 无
**    出口参数:          无
********************************************************************************************************/ 
void ISD4004_StopPlaying(void)
{
	request = 1;
	prt("ISD4004_StopPlaying \r\n");
}

