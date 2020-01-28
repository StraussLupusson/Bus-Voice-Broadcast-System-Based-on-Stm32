/*
	�ļ����� ISD4004.c
	�ļ�������STM32F03C8T6---ISD4004 Device Driver
	�޸�ʱ�䣺2019��4��21��
	���ߣ�������
	����˵������
*/

#include "ISD4004.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "task_config.h"
//�����е�һЩ��������
vu32    time_total;
vu32    st_add,end_add=0;
vu32    adds[VOICE_PASSAGE_NUM];//25����������ʼ��ַ�ݴ�
vu32    adde[VOICE_PASSAGE_NUM];//25�������Ľ�����ַ��ʱ
vu8   count,count_flag,flag2,flag3,flag4;
vu8 request;

/******************************************************************************************************** 
**     ��������:         ISD4004_Init
**    ��������:          ��ʼ��ISD4004���GPIO����
**    ��ڲ�����       	 ��
**    ���ڲ���:          ��
********************************************************************************************************/ 
void ISD4004_Init(void)
{
	//�˿����ó�ʼ��
 	GPIO_InitTypeDef  GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(ISD4004_GPIO_CLK, ENABLE);	 //ʹ��A�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin =  ISD4004_SS_PIN | ISD4004_MOSI_PIN |ISD4004_SCLK_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ISD4004_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  ISD4004_INT_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	
	GPIO_Init(ISD4004_GPIO_PORT, &GPIO_InitStructure);
	
	//������Ҫ PB0 �ӵ�
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//¼����������ʼ��
//#if ISD4004_RECORDTaskTCB_CONFIG
	KEY_Init();
	flag3=0;
	flag4=0;
	time_total=ISD4004_START_ADDR*2;//¼����ַ��170��ʼ����Ӧ�ĵ�Ƭ����ʼ��ʱ��ʱ�����340*0.1��
	adds[0]=ISD4004_START_ADDR;
	count=0;
	ISD4004_TIM4Init(TIME4_FREQ);
	//#endif
}
/******************************************************************************************************** 
**     ��������:         ISD4004_TIM4Init
**    ��������:          ��ʱ��TIM4��ʼ��
**                     ����1��¼����ʱ��ȷ��¼����ַ
						����2: ����ʱ���ʱ
**    ��ڲ�����       	 u16 msec����ʱ����msec΢��
**    ���ڲ���:          ��
**    ����˵����		fl03��ͨ��ʱ����ʼ����2 - 7Timer
						Tout= ((TIM_Period+1)*(TIM_Prescaler+1))/Tclk
********************************************************************************************************/ 

void ISD4004_TIM4Init(u16 msec)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM4��ʼ��
	TIM_TimeBaseStructure.TIM_Period  = msec*10-1;; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =7200-1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM4�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM4, DISABLE);  
					 
}

//#if ISD4004_RECORDTaskTCB_CONFIG
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
/******************************************************************************************************** 
**     ��������:         TIM4_IRQHandler
**    ��������:           ��ʱ��TIM4�жϷ�����
**    ��ڲ�����       	 ��
**    ���ڲ���:          ��
********************************************************************************************************/ 
void TIM4_IRQHandler(void)   //TIM4�ж�
{
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //���TIM4�����жϷ������
		{
			time_total++;
			if(time_total==4800)time_total=0; 	
			//prt("time 1s");
			TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		
		}
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
OSIntExit();  											 
#endif
}
//#endif

