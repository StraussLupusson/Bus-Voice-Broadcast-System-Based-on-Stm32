/*
	�ļ�����      oled.c
	�ļ������� STM32F03C8T6---OLED_Device Driver
	�޸�ʱ�䣺 2018��4��21��
	���ߣ�	     ������
	����˵���� ��
	
*/
	#include "oled.h" 
	#include "delay.h"
	#include <string.h>
	#include "usart.h"
	#include "delay.h"
	#include "font.h"
	
	//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
static OS_ERR err;
#endif
	
  /******************************************************************************************************** 
**     ��������:         OLED_I2C_Start
**    ��������:          OLEDI2C��ʼ�ź�
**    ��ڲ�����       ��
**    ���ڲ���:          ��
**    ����˵����       ��
********************************************************************************************************/ 
void OLED_I2C_Start(void)
{
	OLED_SDA_OUT();
	//����˫���������
	OLED_SCLK = 1;
	OLED_SDA_W = 1;
	delay_us(4);
	OLED_SDA_W = 0;//SDA���ɸ߱��
	delay_us(4);
	OLED_SCLK = 0;//ǯסʱ�����ߣ� ׼�����ݽ���
}
/******************************************************************************************************** 
**     ��������:         OLED_I2C_Stop
**    ��������:          OLED_I2Cֹͣʱ��
**    ��ڲ�����       ��
**    ���ڲ���:          ��
**    ����˵����       ��
********************************************************************************************************/ 
void OLED_I2C_Stop(void)
{
	OLED_SDA_OUT();
	//����˫���������
	OLED_SCLK = 0;
	OLED_SDA_W = 0;
	delay_us(4);
	OLED_SCLK = 1;
	OLED_SDA_W = 1;
	delay_us(4);
	
}
/******************************************************************************************************** 
**     ��������:         OLED_I2C_WaitAnswer
**    ��������:          OLED_I2C�ȴ�Ӧ��
**    ��ڲ�����       ��
**    ���ڲ���:        ��
**    ����˵����       �ɹ��ӻ����ص͵�ƽ
********************************************************************************************************/ 
void OLED_I2C_WaitAnswer(void)
{
	u8 t=0;
	OLED_SDA_IN();//SDA ����Ϊ���� 
	OLED_SDA_W=1;
	delay_us(2); 
	OLED_SCLK=1;
	delay_us(2);
	while(OLED_SDA_R) //�ɹ��ӻ����ص͵�ƽ,�˳��ȴ�
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
**     ��������:         OLED_I2C_Answer
**    ��������:           OLED_I2C��������Ӧ��
**    ��ڲ�����       ��
**    ���ڲ���:          ��
**    ����˵����       ��
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
**     ��������:         OLED_I2C_NoAnswer
**    ��������:           OLED_I2C����������Ӧ��
**    ��ڲ�����       ��
**    ���ڲ���:          ��
**    ����˵����       ��
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
**     ��������:         OLED_I2C_Send_Byte
**    ��������:          IIC ����һ���ֽ� 
**    ��ڲ�����       ��
**    ���ڲ���:          ��
**    ����˵����       ��
********************************************************************************************************/ 
void OLED_I2C_Send_Byte(u8 msg)
{ 
	u8 t; 
	OLED_SDA_OUT();
	OLED_SCLK = 0;//����ʱ�ӿ�ʼ���ݴ���
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
**     ��������:         OLED_Send
**    ��������:        ������ OLED�����ֽں���
**    ��ڲ�����       u8 cmd,u8 mode
**    ���ڲ���:        ��
**    ����˵����       ��һ��������ʾҪ���͵����ݣ��ڶ�����ʾѡ�����ݻ������OLED_CMDд����
						OLED_DATA д����
********************************************************************************************************/ 
 void OLED_Send(u8 cmd,u8 mode)
{
	OLED_I2C_Start();               //��ʼ�ź�
    OLED_I2C_Send_Byte(0x78);       //���ַΪ0x3c�豸��������
	OLED_I2C_WaitAnswer();
    if(mode)	
		OLED_I2C_Send_Byte(0x40);       //���͵�����������ݣ���0x00�����0x40���ݣ�
	else
		OLED_I2C_Send_Byte(0x00);
	OLED_I2C_WaitAnswer(); 	 	
    OLED_I2C_Send_Byte(cmd);        //������
	OLED_I2C_WaitAnswer(); 	 	
    OLED_I2C_Stop();                //ֹͣ�ź�
}
 /******************************************************************************************************** 
**     ��������:         OLED_Init
**    ��������:          OLED��ʼ��
**    ��ڲ�����       ��
**    ���ڲ���:          ��
**    ����˵����       ��
********************************************************************************************************/ 				    
void OLED_Init(void)
{ 	 		 
 
	//�˿����ó�ʼ��
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(OLED_GPIO_CLK, ENABLE);	 //ʹ��A�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin =  OLED_SCL_PIN | OLED_SDA_PIN  ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(OLED_GPIO_PORT, &GPIO_InitStructure);
	
#ifdef OLED9_6
	delay_ms(200);
	//OLED �Ĵ�����ʼ��
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
	OLED_Send(0xAE,OLED_CMD);//�ر���ʾ
	
	OLED_Send(0x40,OLED_CMD);//---set low column address
	OLED_Send(0xB0,OLED_CMD);//---set high column address

	OLED_Send(0xC8,OLED_CMD);//-not offset

	OLED_Send(0x81,OLED_CMD);//���öԱȶ�
	OLED_Send(0xff,OLED_CMD);

	OLED_Send(0xa1,OLED_CMD);//���ض�������

	OLED_Send(0xa6,OLED_CMD);//
	
	OLED_Send(0xa8,OLED_CMD);//��������·��
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
**     ��������:         OLED_Display_On
**    ��������:          ����OLED��ʾ
**    ��ڲ�����       ��
**    ���ڲ���:        ��
**    ����˵����       ��
********************************************************************************************************/  	      
void OLED_Display_On(void)
{
	OLED_Send(0X8D,OLED_CMD);  //SET DCDC����
	OLED_Send(0X14,OLED_CMD);  //DCDC ON
	OLED_Send(0XAF,OLED_CMD);  //DISPLAY ON
}

/******************************************************************************************************** 
**     ��������:         OLED_Display_Off
**    ��������:         �ر�OLED��ʾ 
**    ��ڲ�����       ��
**    ���ڲ���:         ��
**    ����˵����       ��
********************************************************************************************************/     
void OLED_Display_Off(void)
{
	OLED_Send(0X8D,OLED_CMD);  //SET DCDC����
	OLED_Send(0X10,OLED_CMD);  //DCDC OFF
	OLED_Send(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 


/******************************************************************************************************** 
**     ��������:         OLED_Set_Pos
**    ��������:        //��������
**    ��ڲ�����      unsigned char x, unsigned char y
**    ���ڲ���:          ��
**    ����˵����       ��һ��������ʾ�ڼ��У��ڶ���������ʾ�ڼ��У�x:0~127  y:0~8
********************************************************************************************************/ 
	void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	OLED_Send(0xb0+y,OLED_CMD);
	OLED_Send(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_Send((x&0x0f),OLED_CMD); 
}   	  
 
/******************************************************************************************************** 
**     ��������:         OLED_Clear
**    ��������:          ��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	 
**    ��ڲ�����       ��
**    ���ڲ���:         ��
**    ����˵����       ��
********************************************************************************************************/  
void OLED_Clear(void)  
{
	u8 i,n;		  
	for(i=0;i<8;i++)  
	{  
		OLED_Send (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_Send (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_Send (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_Send(0,OLED_DATA); 
	} //������ʾ
}

/******************************************************************************************************** 
**     ��������:         OLED_Fill
**    ��������:         ѡ��������
**    ��ڲ�����       u8 x1,u8 x2,u8 y
**    ���ڲ���:         ��
**    ����˵����        x1 x2 ��ʾ�п�Χ 0~127  y��ʾ�ڼ��� 0~3
********************************************************************************************************/ 	
void OLED_Fill(u8 x1,u8 x2,u8 y)  
{  
	u8 n,i;
	for(i=2*y;i<2*(y+1);i++) 
	{
		OLED_Send (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_Send(((x1&0xf0)>>4)|0x10,OLED_CMD);
		OLED_Send((x1&0x0f),OLED_CMD); 
		for(n=x1;n<x2;n++)OLED_Send(0,OLED_DATA); 
	}
}
/******************************************************************************************************** 
**     ��������:         OLED_Fill1
**    ��������:         ѡ��������
**    ��ڲ�����       u8 x1,u8 x2,u8 y
**    ���ڲ���:         ��
**    ����˵����        x1 x2 ��ʾ�п�Χ 0~127  y��ʾ�ڼ��� 0~3
********************************************************************************************************/ 	
void OLED_Fill1(u8 x1,u8 x2,u8 y)
{
	u8 z,i;  
	for(z=x1;z<=x2;z++)
	{
		for(i=16*y;i<16*(y+1);i++)OLED_DrawPoint(z,i,0);
	}
	OLED_Fresh();//������ʾ
	
}
/******************************************************************************************************** 
**     ��������:         OLED_ShowChar
**    ��������:         ��ָ��λ����ʾһ���ַ�,���������ַ�
**    ��ڲ�����       u8 x,u8 y,u8 chr
**    ���ڲ���:         ��
**    ����˵����       u8 x,�У�u8 y�У�u8 chr��ʾ�ַ�//x:0~127  y:0~3
********************************************************************************************************/ 
void OLED_ShowChar(u8 x,u8 y,u8 chr)
{      	
		
		unsigned char c=0,i=0;
		y = y*2;	//����ҳ����Ϊ��ҳ	
		c=chr-' ';//�õ�ƫ�ƺ��ֵ			
		if(x>128-1){x=0;y=y+1;}
		
			OLED_Set_Pos(x,y);	
			for(i=0;i<8;i++)
			OLED_Send(F8X16[c*16+i],OLED_DATA);
			OLED_Set_Pos(x,y+1);
			for(i=0;i<8;i++)
			OLED_Send(F8X16[c*16+i+8],OLED_DATA);
			
}

/******************************************************************************************************** 
**     ��������:        oled_pow
**    ��������:          //m^n����
**    ��ڲ�����       u8 m,u8 n
**    ���ڲ���:          u32
**    ����˵����       ��
********************************************************************************************************/ 
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  


/******************************************************************************************************** 
**     ��������:         OLED_ShowNum
**    ��������:         ��ʾ����
**    ��ڲ�����       (u8 x,u8 y,u32 num,u8 len)
**    ���ڲ���:          ��
**    ����˵����       x,y :�������	len :���ֵ�λ��   num:��ֵ(0~4294967295);
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
**     ��������:        OLED_ShowString
**    ��������:          //��ʾһ���ַ��Ŵ�
**    ��ڲ�����       (u8 x,u8 y,u8 *chr)
**    ���ڲ���:          ��
**    ����˵����      //x:0~127  y:0~7  chr��ʾ���ַ���
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
**     ��������:         OLED_ShowCHinese
**    ��������:          //��ʾһ������
**    ��ڲ�����       u8 x,u8 y,char(*ch)[16],u8 no
**    ���ڲ���:          ��
**    ����˵����       x:0~127  y:0~3   ch ���ֵ�������  no ������ڼ������֣���0����
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
**     ��������:         OLED_ShowCHineseString
**    ��������:          //��ʾ���ִ�
**    ��ڲ�����       (u8 x,u8 y,char(*ch)[16],u16 length)
**    ���ڲ���:          ��
**    ����˵����        x:0~127  y:0~3   ch ���ֵ�������  length���ָ���
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
**     ��������:         OLED_DrawBMP
**    ��������:          ��ʾ��ʾBMPͼƬ
**    ��ڲ�����       unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]
**    ���ڲ���:          ��
**    ����˵����       128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7
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
/********************************************���Դ���ʾOLED��������*******************************************/
 u8 OLED_GRAM[128][8];	 

/******************************************************************************************************** 
**     ��������:         OLED_Fresh
**    ��������:         //�����Դ浽LCD	
**    ��ڲ�����       ��
**    ���ڲ���:          ��
**    ����˵����        ��
********************************************************************************************************/ 	 
void OLED_Fresh(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_Send (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_Send (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_Send (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_Send(OLED_GRAM[n][i],OLED_DATA); 
	}   
}
/******************************************************************************************************** 
**     ��������:         OLED_Clear_Gram
**    ��������:         //����Դ�	
**    ��ڲ�����       ��
**    ���ڲ���:          ��
**    ����˵����        ��
********************************************************************************************************/ 
  	void OLED_Clear_Gram(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
	OLED_Fresh();//������ʾ
}    			 
 
/******************************************************************************************************** 
**     ��������:         OLED_DrawPoint
**    ��������:         ���㺯��
**    ��ڲ�����       u8 x,u8 y,u8 t
**    ���ڲ���:         ��
**    ����˵����        x  y��ʾ���� t=1,��ʾ���㣬 t=0 ��ʾ�����
********************************************************************************************************/ 			   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//������Χ��.
	pos=y/8;
	bx=y%8;
	temp=1<<bx;
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}
/******************************************************************************************************** 
**     ��������:         OLED_ShowChar1
**    ��������:         ���Դ���ʾ��ָ��λ����ʾһ���ַ�,���������ַ�
**    ��ڲ�����       u8 x,u8 y,u8 chr
**    ���ڲ���:         ��
**    ����˵����       u8 x,�У�u8 y�У�u8 chr��ʾ�ַ�//x:0~127  y:0~3
********************************************************************************************************/ 
void OLED_ShowChar1(u8 x,u8 y,u8 chr)
{      	
		unsigned char c=0,i=0,t = 0;
		c=chr-' ';//�õ�ƫ�ƺ��ֵ			
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
**     ��������:         OLED_ShowNum1
**    ��������:         ���Դ���ʾ����
**    ��ڲ�����       (u8 x,u8 y,u32 num,u8 len)
**    ���ڲ���:          ��
**    ����˵����       x,y :�������	len :���ֵ�λ��   num:��ֵ(0~4294967295);
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
**     ��������:        OLED_ShowString1
**    ��������:          //���Դ���ʾһ���ַ��Ŵ�
**    ��ڲ�����       (u8 x,u8 y,u8 *chr)
**    ���ڲ���:          ��
**    ����˵����      //x:0~127  y:0~7  chr��ʾ���ַ���
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
**     ��������:         OLED_ShowCHinese1
**    ��������:          //��ʾһ������
**    ��ڲ�����       u8 x,u8 y,char(*ch)[16],u8 no
**    ���ڲ���:          ��
**    ����˵����       x:0~127  y:0~3   ch ���ֵ�������  no ������ڼ������֣���0����
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
**     ��������:         OLED_ShowCHineseString1
**    ��������:          //��ʾ���ִ�
**    ��ڲ�����       u8 x,u8 y,char(*ch)[16],u8 no
**    ���ڲ���:          ��
**    ����˵����       x:0~127  y:0~3   ch ���ֵ�������  length ���ֳ���
********************************************************************************************************/ 
void OLED_ShowCHineseString1(u8 x,u8 y,const char(*ch)[16],u16 length)
{   
	u8 t2; 
	for(t2 = 0;t2<length;t2++)
	{
		OLED_ShowCHinese1(x+t2*16,y,ch,t2);
	}		
}