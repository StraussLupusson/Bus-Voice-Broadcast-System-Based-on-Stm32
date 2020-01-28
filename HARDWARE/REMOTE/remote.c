#include "remote.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
	#include "includes.h"
#endif

//红外遥控初始化
//设置IO以及TIM2_CH1的输入捕获
void Remote_Init(void)    			  
{		
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;  
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //使能PORTB时钟 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);	//TIM2 时钟使能 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 //PB9 输入 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		//上拉输入 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOA,GPIO_Pin_1);	//初始化GPIOA.9
						  
 	TIM_TimeBaseStructure.TIM_Period = 10000; //设定计数器自动重装值 最大10ms溢出  
	TIM_TimeBaseStructure.TIM_Prescaler =(72-1); 	//预分频器,1M的计数频率,1us加1.	   	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;  // 选择输入端 IC4映射到TI4上
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
	TIM_ICInitStructure.TIM_ICFilter = 0x03;//IC4F=0011 配置输入滤波器 8个定时器时钟周期滤波
	TIM_ICInit(TIM2, &TIM_ICInitStructure);//初始化定时器输入捕获通道
 
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器	
	
	TIM_ClearFlag(TIM2,TIM_FLAG_Update|TIM_IT_CC2);
	TIM_ITConfig( TIM2,TIM_IT_Update|TIM_IT_CC2,ENABLE);//允许更新中断 ,允许CC4IE捕获中断		
	TIM_Cmd(TIM2,ENABLE ); 	//使能定时器2
}

//遥控器接收状态
//[7]:收到了引导码标志
//[6]:得到了一个按键的所有信息
//[5]:保留	
//[4]:标记上升沿是否已经被捕获								   
//[3:0]:溢出计时器
u8 	REMORE_STA=0;	  	  
u16 Dval;		//下降沿时计数器的值
u32 RmtRec=0;	//红外接收到的数据	   		    
u8  RmtCnt=0;	//按键按下的次数	
void TIM2_IRQHandler(void) 
{
#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
	OSIntEnter();    
#endif
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
	{
		if(REMORE_STA&0x80)								//上次有数据被接收到了
		{	
			REMORE_STA&=~0X10;							//取消上升沿已经被捕获标记
			if((REMORE_STA&0X0F)==0X00)REMORE_STA|=1<<6;	//标记已经完成一次按键的键值信息采集
			if((REMORE_STA&0X0F)<14)REMORE_STA++;
			else
			{
				REMORE_STA&=~(1<<7);					//清空引导标识
				REMORE_STA&=0XF0;						//清空计数器	
			}								 	   	
		}							    
	}

	if(TIM_GetITStatus(TIM2,TIM_IT_CC2)!=RESET)
	{	  
		if(RDATA)//上升沿捕获
		{
  			TIM_OC2PolarityConfig(TIM2,TIM_ICPolarity_Falling);						//CC4P=1	设置为下降沿捕获
			TIM_SetCounter(TIM2,0);							//清空定时器值
			REMORE_STA|=0X10;							//标记上升沿已经被捕获
		}else //下降沿捕获
		{
			Dval=TIM_GetCapture2(TIM2);					//读取CCR4也可以清CC4IF标志位
  		TIM_OC2PolarityConfig(TIM2,TIM_ICPolarity_Rising);				//CC4P=0	设置为上升沿捕获
			if(REMORE_STA&0X10)							//完成一次高电平捕获 
			{
 				if(REMORE_STA&0X80)//接收到了引导码
				{
					//prt("接收到了引导码\r\n");
					if(Dval>300&&Dval<800)			//560为标准值,560us
					{
						//prt("接收到0\r\n");
						RmtRec<<=1;					//左移一位.
						RmtRec|=0;					//接收到0	   
					}else if(Dval>1400&&Dval<1800)	//1680为标准值,1680us
					{
						//prt("接收到1\r\n");
						RmtRec<<=1;					//左移一位.
						RmtRec|=1;					//接收到1
					}else if(Dval>2200&&Dval<2600)	//得到按键键值增加的信息 2500为标准值2.5ms
					{
						//prt("按键次数增加1次\r\n");
						RmtCnt++; 					//按键次数增加1次
						REMORE_STA&=0XF0;				//清空计时器		
					}
 				}else if(Dval>4200&&Dval<4700)		//4500为标准值4.5ms
				{
					//prt("标记成功接收到了引导码\r\n");
					REMORE_STA|=1<<7;					//标记成功接收到了引导码
					RmtCnt=0;						//清除按键次数计数器
				}						 
			}
			REMORE_STA&=~(1<<4);
		}				 		     	    					   
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_CC2|TIM_IT_Update);	 
#if SYSTEM_SUPPORT_OS  
	OSIntExit();    	//退出中断
#endif	
}