/******************************************************************************************************** 
**     ��������:         ISD4004_RecordingTreat
**    ��������:         ¼�����������
**    ��ڲ�����       	 ��
**    ���ڲ���:          ��
********************************************************************************************************/ 
#if ISD4004_RECORDTaskTCB_CONFIG
void ISD4004_RecordingTreat(void)
{
	//prt("ISD4004_RecordingTreat start\r\n");
   SW_RECORD=1;//��IO��Ϊ1��׼����������
   delay_us(1);
   if(SW_RECORD==0)
   {
      if(flag3==0)//¼�����ͷ�����������¼���ú󣬽�ֹ�ٴ�¼�������Ҫ�ٴ�¼�����Ǿ�Ҫ��λ��Ƭ�������¿�ʼ¼��
      {
        if(count==0)//�ж��Ƿ�Ϊ�ϵ��λ������һ�ΰ�¼����
        {
           st_add= ISD4004_START_ADDR;
        }
        else
        {
          st_add=end_add + ISD4004_ADDR_INTERVAL; 
        }//ÿ�����Լ��3����ַ
        adds[count]=st_add;//ÿ����������ʼ��ַ��ʱ
        if(count>=VOICE_PASSAGE_NUM)//�ж���������ʱ�򳬹�25��
        {
           count=0;
           st_add = ISD4004_START_ADDR;
           time_total=ISD4004_START_ADDR*2;
        }
        ISD4004_powerup(); //AN�����£�ISD�ϵ粢�ӳ�50ms
		ISD4004_stopwrdn();
		ISD4004_powerup(); 
        
    	ISD4004_setrec(st_add&0x00ff,st_add>>8); //��ָ���ĵ�ַ
        if(INT==1)// �ж�оƬ��û�����
        {		
            ISD4004_rec(); //����¼��ָ��
        }
        time_total=st_add*2;//��ʱ��ʼֵ����
        TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx	//����ʱ��
        while(SW_RECORD==0);//�ȴ�����¼������
        TIM_Cmd(TIM4, DISABLE);  //ʹ��TIMx	//¼��������ֹͣ��ʱ
        ISD4004_stop(); //����4004ֹͣ����
        end_add=time_total/2+2;//���������Ľ�����ַ
        adde[count]=end_add;//��������������ַ�ݴ�
        count++;//¼�������Լ�
        count_flag=count;//¼�������Ĵ�
        flag2=1;
        flag4=1;//����������
      }
  }
}
#endif
/******************************************************************************************************** 
**     ��������:         ISD4004_PlayingTreat
**    ��������:         �������������
**    ��ڲ�����       	 ��
**    ���ڲ���:          ��
********************************************************************************************************/ 
#if ISD4004_RECORDTaskTCB_CONFIG
void ISD4004_PlayingTreat(void)
{
	//prt("ISD4004_PlayingTreat start\r\n");
  // uchar ovflog;
   AN=1;//׼����������
   delay_us(1);
   if(AN==0)//�жϷ������Ƿ���
   {
		prt("AN==0\r\n");
     if(flag4==1)//����¼����
     {
		prt("flag4==1\r\n");
        if(flag2==1)//�ж��Ƿ�Ϊ¼���ú�ĵ�һ�η���
        {
		prt("flag2==1\r\n");
           count=0;//�ӵ�0�ο�ʼ����
        }
		
        ISD4004_powerup(); //AN�����£�ISD�ϵ粢�ӳ�50ms
		ISD4004_stopwrdn();
		ISD4004_powerup(); 
        st_add=adds[count];//�͵�ǰ��������ʼ��ַ
 		ISD4004_setplay(st_add&0x00ff,st_add>>8); //����setplayָ���ָ����ַ��ʼ����
		ISD4004_play(); //���ͷ���ָ��
		delay_us(20);
		prt("ISD4004_play \r\n");
		while(INT==1); //�ȴ�������ϵ�EOM�ж��ź�
		prt("������� \r\n");
		ISD4004_stop(); //������ϣ�����stopָ��
		prt("����stopָ�� \r\n");
		while(AN==0); // 
		prt("while(AN==0) \r\n");
		ISD4004_stop();
        count++;//���������Լ�
        flag2=0;
        flag3=1;
        if(count>=count_flag)//������ŵ����һ�κ󻹰��Ӽ�����ӵ�һ�����²���
        {
             count=0;
        }
     
      }
   } 
}
#endif
/******************************************************************************************************** 
**     ��������:         ISD4004_Recording
**    ��������:          ָ����ַ¼��
**    ��ڲ�����       	 u16 st_addr��ָ��¼����ʼ��ַ
**    ���ڲ���:          ��
********************************************************************************************************/ 
#if ISD4004_RECORDTaskTCB_CONFIG
void ISD4004_Recording(u16 st_addr)
{
	u16 end_addr =0;
	SW_RECORD=1;//��IO��Ϊ1��׼����������
   delay_us(1);
   while(SW_RECORD ==1 );
   if(SW_RECORD==0)
   {
		ISD4004_powerup(); //AN�����£�ISD�ϵ粢�ӳ�50ms
		ISD4004_stopwrdn();
		ISD4004_powerup(); 
        
    	ISD4004_setrec(st_add&0x00ff,st_add>>8); //��ָ���ĵ�ַ
        if(INT==1)// �ж�оƬ��û�����
        {		
            ISD4004_rec(); //����¼��ָ��
        }
		st_addr+=ISD4004_ADDR_INTERVAL;
		time_total=st_addr*2;//��ʱ��ʼֵ����
        TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx	//����ʱ��
        while(SW_RECORD==0);//�ȴ�����¼������
        TIM_Cmd(TIM4, DISABLE);  //ʹ��TIMx	//¼��������ֹͣ��ʱ
		ISD4004_stop(); //����4004ֹͣ����
        end_addr =time_total/2+2;//���������Ľ�����ַ
		prt("end_addr:%u \r\n",end_addr);  // ��ӡ������ַ
   }
}
#endif
/******************************************************************************************************** 
**     ��������:         ISD4004_Playing1
**    ��������:          ָ����ַ����1
**    ��ڲ�����       	 u16 st_addr,u16 edd_addr��������ʼ��ַ�ͽ�����ַ
**    ���ڲ���:          ��
********************************************************************************************************/ 
void ISD4004_Playing1(u16 st_addr,u16 edd_addr)
{
		request = 0;
		ISD4004_powerup(); //AN�����£�ISD�ϵ粢�ӳ�50ms
		ISD4004_stopwrdn();
		ISD4004_powerup(); 
 		ISD4004_setplay(st_addr&0x00ff,st_addr>>8); //����setplayָ���ָ����ַ��ʼ����
		time_total = st_addr*2;
		TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx	//����ʱ��
		ISD4004_play(); //���ͷ���ָ��
		delay_us(20);
		prt("ISD4004_play \r\n");
		while((time_total < edd_addr*2)&&(request == 0)); //�ȴ�������ϵ�EOM�ж��ź�
		TIM_Cmd(TIM4, DISABLE);  //ʹ��TIMx	//����ʱ��
		prt("������� \r\n");
		ISD4004_stop(); //������ϣ�����stopָ�� 
}

