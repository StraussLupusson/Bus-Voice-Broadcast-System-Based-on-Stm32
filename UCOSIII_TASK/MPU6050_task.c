/*
	�ļ�����      MPU6050_task.c
	�ļ������� MPU6050_task������ʵ��
	�޸�ʱ�䣺 2019��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/
#include "MPU6050_task.h"
#include "start_task.h"
#include "sys.h"
#include "usart.h"
#include "MPU6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "delay.h"
#include "isd4004_task.h"
#include "oled_task.h"

bus_attitude Bus_Attitude; //������̬��
//������ƿ�
OS_TCB MPU6050TaskTCB;
//�����ջ	
CPU_STK MPU6050_TASK_STK[MPU6050_STK_SIZE];



/******************************************************************************************************** 
**     ��������:        MPU6050_task
**    ��������://		MPU6050������
**    ��ڲ�����       (void *p_arg)
**    ���ڲ���:          ��
**    ����˵����       ��
********************************************************************************************************/ 
void MPU6050_task(void *p_arg)
{
	float pitch1,roll1,yaw1; 		//ŷ����
	float pitch2,roll2,yaw2; 		//ŷ����
	short  temp;
	short gyrox,gyroy,gyroz;
	short aacx,aacy,aacz;
	
	//short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����
	//short gyrox,gyroy,gyroz;	//������ԭʼ����
	//short temp;		
	OS_ERR err;
	p_arg = p_arg;
	 
	MPU6050_Init(); 
	prt("mpu task start \r\n");
	while(mpu_dmp_init())
 	{
		MPU6050_Init();
		//prt("mpu_dmp_init Error  trying get\r\n");
		delay_ms(500);
		temp=MPU_Get_Temperature();	//�õ��¶�ֵ
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������
		//prt(" temp :%hd \r\n",temp);
		//prt("aacx:%hd,aacy:%hd,aacz:%hd \r\n",aacx,aacy,aacz);
		//prt("gyrox:%hd,gyroy:%hd,gyroz:%hd \r\n",gyrox,gyroy,gyroz);
		
	} 
	//mpu_dmp_init();
	prt("MPU6050 OK \r\n");
	memset(&Bus_Attitude,0,sizeof(Bus_Attitude));
 	while(1)
	{
		memset(&Bus_Attitude,0,sizeof(Bus_Attitude));
		mpu_dmp_get_data(&pitch1,&roll1,&yaw1);
		 
			temp=MPU_Get_Temperature();	//�õ��¶�ֵ
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������
			
			//prt("pitch:%.3f,roll:%.3f,yaw:%.3f \r\n",pitch1,roll1,yaw1);
	
		delay_ms(ACQUISITION_INTERVAL);
		mpu_dmp_get_data(&pitch2,&roll2,&yaw2);
		Bus_Attitude.pith = MPU6050_AttitudeAnalysis(pitch1,pitch2);
		Bus_Attitude.roll = MPU6050_AttitudeAnalysis(roll1,roll2);
		Bus_Attitude.yaw = MPU6050_AttitudeAnalysis(yaw1,yaw2);
		//prt("yawvary :%d",Bus_Attitude.yaw); // right- +left
		//prt("rollvary :%d",Bus_Attitude.roll);// right- +left
		//prt("pitchvary :%d",Bus_Attitude.pith);// up- +down
		
		if(Bus_Attitude.yaw>5 && abs(Bus_Attitude.roll)<5)
		{
			OSFlagPost(&oledevent_flag,FLAG_BIT(OLED_TURN_LEFT_FLAG),OS_OPT_POST_FLAG_SET,&err);
			//if(Bus_Attitude.yaw>20)
				//OSFlagPost(&isd4004event_flag,FLAG_BIT(ISD4004_TURN_LEFT_FLAG),OS_OPT_POST_FLAG_SET,&err);
				OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ800ms
		}
		if(Bus_Attitude.yaw<-5 && abs(Bus_Attitude.roll)<5)
		{
			OSFlagPost(&oledevent_flag,FLAG_BIT(OLED_TURN_RIGHT_FLAG),OS_OPT_POST_FLAG_SET,&err);
			//if(Bus_Attitude.yaw<-20)
			//OSFlagPost(&isd4004event_flag,FLAG_BIT(ISD4004_TURN_RIGHT_FLAG),OS_OPT_POST_FLAG_SET,&err);
			OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ800ms
		}	
		
		if(abs(Bus_Attitude.roll)>5)
		{
			OSFlagPost(&oledevent_flag,FLAG_BIT(OLED_ROLL_TOO_FLAG),OS_OPT_POST_FLAG_SET,&err);
			OSFlagPost(&isd4004event_flag,FLAG_BIT(ISD4004_ROLL_TOO_FLAG),OS_OPT_POST_FLAG_SET,&err);
			OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ800ms
		}
	}
	
}

	
	
