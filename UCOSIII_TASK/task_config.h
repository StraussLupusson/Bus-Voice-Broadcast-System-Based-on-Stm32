/*
	文件名：      task_config.c
	文件描述： 任务配置函数
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/
#ifndef __TASK_CONFIG_H_
#define __TASK_CONFIG_H_

//任务编译配置
#define MPU6050TaskTCB_CONFIG 1
#define GPS_DATADEALTaskTCB_CONFIG 0
#define GPSTaskTCB_CONFIG     1
#define KEYTaskTCB_CONFIG     1
#define OLEDTaskTCB_CONFIG 1
#define ISD4004TaskTCB_CONFIG 1
#define ISD4004_RECORDTaskTCB_CONFIG 0
#define DHT11TaskTCB_CONFIG  0
#define SMOKE_SENSORTaskTCB_CONFIG 	0
#define GETTIMETaskTCB_CONFIG  1
#define OTHERTaskTCB_CONFIG  1

#define CHINESE  1
#define ENGLISH  2
#define MIXING   3

#define DEFAULT_LANGUAGE  CHINESE  //默认中文
extern volatile unsigned char languagesetting ;
#endif