//处理红外键盘
//返回值:
//0,没有任何按键按下
//其他,按下的按键键值.
u8 Get_RemoteKeyValue(void)
{        
	u8 sta=0;       
    u8 t1,t2;  
	if(REMORE_STA&(1<<6))//得到一个按键的所有信息了
	{ 
		//prt("REMORE_STA&(1<<6)\r\n");
	    t1=RmtRec>>24;			//得到地址码
	    t2=(RmtRec>>16)&0xff;	//得到地址反码 
 	   // if((t1==(u8)~t2)&&t1==REMOTE_ID)//检验遥控识别码(ID)及地址 
	    { 
			//prt("t1==REMOTE_ID\r\n");
	        t1=RmtRec>>8;
	        t2=RmtRec; 	
	        if(t1==(u8)~t2)sta=t1;//键值正确	 
		}  
		if((sta==0)||((REMORE_STA&0X80)==0))//按键数据错误/遥控已经没有按下了
		{
			//prt("sta==0)||((REMORE_STA&0X80)==0\r\n");
		 	REMORE_STA&=~(1<<6);//清除接收到有效按键标识
			RmtCnt=0;		//清除按键次数计数器
		}
	}  
    return sta;
}
/******************************************************************************************************** 
**     函数名称:         Remote_Scan
**    功能描述:          红外扫描
**    入口参数：       Pres_Mode mode  不支持连续按SINGLE_PRES  支持连续按DOUBLE_PRES 
**    出口参数:          键值
**    其他说明：       无
********************************************************************************************************/ 
u8 Remote_Scan(Pres_Mode mode)
{
	u8 key;
	static u8 k = 1;
	u8 str = 0;
	if(mode)k = 1;
	key=Get_RemoteKeyValue();	
	if(key&&k)
	{	  
		switch(key)
		{
			case 0:str = ERROR;break;		   
			case 162:str =  CH_DEC;	break;		    
			case 98:str =  CH;	break;	  
			case 226:str =  CH_INC;	break;			 
			case 34:str =  LEFT;	break;			  
			case 2:str =  RIGHT;break;			   
			case 194:str =  STOP;	break;		  
			case 224:str =  VOL_DEC;break;			  
			case 168:str =  VOL_INC;break;				   
			case 144:str =  EQ;	break;			    
			case 104:str =  KEY_0;break;			  
			case 152:str =  KEY_100;break;			   
			case 176:str =  KEY_200;break;		    
			case 48:str =  KEY_1;break;			    
			case 24:str =  KEY_2;break;				    
			case 122:str =  KEY_3;	break;			  
			case 16:str =  KEY_4;break;				   					
			case 56:str =  KEY_5;break;			 
			case 90:str =  KEY_6;break;		
			case 66:str =  KEY_7;break;		
			case 74:str =  KEY_8;break;		
			case 82:str =  KEY_9;break;			 
		}k = 0;
   }
   if(REMORE_STA == 0)k = 1;
   if (str!= 0)
		return str;
   return 0;
}    

/*
测试
*/
#if 0
//小型遥控器
u8* Get_RemoteKey(Pres_Mode mode)
{
	u8 key;
	static u8 k = 1;
	char *str = 0;
	if(mode)k = 1;
	key=Get_RemoteKeyValue();	
	if(key&&k)
	{	  
		switch(key)
		{
			case 0:str="ERROR";break;			   
			case 162:str="CH-";break;	    
			case 98:str="CH";break;	    
			case 226:str="CH+";break;		 
			case 34:str="LEFT";break;		  
			case 2:str="RIGHT";break;	   
			case 194:str="STOP";break;		  
			case 224:str="VOL-";break;		  
			case 168:str="VOL+";break;		   
			case 144:str="EQ";break;		    
			case 104:str="0";break;		  
			case 152:str="100+";break;	   
			case 176:str="200+";break;	    
			case 48:str="1";break;		    
			case 24:str="2";break;		    
			case 122:str="3";break;		  
			case 16:str="4";break;			   					
			case 56:str="5";break;	 
			case 90:str="6";break;
			case 66:str="7";break;
			case 74:str="8";break;
			case 82:str="9";break;		 
		}k = 0;
		
		
   }
   if(REMORE_STA == 0)k = 1;
   if (str!= 0)
	prt("%s \r\n",str);
   return 0;
}
//#if 0
//三星品牌遥控
u8* Get_RemoteKey(Pres_Mode mode)
{
	u8 key;
	static u8 k = 1;
	static u8 sw = 1;
	char *str = 0;
	if(mode)k = 1;
	key=Get_RemoteKeyValue();	
	if(key&&k)
	{	  
		switch(key)
		{
			case 0:str="ERROR";break;			   
			case 32:str="1";break;	    
			case 160:str="2";break;	    
			case 96:str="3";break;		 
			case 16:str="4";break;		  
			case 144:str="5";break;	   
			case 80:str="6";break;		  
			case 48:str="7";break;		  
			case 176:str="8";break;		   
			case 112:str="9";break;	
			case 196:str="love";break;
			case 200:str="return";break;				
			case 136:str="0";break;		  
				    
			case 6:str="up";break;		    
			case 134:str="down";break;		    
			case 166:str="left";break;		  
			case 70:str="right";break;			   					
			case 22:str="ok";break;	 
			case 88:str="list";break;
			case 158:str="home";break;
			case 26:str="return";break;
			case 180:str="tool";break;
			case 224:str="Vol+";break;
			case 208:str="Vol-";break;	
			case 72:str="CH+";break;	
			case 8:str="CH-";break;	
			case 64: sw++;if(sw%2)str="Close";else str="Open"; break;
			case 240:str="Mute";break;				
		
		}k = 0;
		
		if(REMORE_STA)
		{
			printf("%s\r\n",str);
			//LED1 = !LED1;
			return (u8*)str;
			delay_ms(100);
		}
   }
   if(REMORE_STA == 0)k = 1;
   return 0; 
}
#endif




