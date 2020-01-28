/*
	文件名：      ISD4004_task.h
	文件描述： ISD4004_task任务函数实现
	修改时间： 2019年5月1日
	作者：	     汪武林
	其他说明： 无
	
*/
#ifndef ISD4004_TASK_H_
#define ISD4004_TASK_H_


#include "includes.h"
#include "ISD4004.h"
/*
1.海洋大学到了，请带好行李物品从后门下车
2.海尾村到了，要下车的乘客请带好行李物品从后门下车
3.临东村到了，要下车的乘客请带好行李物品从后门下车
4.木兰村到了，要下车的乘客请带好行李物品从后门下车
5.宝满村到了，要下车的乘客请带好行李物品从后门下车
6.石头村到了，要下车的乘客请带好行李物品从后门下车
7.龙划村到了，要下车的乘客请带好行李物品从后门下车
8.湖光路口到了，要下车的乘客请带好行李物品从后门下车
9.汽车南站到了，请带好行李物品从后门下车
10.下一站是海洋大学
11.下一站是海尾村
12.下一站是临东村
13.下一站是木兰村
14.下一站是宝满村
15.下一站是石头村
16.下一站是龙划村
17.下一站是湖光路口
18.下一站是汽车南站
19.本站是班车的终点站，感谢您乘坐一一四公交车
20.欢迎乘坐一一四公交车，本次班车从海洋大学开往汽车南站
21.欢迎乘坐一一四公交车，本次班车从汽车南站开往海洋大学
22.尊老爱幼是中华名族的传统美德，请您为有需要的人让座
23.欢迎使用斯特尔公交语音播报系统
24.车辆颠簸过大，请坐好扶稳

25.Ocean University arrived, please bring your luggage and get off the back door.
26.Haiwei Village arrived, passengers who want to get off, please bring your luggage and get off the back door.
27.Lindong Village arrived, passengers who want to get off, please bring your luggage and get off the back door.
28.Mulan Village arrived,, passengers who want to get off, please bring your luggage and get off the back door.
29.Baoman Village arrived, passengers who want to get off, please bring your luggage and get off the back door.
30.Stone Village arrived, passengers who want to get off, please bring your luggage and get off the back door.
31.LonghuaVillage arrived, passengers who want to get off, please bring your luggage and get off the back door.
32.Huguang intersection arrived, passengers who want to get off, please bring your luggage and get off the back door.
33.The South Bus Station arrived, please bring your luggage and get off the back door.
34.The next stop is Ocean University.
35.The next stop is Haiwei Village.
36.The next stop is Lindong Village.
37.The next stop is Mulan Village.
38.The next stop is Baoman Village.
39.The next stop is Stone Village.
40.The next stop is Longzuo Village.
41.The next stop is the lake light junction.
42.The next stop is the South Bus Station.
43.This station is the terminus of the shuttle bus. Thank you for taking the bus.
44.Welcome to take the one or four bus, this bus from Ocean University to South Bus Station.
45.Welcome to take the 1 1 4 bus, this bus from the South Bus Station to Ocean University.
46.Respecting the old and loving the young is the traditional virtue of the Chinese celebrities. Please give a seat to those in need.
47.Welcome to the Steer Bus Voice Broadcasting System.
48.The car is shaking too much, please sit still.
*/
//语音地址
#define  ISD4004_VADDR_1    1900

#define  ISD4004_VADDR_2    207  

#define  ISD4004_VADDR_3    241  

#define  ISD4004_VADDR_4    275  

#define  ISD4004_VADDR_5    308  

#define  ISD4004_VADDR_6    344  

#define  ISD4004_VADDR_7    380  

#define  ISD4004_VADDR_8    417  

#define  ISD4004_VADDR_9    1942  

#define  ISD4004_VADDR_10    479  

#define  ISD4004_VADDR_11    492  

#define  ISD4004_VADDR_12    170  

#define  ISD4004_VADDR_13    1981  

#define  ISD4004_VADDR_14    663  

#define  ISD4004_VADDR_15    686  

#define  ISD4004_VADDR_16    707  

#define  ISD4004_VADDR_17    729  

#define  ISD4004_VADDR_18    752  

#define  ISD4004_VADDR_19    776  

#define  ISD4004_VADDR_20    815  

#define  ISD4004_VADDR_21    859  

#define  ISD4004_VADDR_22    906  

#define  ISD4004_VADDR_23    948  

#define  ISD4004_VADDR_24    978  

#define  ISD4004_VADDR_25    1005  

#define  ISD4004_VADDR_26    1045  

#define  ISD4004_VADDR_27    1089  

#define  ISD4004_VADDR_28    1139  

#define  ISD4004_VADDR_29    1188  

#define  ISD4004_VADDR_30    1235  

#define  ISD4004_VADDR_31    1281  

#define  ISD4004_VADDR_32    1331  

#define  ISD4004_VADDR_33    1381  

#define  ISD4004_VADDR_34    1418  

#define  ISD4004_VADDR_35    1441  

#define  ISD4004_VADDR_36    1465  

#define  ISD4004_VADDR_37    1486  

#define  ISD4004_VADDR_38    1510  

#define  ISD4004_VADDR_39    1533  

#define  ISD4004_VADDR_40    1558  

#define  ISD4004_VADDR_41    1581  

#define  ISD4004_VADDR_42    1608  

#define  ISD4004_VADDR_43    1631  

#define  ISD4004_VADDR_44    1671  

#define  ISD4004_VADDR_45    1712  

#define  ISD4004_VADDR_46    1762  

#define  ISD4004_VADDR_47    1826  

#define  ISD4004_VADDR_48    1854  


//语音事件种类
#define ISD4004_STA1_REACH  0
#define ISD4004_STA2_REACH  1
#define ISD4004_STA3_REACH  2
#define ISD4004_STA4_REACH  3
#define ISD4004_STA5_REACH  4
#define ISD4004_STA6_REACH  5
#define ISD4004_STA7_REACH  6
#define ISD4004_STA8_REACH  7
#define ISD4004_STA9_REACH  8
#define ISD4004_OTHER_FLAG  10
#define ISD4004_TURN_RIGHT_FLAG  12
#define ISD4004_TURN_LEFT_FLAG 	 13
#define ISD4004_ROLL_TOO_FLAG	 14
#define ISD4004_SPEEDING_FLAG	 15
#define ISD4004_SYSTEMINIT_FLAG	 16
#define ISD4004_POSITIVE_DIRECTION_FLAG 17
#define ISD4004_REVERSE_DIRECTION_FLAG 18

#define POSITIVE_DIRCT 1u  //正方向
#define REVERSE_DIRCT  0u   //反方向
extern vu8 direction;
//任务优先级
#define ISD4004_TASK_PRIO		4u
//任务堆栈大小	
#define ISD4004_STK_SIZE 		128u
//任务控制块
extern OS_TCB ISD4004TaskTCB;
//任务堆栈	
extern CPU_STK ISD4004_TASK_STK[ISD4004_STK_SIZE];

void ISD4004_task(void *p_arg);
void Play_Voice(u8 voice_kind);

#endif
