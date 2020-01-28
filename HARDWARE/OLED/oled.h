/*
	文件名：      oled.h
	文件描述： STM32F03C8T6---OLED_Device Driver
	修改时间： 2018年4月21日
	作者：	     汪武林
	其他说明： 汉字字体用PCtoLCD2002.exe生成
	
				汉字格式：阴码格式，列行式，逆序，16进制，8个像素 字体点阵16*64  c51 0x行前后{}
				图形格式：阴码格式，列行式，逆序，16进制，8个像素 16*16  c51  0x 行前后，，
				要显示的文字放到chinesefont.h对应的字符数组里面，也可以自定义数组
				本例程只支持16*16汉字，汉字数组为char[][16]类型
*/
#ifndef __OLED_H
#define __OLED_H	
		  	 
#include "sys.h"
#include "stdlib.h"	    

//-----------------OLED IIC端口定义----------------  					   
#define OLED_SCL_PIN              GPIO_Pin_11
#define OLED_SDA_PIN              GPIO_Pin_10
#define OLED_GPIO_PORT            GPIOB
#define OLED_GPIO_CLK             RCC_APB2Periph_GPIOB

#define OLED_SCLK		 PBout(11)	 //CLK
#define OLED_SDA_W		 PBout(10) 	 //DIN
#define OLED_SDA_R		 PBin(10) 
 		     
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
//IO方向设置
#define OLED_SDA_IN()  {GPIOB->CRH&=0xFFFFF0FF;GPIOB->CRH|=8<<(4*2);}	//PB10输入模式
#define OLED_SDA_OUT() {GPIOB->CRH&=0xFFFFF0FF;GPIOB->CRH|=3<<(4*2);} 	//PB10输出模式 


#define OLED9_6   1

		    						  
//OLED通用控制用函数
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

//带显存OLED显示函数						   		    
void OLED_Fresh(void);
void OLED_Clear_Gram(void)  ;
void OLED_Fill1(u8 x1,u8 x2,u8 y);
void OLED_ShowChar1(u8 x,u8 y,u8 chr);
void OLED_ShowNum1(u8 x,u8 y,u32 num,u8 len);
void OLED_ShowString1(u8 x,u8 y, u8 *p);	
void OLED_ShowCHinese1(u8 x,u8 y,const char(*ch)[16],u8 no);
void OLED_ShowCHineseString1(u8 x,u8 y,const char(*ch)[16],u16 length);
#endif  
	 



