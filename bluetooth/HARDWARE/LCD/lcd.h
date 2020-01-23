#ifndef __LCD_H
#define __LCD_H		
#include "systick.h"
#include "sys.h"
#include "stdlib.h"

//////////////////////////////////////////////////////////////////////////////////	 
 
//LCD��������
extern  u16  lcd_id;         //LCD ID
extern  u8   dir_flag;       //���������������ƣ�0��������1��������	
extern  u16  lcd_width;      //LCD ���
extern  u16  lcd_height;     //LCD �߶�
extern  u16	 write_gramcmd;	 //дgramָ��
extern	u16  setxcmd;		     //����x����ָ��
extern	u16  setycmd;		     //����y����ָ��	 

//LCD�Ļ�����ɫ�ͱ���ɫ	   
extern u16  BRUSH_COLOR;//Ĭ�Ϻ�ɫ    
extern u16  BACK_COLOR; //������ɫ.Ĭ��Ϊ��ɫ


//////////////////////////////////////////////////////////////////////////////////	 
//-----------------LCD����˿ڶ���---------------- 
#define	LCD_BACK PFout(10)  //LCD����    	PF10 	    

//A12��Ϊ��������������  ����ʱSTM32�ڲ�������һλ����	    
#define  CMD_BASE     ((u32)(0x6C000000 | 0x00001FFE))
#define  DATA_BASE    ((u32)(0x6C000000 | 0x00002000))

#define LCD_CMD       ( * (u16 *) CMD_BASE )
#define LCD_DATA      ( * (u16 *) DATA_BASE)
	 
//ɨ�跽����
#define  L2R_U2D  0 //������,���ϵ���
#define  L2R_D2U  1 //������,���µ���
#define  R2L_U2D  2 //���ҵ���,���ϵ���
#define  R2L_D2U  3 //���ҵ���,���µ���

#define  U2D_L2R  4 //���ϵ���,������
#define  U2D_R2L  5 //���ϵ���,���ҵ���
#define  D2U_L2R  6 //���µ���,������
#define  D2U_R2L  7 //���µ���,���ҵ���	 

//��ɫֵ����
#define  WHITE        0xFFFF
#define  BLACK        0x0000	
#define  RED          0xF800
#define  GREEN        0x07E0
#define  BLUE         0x001F 
#define  BRED         0XF81F
#define  GRED 			  0XFFE0
#define  GBLUE			  0X07FF
#define  BROWN 			  0XBC40  
#define  BRRED 			  0XFC07  
#define  GRAY  			  0X8430  
#define  MAGENTA      0xF81F
#define  CYAN         0x7FFF
#define  YELLOW       0xFFE0

	    			
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_Value);
u16 LCD_ReadReg(u16 LCD_Reg);
void LCD_WriteGRAM(void);

void LCD_init(void);													   	//��ʼ��
void LCD_DisplayOn(void);													//����ʾ
void LCD_DisplayOff(void);												//����ʾ
void LCD_Clear(u16 Color);	 											//����
void LCD_SetCursor(u16 Xpos, u16 Ypos);						//���ù��
void LCD_DrawPoint(u16 x,u16 y);									//����
void LCD_Color_DrawPoint(u16 x,u16 y,u16 color);	//��ɫ����
u16  LCD_GetPoint(u16 x,u16 y); 								  //���� 

void LCD_Open_Window(u16 X0,u16 Y0,u16 width,u16 height); 	
void Set_Scan_Direction(u8 direction);					 
void Set_Display_Mode(u8 mode);						 

void LCD_Fill_onecolor(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);		//��䵥����ɫ
void LCD_Draw_Picture(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);		//���ָ����ɫ
void LCD_DisplayChar(u16 x,u16 y,u8 word,u8 size);						      //��ʾһ���ַ�
void LCD_DisplayString(u16 x,u16 y,u8 size,u8 *p);		           //��ʾһ��12/16/24�����ַ���
void LCD_DisplayString_color(u16 x,u16 y,u8 size,u8 *p,u16 brushcolor,u16 backcolor); //��ʾһ��12/16/24�����Զ�����ɫ���ַ���
void LCD_DisplayNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);				//��ʾ ����
void LCD_DisplayNum_color(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode,u16 brushcolor,u16 backcolor); //��ʾ�Զ�����ɫ����	  	   																			 

//add function
void LCD_DisplayChinese_one(u16 x,u16 y,u8 word,u8 size);
							  		 
#endif  
	 
	 



