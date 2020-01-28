/*
	�ļ�����      OLED_task.h
	�ļ������� OLED_task������ʵ��
	�޸�ʱ�䣺 2019��5��1��
	���ߣ�	     ������
	����˵���� ��
	
*/
#ifndef __OLED_TASK_H_
#define __OLED_TASK_H_


#include "includes.h"

//��ʾ�¼���־
#define STA1_REACH 0
#define STA2_REACH 1
#define STA3_REACH 2
#define STA4_REACH 3
#define STA5_REACH 4
#define STA6_REACH 5
#define STA7_REACH 6
#define STA8_REACH 7
#define STA9_REACH 8
#define STA10_REACH 9

#define SELECT_STA1  10
#define SELECT_STA2  11
#define SELECT_STA3  12
#define SELECT_STA4  13
#define SELECT_STA5  14
#define SELECT_STA6  15
#define SELECT_STA7  16
#define SELECT_STA8  17
#define SELECT_STA9  18
#define SELECT_STA10 19
#define CANCEL_SELECT 20
#define OLED_TURN_LEFT_FLAG 21
#define OLED_TURN_RIGHT_FLAG 22
#define OLED_ROLL_TOO_FLAG 23
#define OLED_SPEEDING_FLAG 24
#define OLED_OTHER1_FLAG 25
#define OLED_OTHER2_FLAG 26
#define OLED_OTHER3_FLAG 27
#define OLED_OTHER4_FLAG 28
#define KEY_SETING_FLAG 29
#define POSITIVE_DIRCT 1u  //������
#define REVERSE_DIRCT  0u   //������
//�������ȼ�
#define OLED_TASK_PRIO		4u
//�����ջ��С	
#define OLED_STK_SIZE 		256u
//������ƿ�
extern OS_TCB OLEDTaskTCB;
//�����ջ	
extern CPU_STK OLED_TASK_STK[OLED_STK_SIZE];

void OLED_task(void *p_arg);

extern vu8 direction;
#endif