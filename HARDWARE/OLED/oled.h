/*
	�ļ�����      oled.h
	�ļ������� STM32F03C8T6---OLED_Device Driver
	�޸�ʱ�䣺 2018��4��21��
	���ߣ�	     ������
	����˵���� ����������PCtoLCD2002.exe����
	
				���ָ�ʽ�������ʽ������ʽ������16���ƣ�8������ �������16*64  c51 0x��ǰ��{}
				ͼ�θ�ʽ�������ʽ������ʽ������16���ƣ�8������ 16*16  c51  0x ��ǰ�󣬣�
				Ҫ��ʾ�����ַŵ�chinesefont.h��Ӧ���ַ��������棬Ҳ�����Զ�������
				������ֻ֧��16*16���֣���������Ϊchar[][16]����
*/
#ifndef __OLED_H
#define __OLED_H	
		  	 
#include "sys.h"
#include "stdlib.h"	    

//-----------------OLED IIC�˿ڶ���----------------  					   
#define OLED_SCL_PIN              GPIO_Pin_11
#define OLED_SDA_PIN              GPIO_Pin_10
#define OLED_GPIO_PORT            GPIOB
#define OLED_GPIO_CLK             RCC_APB2Periph_GPIOB

#define OLED_SCLK		 PBout(11)	 //CLK
#define OLED_SDA_W		 PBout(10) 	 //DIN
#define OLED_SDA_R		 PBin(10) 
 		     
#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����
//IO��������
#define OLED_SDA_IN()  {GPIOB->CRH&=0xFFFFF0FF;GPIOB->CRH|=8<<(4*2);}	//PB10����ģʽ
#define OLED_SDA_OUT() {GPIOB->CRH&=0xFFFFF0FF;GPIOB->CRH|=3<<(4*2);} 	//PB10���ģʽ 


#define OLED9_6   1

		    						  
//OLEDͨ�ÿ����ú���
void OLED_Send(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 x2,u8 y);
void OLED_ShowChar(u8 x,u8 y,u8 chr);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len);
void OLED_ShowString(u8 x,u8 y, u8 *p);	 
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(u8 x,u8 y,const char(*ch)[16],u8 no);
void OLED_ShowCHineseString(u8 x,u8 y,const char(*ch)[16],u16 length);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,const unsigned char BMP[]);

//���Դ�OLED��ʾ����						   		    
void OLED_Fresh(void);
void OLED_Clear_Gram(void)  ;
void OLED_Fill1(u8 x1,u8 x2,u8 y);
void OLED_ShowChar1(u8 x,u8 y,u8 chr);
void OLED_ShowNum1(u8 x,u8 y,u32 num,u8 len);
void OLED_ShowString1(u8 x,u8 y, u8 *p);	
void OLED_ShowCHinese1(u8 x,u8 y,const char(*ch)[16],u8 no);
void OLED_ShowCHineseString1(u8 x,u8 y,const char(*ch)[16],u16 length);
#endif  
	 



