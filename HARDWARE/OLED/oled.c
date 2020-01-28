/*
	文件名：      oled.c
	文件描述： STM32F03C8T6---OLED_Device Driver
	修改时间： 2018年4月21日
	作者：	     汪武林
	其他说明： 无
	
*/
	#include "oled.h" 
	#include "delay.h"
	#include <string.h>
	#include "usart.h"
	#include "delay.h"
	#include "font.h"
	
	//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
static OS_ERR err;
#endif
	
  /******************************************************************************************************** 
**     函数名称:         OLED_I2C_Start
**    功能描述:          OLEDI2C起始信号
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void OLED_I2C_Start(void)
{
	OLED_SDA_OUT();
	//浮空双线输出拉高
	OLED_SCLK = 1;
	OLED_SDA_W = 1;
	delay_us(4);
	OLED_SDA_W = 0;//SDA线由高变低
	delay_us(4);
	OLED_SCLK = 0;//钳住时钟总线， 准备数据接受
}
/******************************************************************************************************** 
**     函数名称:         OLED_I2C_Stop
**    功能描述:          OLED_I2C停止时序
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void OLED_I2C_Stop(void)
{
	OLED_SDA_OUT();
	//浮空双线输出拉高
	OLED_SCLK = 0;
	OLED_SDA_W = 0;
	delay_us(4);
	OLED_SCLK = 1;
	OLED_SDA_W = 1;
	delay_us(4);
	
}
/******************************************************************************************************** 
**     函数名称:         OLED_I2C_WaitAnswer
**    功能描述:          OLED_I2C等待应答
**    入口参数：       无
**    出口参数:        无
**    其他说明：       成功从机返回低电平
********************************************************************************************************/ 
void OLED_I2C_WaitAnswer(void)
{
	u8 t=0;
	OLED_SDA_IN();//SDA 设置为输入 
	OLED_SDA_W=1;
	delay_us(2); 
	OLED_SCLK=1;
	delay_us(2);
	while(OLED_SDA_R) //成功从机返回低电平,退出等待
	{
		t++;
		if(t>250)
		{
			OLED_I2C_Stop();
			break;
		}
	}
	OLED_SCLK=0;
}
/******************************************************************************************************** 
**     函数名称:         OLED_I2C_Answer
**    功能描述:           OLED_I2C主机发送应答
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void OLED_I2C_Answer(void)
{
	OLED_SDA_OUT();
	OLED_SCLK=0;
	OLED_SDA_W=0;
	delay_us(2);
	OLED_SCLK=1;
	delay_us(2);
	OLED_SCLK=0;
}
/******************************************************************************************************** 
**     函数名称:         OLED_I2C_NoAnswer
**    功能描述:           OLED_I2C主机不发送应答
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void OLED_I2C_NoAnswer(void)
{
	OLED_SDA_OUT();
	OLED_SCLK=0;
	OLED_SDA_W=1;
	delay_us(2);
	OLED_SCLK=1;
	delay_us(2);
	OLED_SCLK=0;
} 
/******************************************************************************************************** 
**     函数名称:         OLED_I2C_Send_Byte
**    功能描述:          IIC 发送一个字节 
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 
void OLED_I2C_Send_Byte(u8 msg)
{ 
	u8 t; 
	OLED_SDA_OUT();
	OLED_SCLK = 0;//拉低时钟开始数据传输
	 for(t=0;t<8;t++)
	 { 
		OLED_SDA_W=(msg&0x80)>>7;//I2C_SDA_W  = msg&(1<<(7-i));
		msg<<=1; 
		delay_us(2); 
		OLED_SCLK=1;
		delay_us(2); 
		OLED_SCLK=0;
		delay_us(2);
	  }
}
/******************************************************************************************************** 
**     函数名称:         OLED_Send
**    功能描述:        主机给 OLED发送字节函数
**    入口参数：       u8 cmd,u8 mode
**    出口参数:        无
**    其他说明：       第一个参数表示要发送的数据；第二个表示选择数据还是命令，OLED_CMD写命令
						OLED_DATA 写数据
********************************************************************************************************/ 
 void OLED_Send(u8 cmd,u8 mode)
{
	OLED_I2C_Start();               //开始信号
    OLED_I2C_Send_Byte(0x78);       //向地址为0x3c设备发送数据
	OLED_I2C_WaitAnswer();
    if(mode)	
		OLED_I2C_Send_Byte(0x40);       //发送的是命令还是数据？（0x00命令）（0x40数据）
	else
		OLED_I2C_Send_Byte(0x00);
	OLED_I2C_WaitAnswer(); 	 	
    OLED_I2C_Send_Byte(cmd);        //命令码
	OLED_I2C_WaitAnswer(); 	 	
    OLED_I2C_Stop();                //停止信号
}
 /******************************************************************************************************** 
**     函数名称:         OLED_Init
**    功能描述:          OLED初始化
**    入口参数：       无
**    出口参数:          无
**    其他说明：       无
********************************************************************************************************/ 				    
void OLED_Init(void)
{ 	 		 
 
	//端口配置初始化
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(OLED_GPIO_CLK, ENABLE);	 //使能A端口时钟

	GPIO_InitStructure.GPIO_Pin =  OLED_SCL_PIN | OLED_SDA_PIN  ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(OLED_GPIO_PORT, &GPIO_InitStructure);
	
#ifdef OLED9_6
	delay_ms(200);
	//OLED 寄存器初始化
	OLED_Send(0xAE,OLED_CMD);//--display off
	OLED_Send(0x00,OLED_CMD);//---set low column address
	OLED_Send(0x10,OLED_CMD);//---set high column address
	OLED_Send(0x40,OLED_CMD);//--set start line address  
	OLED_Send(0xB0,OLED_CMD);//--set page address
	OLED_Send(0x81,OLED_CMD); // contract control
	OLED_Send(0xFF,OLED_CMD);//--128   
	OLED_Send(0xA1,OLED_CMD);//set segment remap 
	OLED_Send(0xA6,OLED_CMD);//--normal / reverse
	OLED_Send(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_Send(0x3F,OLED_CMD);//--1/32 duty
	OLED_Send(0xC8,OLED_CMD);//Com scan direction
	OLED_Send(0xD3,OLED_CMD);//-set display offset
	OLED_Send(0x00,OLED_CMD);//
	
	OLED_Send(0xD5,OLED_CMD);//set osc division
	OLED_Send(0x80,OLED_CMD);//
	
	OLED_Send(0xD8,OLED_CMD);//set area color mode off
	OLED_Send(0x05,OLED_CMD);//
	
	OLED_Send(0xD9,OLED_CMD);//Set Pre-Charge Period
	OLED_Send(0xF1,OLED_CMD);//
	
	OLED_Send(0xDA,OLED_CMD);//set com pin configuartion
	OLED_Send(0x12,OLED_CMD);//
	
	OLED_Send(0xDB,OLED_CMD);//set Vcomh
	OLED_Send(0x30,OLED_CMD);//
	
	OLED_Send(0x8D,OLED_CMD);//set charge pump enable
	OLED_Send(0x14,OLED_CMD);//
	
	OLED_Send(0xAF,OLED_CMD);//--turn on oled panel
	OLED_Clear();
#else
	delay_ms(200);
	OLED_Send(0xAE,OLED_CMD);//关闭显示
	
	OLED_Send(0x40,OLED_CMD);//---set low column address
	OLED_Send(0xB0,OLED_CMD);//---set high column address

	OLED_Send(0xC8,OLED_CMD);//-not offset

	OLED_Send(0x81,OLED_CMD);//设置对比度
	OLED_Send(0xff,OLED_CMD);

	OLED_Send(0xa1,OLED_CMD);//段重定向设置

	OLED_Send(0xa6,OLED_CMD);//
	
	OLED_Send(0xa8,OLED_CMD);//设置驱动路数
	OLED_Send(0x1f,OLED_CMD);
	
	OLED_Send(0xd3,OLED_CMD);
	OLED_Send(0x00,OLED_CMD);
	
	OLED_Send(0xd5,OLED_CMD);
	OLED_Send(0xf0,OLED_CMD);
	
	OLED_Send(0xd9,OLED_CMD);
	OLED_Send(0x22,OLED_CMD);
	
	OLED_Send(0xda,OLED_CMD);
	OLED_Send(0x02,OLED_CMD);
	
	OLED_Send(0xdb,OLED_CMD);
	OLED_Send(0x49,OLED_CMD);
	
	OLED_Send(0x8d,OLED_CMD);
	OLED_Send(0x14,OLED_CMD);
	
	OLED_Send(0xaf,OLED_CMD);
	OLED_Clear();

#endif
}  

/******************************************************************************************************** 
**     函数名称:         OLED_Display_On
**    功能描述:          开启OLED显示
**    入口参数：       无
**    出口参数:        无
**    其他说明：       无
********************************************************************************************************/  	      
void OLED_Display_On(void)
{
	OLED_Send(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_Send(0X14,OLED_CMD);  //DCDC ON
	OLED_Send(0XAF,OLED_CMD);  //DISPLAY ON
}

/******************************************************************************************************** 
**     函数名称:         OLED_Display_Off
**    功能描述:         关闭OLED显示 
**    入口参数：       无
**    出口参数:         无
**    其他说明：       无
********************************************************************************************************/     
void OLED_Display_Off(void)
{
	OLED_Send(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_Send(0X10,OLED_CMD);  //DCDC OFF
	OLED_Send(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 


/******************************************************************************************************** 
**     函数名称:         OLED_Set_Pos
**    功能描述:        //坐标设置
**    入口参数：      unsigned char x, unsigned char y
**    出口参数:          无
**    其他说明：       第一个参数表示第几列，第二个参数表示第几行，x:0~127  y:0~8
********************************************************************************************************/ 
	void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	OLED_Send(0xb0+y,OLED_CMD);
	OLED_Send(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_Send((x&0x0f),OLED_CMD); 
}   	  
 
/******************************************************************************************************** 
**     函数名称:         OLED_Clear
**    功能描述:          清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	 
**    入口参数：       无
**    出口参数:         无
**    其他说明：       无
********************************************************************************************************/  
void OLED_Clear(void)  
{
	u8 i,n;		  
	for(i=0;i<8;i++)  
	{  
		OLED_Send (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_Send (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_Send (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_Send(0,OLED_DATA); 
	} //更新显示
}

/******************************************************************************************************** 
**     函数名称:         OLED_Fill
**    功能描述:         选择性清屏
**    入口参数：       u8 x1,u8 x2,u8 y
**    出口参数:         无
**    其他说明：        x1 x2 表示列宽范围 0~127  y表示第几行 0~3
********************************************************************************************************/ 	
void OLED_Fill(u8 x1,u8 x2,u8 y)  
{  
	u8 n,i;
	for(i=2*y;i<2*(y+1);i++) 
	{
		OLED_Send (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_Send(((x1&0xf0)>>4)|0x10,OLED_CMD);
		OLED_Send((x1&0x0f),OLED_CMD); 
		for(n=x1;n<x2;n++)OLED_Send(0,OLED_DATA); 
	}
}
/******************************************************************************************************** 
**     函数名称:         OLED_Fill1
**    功能描述:         选择性清屏
**    入口参数：       u8 x1,u8 x2,u8 y
**    出口参数:         无
**    其他说明：        x1 x2 表示列宽范围 0~127  y表示第几行 0~3
********************************************************************************************************/ 	
void OLED_Fill1(u8 x1,u8 x2,u8 y)
{
	u8 z,i;  
	for(z=x1;z<=x2;z++)
	{
		for(i=16*y;i<16*(y+1);i++)OLED_DrawPoint(z,i,0);
	}
	OLED_Fresh();//更新显示
	
}
/******************************************************************************************************** 
**     函数名称:         OLED_ShowChar
**    功能描述:         在指定位置显示一个字符,包括部分字符
**    入口参数：       u8 x,u8 y,u8 chr
**    出口参数:         无
**    其他说明：       u8 x,列，u8 y行，u8 chr显示字符//x:0~127  y:0~3
********************************************************************************************************/ 
void OLED_ShowChar(u8 x,u8 y,u8 chr)
{      	
		
		unsigned char c=0,i=0;
		y = y*2;	//将八页设置为四页	
		c=chr-' ';//得到偏移后的值			
		if(x>128-1){x=0;y=y+1;}
		
			OLED_Set_Pos(x,y);	
			for(i=0;i<8;i++)
			OLED_Send(F8X16[c*16+i],OLED_DATA);
			OLED_Set_Pos(x,y+1);
			for(i=0;i<8;i++)
			OLED_Send(F8X16[c*16+i+8],OLED_DATA);
			
}

/******************************************************************************************************** 
**     函数名称:        oled_pow
**    功能描述:          //m^n函数
**    入口参数：       u8 m,u8 n
**    出口参数:          u32
**    其他说明：       无
********************************************************************************************************/ 
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  


/******************************************************************************************************** 
**     函数名称:         OLED_ShowNum
**    功能描述:         显示数字
**    入口参数：       (u8 x,u8 y,u32 num,u8 len)
**    出口参数:          无
**    其他说明：       x,y :起点坐标	len :数字的位数   num:数值(0~4294967295);
						//x:0~127  y:0~3
********************************************************************************************************/ 	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len)
{         	
	u8 t,temp;
	u8 enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+8*t,y,' ');
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+8*t,y,temp+'0'); 
	}
} 

/******************************************************************************************************** 
**     函数名称:        OLED_ShowString
**    功能描述:          //显示一个字符号串
**    入口参数：       (u8 x,u8 y,u8 *chr)
**    出口参数:          无
**    其他说明：      //x:0~127  y:0~7  chr显示的字符串
********************************************************************************************************/ 
void OLED_ShowString(u8 x,u8 y,u8 *chr)
{

	unsigned char j=0;
	while (chr[j]!='\0')
	{		OLED_ShowChar(x,y,chr[j]);
			x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}


/******************************************************************************************************** 
**     函数名称:         OLED_ShowCHinese
**    功能描述:          //显示一个汉字
**    入口参数：       u8 x,u8 y,char(*ch)[16],u8 no
**    出口参数:          无
**    其他说明：       x:0~127  y:0~3   ch 汉字点阵数组  no 数组里第几个汉字，从0算起
********************************************************************************************************/ 
void OLED_ShowCHinese(u8 x,u8 y,const char(*ch)[16],u8 no)
{      			    
	u8 t,adder=0;
	y = y*2;
	OLED_Set_Pos(x,y);	
    for(t=0;t<16;t++)
		{
				OLED_Send(ch[2*no][t],OLED_DATA);
				adder+=1;
     }	
		OLED_Set_Pos(x,y+1);	
    for(t=0;t<16;t++)
			{	
				OLED_Send(ch[2*no+1][t],OLED_DATA);
				adder+=1;
      }					
}

/******************************************************************************************************** 
**     函数名称:         OLED_ShowCHineseString
**    功能描述:          //显示汉字串
**    入口参数：       (u8 x,u8 y,char(*ch)[16],u16 length)
**    出口参数:          无
**    其他说明：        x:0~127  y:0~3   ch 汉字点阵数组  length汉字个数
********************************************************************************************************/ 
void OLED_ShowCHineseString(u8 x,u8 y,const char(*ch)[16],u16 length)
{   

	u8 t2;
	for(t2 = 0;t2<length;t2++)
	{
		OLED_ShowCHinese(x+t2*16,y,ch,t2);
	}		
}
/******************************************************************************************************** 
**     函数名称:         OLED_DrawBMP
**    功能描述:          显示显示BMP图片
**    入口参数：       unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]
**    出口参数:          无
**    其他说明：       128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7
********************************************************************************************************/ 
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,const unsigned char BMP[])
{ 	
 unsigned int j=0;
 unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	OLED_Send(BMP[j++],OLED_DATA);	    	
	    }
	}
} 
/********************************************带显存显示OLED驱动函数*******************************************/
 u8 OLED_GRAM[128][8];	 

/******************************************************************************************************** 
**     函数名称:         OLED_Fresh
**    功能描述:         //更新显存到LCD	
**    入口参数：       无
**    出口参数:          无
**    其他说明：        无
********************************************************************************************************/ 	 
void OLED_Fresh(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_Send (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_Send (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_Send (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_Send(OLED_GRAM[n][i],OLED_DATA); 
	}   
}
/******************************************************************************************************** 
**     函数名称:         OLED_Clear_Gram
**    功能描述:         //清除显存	
**    入口参数：       无
**    出口参数:          无
**    其他说明：        无
********************************************************************************************************/ 
  	void OLED_Clear_Gram(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
	OLED_Fresh();//更新显示
}    			 
 
/******************************************************************************************************** 
**     函数名称:         OLED_DrawPoint
**    功能描述:         画点函数
**    入口参数：       u8 x,u8 y,u8 t
**    出口参数:         无
**    其他说明：        x  y表示坐标 t=1,表示画点， t=0 表示清除点
********************************************************************************************************/ 			   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
	pos=y/8;
	bx=y%8;
	temp=1<<bx;
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}
/******************************************************************************************************** 
**     函数名称:         OLED_ShowChar1
**    功能描述:         带显存显示在指定位置显示一个字符,包括部分字符
**    入口参数：       u8 x,u8 y,u8 chr
**    出口参数:         无
**    其他说明：       u8 x,列，u8 y行，u8 chr显示字符//x:0~127  y:0~3
********************************************************************************************************/ 
void OLED_ShowChar1(u8 x,u8 y,u8 chr)
{      	
		unsigned char c=0,i=0,t = 0;
		c=chr-' ';//得到偏移后的值			
		if(x>127){x=0;y=y+1;}
		for(t=0;t<8;t++)
		{
				for(i=0;i<8;i++)
			 {
				if(F8X16[c*16+t]&(1<<i)) OLED_DrawPoint(x+t,y*16+i,1);
				else OLED_DrawPoint(x+t,y*16+i,0);
			 }
		}
		for(t=0;t<8;t++)
		{
				for(i=0;i<8;i++)
			 {
				if(F8X16[c*16+8+t]&(1<<i)) OLED_DrawPoint(x+t,y*16+8+i,1);
				else OLED_DrawPoint(x+t,y*16+i,0);
			 }			

		}
}
/******************************************************************************************************** 
**     函数名称:         OLED_ShowNum1
**    功能描述:         带显存显示数字
**    入口参数：       (u8 x,u8 y,u32 num,u8 len)
**    出口参数:          无
**    其他说明：       x,y :起点坐标	len :数字的位数   num:数值(0~4294967295);
						//x:0~127  y:0~7
********************************************************************************************************/ 	
void OLED_ShowNum1(u8 x,u8 y,u32 num,u8 len)
{         	
	u8 t,temp;
	u8 enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar1(x+8*t,y,' ');
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar1(x+8*t,y,temp+'0'); 
	}
} 

/******************************************************************************************************** 
**     函数名称:        OLED_ShowString1
**    功能描述:          //带显存显示一个字符号串
**    入口参数：       (u8 x,u8 y,u8 *chr)
**    出口参数:          无
**    其他说明：      //x:0~127  y:0~7  chr显示的字符串
********************************************************************************************************/ 
void OLED_ShowString1(u8 x,u8 y,u8 *chr)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		OLED_ShowChar1(x,y,chr[j]);
			x+=8;
		if(x>120){x=0;y+=1;}
			j++;
	}
}
/******************************************************************************************************** 
**     函数名称:         OLED_ShowCHinese1
**    功能描述:          //显示一个汉字
**    入口参数：       u8 x,u8 y,char(*ch)[16],u8 no
**    出口参数:          无
**    其他说明：       x:0~127  y:0~3   ch 汉字点阵数组  no 数组里第几个汉字，从0算起
********************************************************************************************************/ 
void OLED_ShowCHinese1(u8 x,u8 y,const char(*ch)[16],u8 no)
{      			    
	u8 t,i;
	y = y*16;
    for(t=0;t<16;t++)
	{
		 for(i=0;i<8;i++)
		 {
			if(ch[2*no][t]&(1<<i)) OLED_DrawPoint(x+t,y+i,1);
			else OLED_DrawPoint(x+t,y+i,0);
		 }
	}		
    for(t=0;t<16;t++)
	{	
			for(i=0;i<8;i++)
		 {
			if(ch[2*no+1][t]&(1<<i)) OLED_DrawPoint(x+t,y+8+i,1);
			else OLED_DrawPoint(x+t,y+8+i,0);
		 }
    }					
}
/******************************************************************************************************** 
**     函数名称:         OLED_ShowCHineseString1
**    功能描述:          //显示汉字串
**    入口参数：       u8 x,u8 y,char(*ch)[16],u8 no
**    出口参数:          无
**    其他说明：       x:0~127  y:0~3   ch 汉字点阵数组  length 汉字长度
********************************************************************************************************/ 
void OLED_ShowCHineseString1(u8 x,u8 y,const char(*ch)[16],u16 length)
{   
	u8 t2; 
	for(t2 = 0;t2<length;t2++)
	{
		OLED_ShowCHinese1(x+t2*16,y,ch,t2);
	}		
}