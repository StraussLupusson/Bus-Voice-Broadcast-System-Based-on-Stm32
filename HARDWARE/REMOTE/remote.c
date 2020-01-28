#include "remote.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#if SYSTEM_SUPPORT_OS  //ʹ��UCOS����ϵͳ
	#include "includes.h"
#endif

//����ң�س�ʼ��
//����IO�Լ�TIM2_CH1�����벶��
void Remote_Init(void)    			  
{		
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;  
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //ʹ��PORTBʱ�� 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);	//TIM2 ʱ��ʹ�� 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 //PB9 ���� 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		//�������� 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOA,GPIO_Pin_1);	//��ʼ��GPIOA.9
						  
 	TIM_TimeBaseStructure.TIM_Period = 10000; //�趨�������Զ���װֵ ���10ms���  
	TIM_TimeBaseStructure.TIM_Prescaler =(72-1); 	//Ԥ��Ƶ��,1M�ļ���Ƶ��,1us��1.	   	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;  // ѡ������� IC4ӳ�䵽TI4��
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
	TIM_ICInitStructure.TIM_ICFilter = 0x03;//IC4F=0011 ���������˲��� 8����ʱ��ʱ�������˲�
	TIM_ICInit(TIM2, &TIM_ICInitStructure);//��ʼ����ʱ�����벶��ͨ��
 
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���	
	
	TIM_ClearFlag(TIM2,TIM_FLAG_Update|TIM_IT_CC2);
	TIM_ITConfig( TIM2,TIM_IT_Update|TIM_IT_CC2,ENABLE);//��������ж� ,����CC4IE�����ж�		
	TIM_Cmd(TIM2,ENABLE ); 	//ʹ�ܶ�ʱ��2
}

//ң��������״̬
//[7]:�յ����������־
//[6]:�õ���һ��������������Ϣ
//[5]:����	
//[4]:����������Ƿ��Ѿ�������								   
//[3:0]:�����ʱ��
u8 	REMORE_STA=0;	  	  
u16 Dval;		//�½���ʱ��������ֵ
u32 RmtRec=0;	//������յ�������	   		    
u8  RmtCnt=0;	//�������µĴ���	
void TIM2_IRQHandler(void) 
{
#if SYSTEM_SUPPORT_OS  //ʹ��UCOS����ϵͳ
	OSIntEnter();    
#endif
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
	{
		if(REMORE_STA&0x80)								//�ϴ������ݱ����յ���
		{	
			REMORE_STA&=~0X10;							//ȡ���������Ѿ���������
			if((REMORE_STA&0X0F)==0X00)REMORE_STA|=1<<6;	//����Ѿ����һ�ΰ����ļ�ֵ��Ϣ�ɼ�
			if((REMORE_STA&0X0F)<14)REMORE_STA++;
			else
			{
				REMORE_STA&=~(1<<7);					//���������ʶ
				REMORE_STA&=0XF0;						//��ռ�����	
			}								 	   	
		}							    
	}

	if(TIM_GetITStatus(TIM2,TIM_IT_CC2)!=RESET)
	{	  
		if(RDATA)//�����ز���
		{
  			TIM_OC2PolarityConfig(TIM2,TIM_ICPolarity_Falling);						//CC4P=1	����Ϊ�½��ز���
			TIM_SetCounter(TIM2,0);							//��ն�ʱ��ֵ
			REMORE_STA|=0X10;							//����������Ѿ�������
		}else //�½��ز���
		{
			Dval=TIM_GetCapture2(TIM2);					//��ȡCCR4Ҳ������CC4IF��־λ
  		TIM_OC2PolarityConfig(TIM2,TIM_ICPolarity_Rising);				//CC4P=0	����Ϊ�����ز���
			if(REMORE_STA&0X10)							//���һ�θߵ�ƽ���� 
			{
 				if(REMORE_STA&0X80)//���յ���������
				{
					//prt("���յ���������\r\n");
					if(Dval>300&&Dval<800)			//560Ϊ��׼ֵ,560us
					{
						//prt("���յ�0\r\n");
						RmtRec<<=1;					//����һλ.
						RmtRec|=0;					//���յ�0	   
					}else if(Dval>1400&&Dval<1800)	//1680Ϊ��׼ֵ,1680us
					{
						//prt("���յ�1\r\n");
						RmtRec<<=1;					//����һλ.
						RmtRec|=1;					//���յ�1
					}else if(Dval>2200&&Dval<2600)	//�õ�������ֵ���ӵ���Ϣ 2500Ϊ��׼ֵ2.5ms
					{
						//prt("������������1��\r\n");
						RmtCnt++; 					//������������1��
						REMORE_STA&=0XF0;				//��ռ�ʱ��		
					}
 				}else if(Dval>4200&&Dval<4700)		//4500Ϊ��׼ֵ4.5ms
				{
					//prt("��ǳɹ����յ���������\r\n");
					REMORE_STA|=1<<7;					//��ǳɹ����յ���������
					RmtCnt=0;						//�����������������
				}						 
			}
			REMORE_STA&=~(1<<4);
		}				 		     	    					   
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_CC2|TIM_IT_Update);	 
#if SYSTEM_SUPPORT_OS  
	OSIntExit();    	//�˳��ж�
#endif	
}

//����������
//����ֵ:
//0,û���κΰ�������
//����,���µİ�����ֵ.
u8 Get_RemoteKeyValue(void)
{        
	u8 sta=0;       
    u8 t1,t2;  
	if(REMORE_STA&(1<<6))//�õ�һ��������������Ϣ��
	{ 
		//prt("REMORE_STA&(1<<6)\r\n");
	    t1=RmtRec>>24;			//�õ���ַ��
	    t2=(RmtRec>>16)&0xff;	//�õ���ַ���� 
 	   // if((t1==(u8)~t2)&&t1==REMOTE_ID)//����ң��ʶ����(ID)����ַ 
	    { 
			//prt("t1==REMOTE_ID\r\n");
	        t1=RmtRec>>8;
	        t2=RmtRec; 	
	        if(t1==(u8)~t2)sta=t1;//��ֵ��ȷ	 
		}  
		if((sta==0)||((REMORE_STA&0X80)==0))//�������ݴ���/ң���Ѿ�û�а�����
		{
			//prt("sta==0)||((REMORE_STA&0X80)==0\r\n");
		 	REMORE_STA&=~(1<<6);//������յ���Ч������ʶ
			RmtCnt=0;		//�����������������
		}
	}  
    return sta;
}
/******************************************************************************************************** 
**     ��������:         Remote_Scan
**    ��������:          ����ɨ��
**    ��ڲ�����       Pres_Mode mode  ��֧��������SINGLE_PRES  ֧��������DOUBLE_PRES 
**    ���ڲ���:          ��ֵ
**    ����˵����       ��
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
����
*/
#if 0
//С��ң����
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
//����Ʒ��ң��
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




