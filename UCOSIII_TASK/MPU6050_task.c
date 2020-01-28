/*
	文件名：      MPU6050_task.c
	文件描述： MPU6050_task任务函数实现
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
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

bus_attitude Bus_Attitude; //车的姿态角
//任务控制块
OS_TCB MPU6050TaskTCB;
//任务堆栈	
CPU_STK MPU6050_TASK_STK[MPU6050_STK_SIZE];



/******************************************************************************************************** 
**     函数名称:        MPU6050_task
**    功能描述://		MPU6050任务函数
**    入口参数：       (void *p_arg)
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void MPU6050_task(void *p_arg)
{
	float pitch1,roll1,yaw1; 		//欧拉角
	float pitch2,roll2,yaw2; 		//欧拉角
	short  temp;
	short gyrox,gyroy,gyroz;
	short aacx,aacy,aacz;
	
	//short aacx,aacy,aacz;		//加速度传感器原始数据
	//short gyrox,gyroy,gyroz;	//陀螺仪原始数据
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
		temp=MPU_Get_Temperature();	//得到温度值
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据
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
		 
			temp=MPU_Get_Temperature();	//得到温度值
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据
			
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
				OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时800ms
		}
		if(Bus_Attitude.yaw<-5 && abs(Bus_Attitude.roll)<5)
		{
			OSFlagPost(&oledevent_flag,FLAG_BIT(OLED_TURN_RIGHT_FLAG),OS_OPT_POST_FLAG_SET,&err);
			//if(Bus_Attitude.yaw<-20)
			//OSFlagPost(&isd4004event_flag,FLAG_BIT(ISD4004_TURN_RIGHT_FLAG),OS_OPT_POST_FLAG_SET,&err);
			OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时800ms
		}	
		
		if(abs(Bus_Attitude.roll)>5)
		{
			OSFlagPost(&oledevent_flag,FLAG_BIT(OLED_ROLL_TOO_FLAG),OS_OPT_POST_FLAG_SET,&err);
			OSFlagPost(&isd4004event_flag,FLAG_BIT(ISD4004_ROLL_TOO_FLAG),OS_OPT_POST_FLAG_SET,&err);
			OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时800ms
		}
	}
	
}

	
	
