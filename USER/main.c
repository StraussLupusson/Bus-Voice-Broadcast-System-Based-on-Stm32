/*
	�ļ�����      main.c
	�ļ������� 	 ������
	�޸�ʱ�䣺 2019��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/

#include "sys.h"
#include "delay.h"
#include "task.h"
#include "usart.h"
/******************************************************************************************************** 
**     ��������:         main
**    ��������:          ������
**    ��ڲ�����       ��
**    ���ڲ���:        ��
**    ����˵����       ��ʼ�����ڣ�������ʼ����
********************************************************************************************************/ 
 int main(void)
 {	
	OS_ERR err;
	CPU_SR_ALLOC();
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	//uart_init(115200);
	delay_ms(1000);
	
	//printf("BVBS PROJECT START \r\n");
	OSInit(&err); 
	OS_CRITICAL_ENTER();
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);	
	
	OS_CRITICAL_EXIT();
	OSStart(&err);
	
	while(1)
	{
		
	}
 
 }