/******************************************************************************************************** 
**     ��������:         ISD4004_Playing
**    ��������:          ָ����ַ����
**    ��ڲ�����       	 u16 addr ������ʼ��ַ
**    ���ڲ���:          ��
********************************************************************************************************/ 
void ISD4004_Playing(u16 addr)
{
		
		request = 0;
		ISD4004_powerup(); //AN�����£�ISD�ϵ粢�ӳ�50ms
		ISD4004_stopwrdn();
		ISD4004_powerup(); 
 		ISD4004_setplay(addr&0x00ff,addr>>8); //����setplayָ���ָ����ַ��ʼ����
		ISD4004_play(); //���ͷ���ָ��
		delay_us(20);
		prt("ISD4004_play \r\n");
		time_total = 0;
		TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx	//����ʱ��
		while((INT==1) &&(request == 0))
		{
			if(time_total > 200)break;		
		} //�ȴ�������ϵ�EOM�ж��ź�
		prt("������� \r\n");
		ISD4004_stop(); //������ϣ�����stopָ�� 
		TIM_Cmd(TIM4, DISABLE);  //ʹ��TIMx	//����ʱ��
}
/******************************************************************************************************** 
**     ��������:         ISD4004_Display
**    ��������:          ��ʾ��������ʼ��
**    ��ڲ�����       	 ��
**    ���ڲ���:          ��
********************************************************************************************************/ 
#if ISD4004_RECORDTaskTCB_CONFIG
void ISD4004_Display(void)
{
        uchar x,i;
//		const u16 addr[37] = {1501,196,243,290,340,387,435,490,550,1533,1572,1616,583,711,740,764,823,845,898,922,974,998,
//					1050,1074,1125,1149,1200,1232,1286,1313,1365,1445,
//					1660,1719,1762,1809,1839};
        if(flag3==1||flag4==1)//�ж��Ƿ���¼�������߷�����
        {
          x=count-1;
          if(x==255){x=count_flag-1;}
        }
		//prt("��ʾ��ǰ�������ǵڼ���:%u\r\n",x);
        
//        if(flag3==0)//¼��ʱ��ʾ������������ʼ�ͽ�����ַ
//        {
//			for(i = 0;i<VOICE_PASSAGE_NUM;i++)
//				prt("¼��������������ʼ�ͽ�����ַ:%d--%d\r\n",adds[i],adde[i]);
//           
//        }
//        if(flag4==1)//����ʱ��ʾ������������ʼ�ͽ�����ַ
//        {
//			prt("���ű�����������ʼ�ͽ�����ַ:%d--%d\r\n",adds[x],adde[x]);
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
**     ��������:         ISD4004_send
**    ��������:         spi���з����ӳ����ڲ�����
**    ��ڲ�����       	 uchar isdx  Ҫ���͵İ�λ����
**    ���ڲ���:          ��
********************************************************************************************************/ 
void ISD4004_send(uchar isdx)
{
	u8 i;
	SS=0;//ss=0,��spiͨ�Ŷ�
	SCLK=0;
	for(i=0;i<8;i++)//�ȷ���λ�ٷ���λ�����η��͡�
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
**     ��������:         ISD4004_stop
**    ��������:          stopָ�ֹͣ��ǰ�������ڲ�����
**    ��ڲ�����       	 ��
**    ���ڲ���:          ��
********************************************************************************************************/ 
void ISD4004_stop(void)//
{
	delay_us(10);
	ISD4004_send(ISD4004_STOP_CMD);
	SS=1;
	delay_ms(50);
}
/******************************************************************************************************** 
**     ��������:         ISD4004_powerup
**    ��������:          �����ϵ�ָ��
**    ��ڲ�����       	 ��
**    ���ڲ���:          ��
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
**     ��������:         ISD4004_stopwrdn
**    ��������:          ���͵���ָ��
**    ��ڲ�����       	 ��
**    ���ڲ���:          ��
********************************************************************************************************/ 
void ISD4004_stopwrdn(void)//
{
	delay_us(10);
	ISD4004_send(ISD4004_STOPWRDN_CMD);
	SS=1;
	delay_ms(50);
}

/******************************************************************************************************** 
**     ��������:         ISD4004_rec
**    ��������:          ����¼��ָ��
**    ��ڲ�����       	 ��
**    ���ڲ���:          ��
********************************************************************************************************/ 
void ISD4004_rec(void)
{
	ISD4004_send(ISD4004_REC_CMD);
	SS=1;
}
/******************************************************************************************************** 
**     ��������:         ISD4004_setrec
**    ��������:         ����setrecָ��
**    ��ڲ�����       	 unsigned char adl,unsigned char adh�����ͷ�����ʼ��ַ��λ����ʼ��ַ��λ
**    ���ڲ���:          ��
********************************************************************************************************/ 
void ISD4004_setrec(unsigned char adl,unsigned char adh)
{
	delay_ms(1);
	ISD4004_send(adl); //���ͷ�����ʼ��ַ��λ
	delay_us(2);
	ISD4004_send(adh); //���ͷ�����ʼ��ַ��λ
	delay_us(2);
	ISD4004_send(ISD4004_SETREC_CMD); //��ISD4004_setrec���ֽ�
	SS=1;
}
/******************************************************************************************************** 
**     ��������:         ISD4004_play
**    ��������:          ����playָ��
**    ��ڲ�����       	 ��
**    ���ڲ���:          ��
********************************************************************************************************/ 
void ISD4004_play(void)
{
	ISD4004_send(ISD4004_PLAY_CMD);
	SS=1;
}
/******************************************************************************************************** 
**     ��������:         ISD4004_setplay
**    ��������:          ����setplayָ��
**    ��ڲ�����       	 uchar adl,uchar adh ����¼����ʼ��ַ��λ����ʼ��ַ��λ
**    ���ڲ���:          ��
********************************************************************************************************/ 
void ISD4004_setplay(uchar adl,uchar adh)
{
	delay_ms(1);
	ISD4004_send(adl); //���ͷ�����ʼ��ַ��λ
	delay_us(2);
	ISD4004_send(adh); //���ͷ�����ʼ��ַ��λ
	delay_us(2);
	ISD4004_send(ISD4004_SETPLAY_CMD); //����setplayָ���ֽ�
	SS=1;
}

/******************************************************************************************************** 
**     ��������:         ISD4004_StopPlaying
**    ��������:          ֹͣ��ǰ����
**    ��ڲ�����       	 ��
**    ���ڲ���:          ��
********************************************************************************************************/ 
void ISD4004_StopPlaying(void)
{
	request = 1;
	prt("ISD4004_StopPlaying \r\n");
}

