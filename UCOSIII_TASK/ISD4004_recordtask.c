/*
	�ļ�����      ISD4004_recordtask.c
	�ļ������� ISD4004_recordtask������ʵ��
	�޸�ʱ�䣺 2019��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/
#include "ISD4004_recordtask.h"
#include "LED.h"
#include "sys.h"
#include "usart.h"
#include "ISD4004_task.h"
#include "busposmesg.h"
#include "task_config.h"
//������ƿ�
OS_TCB ISD4004_RECORDTaskTCB;
//�����ջ	
CPU_STK ISD4004_RECORD_TASK_STK[ISD4004_RECORD_STK_SIZE];

//static u16 VoiceAddr[POS_NUM] = {170,270,370,470,570,670,770,870,970,1070};
/******************************************************************************************************** 
**     ��������:         ISD4004_RECORD_task
**    ��������:        //ISD4004_RECORD������
**    ��ڲ�����       (void *p_arg)
**    ���ڲ���:         ��
**    ����˵����       ��
********************************************************************************************************/ 
#if ISD4004_RECORDTaskTCB_CONFIG
void ISD4004_RECORD_task(void *p_arg)
{

	OS_ERR err;
	u8 i;
	
	p_arg = p_arg;
	prt("ISD4004_RECORD_task  start \r\n");
	ISD4004_Init();
	
	while(1)
	{
		//ISD4004_Playing(ISD4004_VADDR_8);
		ISD4004_RecordingTreat();
		ISD4004_PlayingTreat();
		ISD4004_Display();
		//OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ800ms	
//	for(i = 0;i <VOICE_PASSAGE_NUM; i++)
//	{
//			ISD4004_Playing(addr[i]);
//			
//	}
	}
}
#endif
