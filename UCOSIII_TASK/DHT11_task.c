/*
	�ļ�����      DHT11_task.c
	�ļ������� DHT11_task������ʵ��
	�޸�ʱ�䣺 2019��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/
#include "DHT11_task.h"
#include "sys.h"
#include "usart.h"
#include <string.h>
#include "start_task.h"
//������ƿ�
OS_TCB DHT11TaskTCB;
//�����ջ	
CPU_STK DHT11_TASK_STK[DHT11_STK_SIZE];
vu8 DHT11_OK_FLAG = 0;
char temp_buf[4];
char humity_buf[5];


/******************************************************************************************************** 
**     ��������:         DHT11_task
**    ��������:          //DHT11������
**    ��ڲ�����       (void *p_arg)
**    ���ڲ���:          ��
**    ����˵����       ��
********************************************************************************************************/ 
void DHT11_task(void *p_arg)
{
	CPU_SR_ALLOC();
	DHT11_Data DHT11_data;
	OS_ERR err;
	u8 ret;
	p_arg = p_arg;
	prt("DHT11  start \r\n");
	while(DHT11_Init() == ERROR)
	{
		prt("DHT11  time out ");
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms
	}
	
	while(1)
	{
		//memset(&DHT11_data,0,sizeof(DHT11_Data));
		 if(DHT11_Read_Data(&DHT11_data) == ERROR)
			DHT11_OK_FLAG = 0;
		else DHT11_OK_FLAG = 1;
		//prt("wendu:%.3f,shidu:%.3f \r\n",DHT11_data.wendu,DHT11_data.shidu);
		OS_CRITICAL_ENTER();
		sprintf(temp_buf,"%.0f",DHT11_data.wendu);
		sprintf(humity_buf,"%.0f%%",DHT11_data.shidu);
		OS_CRITICAL_EXIT();	
		//prt("dht11 task");
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms
		
		//OSTimeDlyHMSM(0,0,1,999,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ200ms
	}
}
