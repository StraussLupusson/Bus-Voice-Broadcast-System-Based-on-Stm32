/*
	�ļ�����      ISD4004_task.h
	�ļ������� ISD4004_task������ʵ��
	�޸�ʱ�䣺 2019��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/
#ifndef ISD4004_TASK_H_
#define ISD4004_TASK_H_


#include "includes.h"
#include "ISD4004.h"
/*
1.�����ѧ���ˣ������������Ʒ�Ӻ����³�
2.��β�嵽�ˣ�Ҫ�³��ĳ˿������������Ʒ�Ӻ����³�
3.�ٶ��嵽�ˣ�Ҫ�³��ĳ˿������������Ʒ�Ӻ����³�
4.ľ���嵽�ˣ�Ҫ�³��ĳ˿������������Ʒ�Ӻ����³�
5.�����嵽�ˣ�Ҫ�³��ĳ˿������������Ʒ�Ӻ����³�
6.ʯͷ�嵽�ˣ�Ҫ�³��ĳ˿������������Ʒ�Ӻ����³�
7.�����嵽�ˣ�Ҫ�³��ĳ˿������������Ʒ�Ӻ����³�
8.����·�ڵ��ˣ�Ҫ�³��ĳ˿������������Ʒ�Ӻ����³�
9.������վ���ˣ������������Ʒ�Ӻ����³�
10.��һվ�Ǻ����ѧ
11.��һվ�Ǻ�β��
12.��һվ���ٶ���
13.��һվ��ľ����
14.��һվ�Ǳ�����
15.��һվ��ʯͷ��
16.��һվ��������
17.��һվ�Ǻ���·��
18.��һվ��������վ
19.��վ�ǰ೵���յ�վ����л������һһ�Ĺ�����
20.��ӭ����һһ�Ĺ����������ΰ೵�Ӻ����ѧ����������վ
21.��ӭ����һһ�Ĺ����������ΰ೵��������վ���������ѧ
22.���ϰ������л�����Ĵ�ͳ���£�����Ϊ����Ҫ��������
23.��ӭʹ��˹�ض�������������ϵͳ
24.�����������������÷���

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
//������ַ
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


//�����¼�����
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

#define POSITIVE_DIRCT 1u  //������
#define REVERSE_DIRCT  0u   //������
extern vu8 direction;
//�������ȼ�
#define ISD4004_TASK_PRIO		4u
//�����ջ��С	
#define ISD4004_STK_SIZE 		128u
//������ƿ�
extern OS_TCB ISD4004TaskTCB;
//�����ջ	
extern CPU_STK ISD4004_TASK_STK[ISD4004_STK_SIZE];

void ISD4004_task(void *p_arg);
void Play_Voice(u8 voice_kind);

#endif
