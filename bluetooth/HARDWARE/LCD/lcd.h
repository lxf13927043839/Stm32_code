#ifndef __LCD_H
#define __LCD_H		
#include "systick.h"
#include "sys.h"
#include "stdlib.h"

//////////////////////////////////////////////////////////////////////////////////	 
 
//LCD驱动参数
extern  u16  lcd_id;         //LCD ID
extern  u8   dir_flag;       //横屏还是竖屏控制：0，竖屏；1，横屏。	
extern  u16  lcd_width;      //LCD 宽度
extern  u16  lcd_height;     //LCD 高度
extern  u16	 write_gramcmd;	 //写gram指令
extern	u16  setxcmd;		     //设置x坐标指令
extern	u16  setycmd;		     //设置y坐标指令	 

//LCD的画笔颜色和背景色	   
extern u16  BRUSH_COLOR;//默认红色    
extern u16  BACK_COLOR; //背景颜色.默认为白色


//////////////////////////////////////////////////////////////////////////////////	 
//-----------------LCD背光端口定义---------------- 
#define	LCD_BACK PFout(10)  //LCD背光    	PF10 	    

//A12作为数据命令区分线  设置时STM32内部会右移一位对齐	    
#define  CMD_BASE     ((u32)(0x6C000000 | 0x00001FFE))
#define  DATA_BASE    ((u32)(0x6C000000 | 0x00002000))

#define LCD_CMD       ( * (u16 *) CMD_BASE )
#define LCD_DATA      ( * (u16 *) DATA_BASE)
	 
//扫描方向定义
#define  L2R_U2D  0 //从左到右,从上到下
#define  L2R_D2U  1 //从左到右,从下到上
#define  R2L_U2D  2 //从右到左,从上到下
#define  R2L_D2U  3 //从右到左,从下到上

#define  U2D_L2R  4 //从上到下,从左到右
#define  U2D_R2L  5 //从上到下,从右到左
#define  D2U_L2R  6 //从下到上,从左到右
#define  D2U_R2L  7 //从下到上,从右到左	 

//颜色值定义
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

void LCD_init(void);													   	//初始化
void LCD_DisplayOn(void);													//开显示
void LCD_DisplayOff(void);												//关显示
void LCD_Clear(u16 Color);	 											//清屏
void LCD_SetCursor(u16 Xpos, u16 Ypos);						//设置光标
void LCD_DrawPoint(u16 x,u16 y);									//画点
void LCD_Color_DrawPoint(u16 x,u16 y,u16 color);	//颜色画点
u16  LCD_GetPoint(u16 x,u16 y); 								  //读点 

void LCD_Open_Window(u16 X0,u16 Y0,u16 width,u16 height); 	
void Set_Scan_Direction(u8 direction);					 
void Set_Display_Mode(u8 mode);						 

void LCD_Fill_onecolor(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);		//填充单个颜色
void LCD_Draw_Picture(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);		//填充指定颜色
void LCD_DisplayChar(u16 x,u16 y,u8 word,u8 size);						      //显示一个字符
void LCD_DisplayString(u16 x,u16 y,u8 size,u8 *p);		           //显示一个12/16/24字体字符串
void LCD_DisplayString_color(u16 x,u16 y,u8 size,u8 *p,u16 brushcolor,u16 backcolor); //显示一个12/16/24字体自定义颜色的字符串
void LCD_DisplayNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);				//显示 数字
void LCD_DisplayNum_color(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode,u16 brushcolor,u16 backcolor); //显示自定义颜色数字	  	   																			 

//add function
void LCD_DisplayChinese_one(u16 x,u16 y,u8 word,u8 size);
void LCD_DisplayChinese_string(u16 x,u16 y,u8 size,int *p);

#endif  

//int chinese[50]={1,2,3,4,5,-1};

//int main()
//{
//	int key_id=-1;
//	
//	LED_init();
//	Systick_init(168);  //初始化延时函数，没有初始化会导致程序卡死
//	
//	UART1_init(115200);
//	KEY_init();
//	
// 	LCD_init();           //初始化LCD FSMC接口和显示驱动
//	BRUSH_COLOR=RED;      //设置画笔颜色为红色
//	
//	LCD_DisplayString(10,10,24,"Illuminati STM32F4");	
//	LCD_DisplayString(20,40,16,"Author:Clever");
//	LCD_DisplayString(30,80,24,"4.TFTLCD TEST");
//	LCD_DisplayString(50,110,16,"KEY0:Screen Test");	
//	LCD_DisplayString(50,130,16,"KEY1:Number Test");	
//	 
//	
//	LCD_DisplayChinese_one(120 ,200 ,3, 24);
//	
//	
//	LCD_DisplayChinese_string(10,224,24,chinese);
//	
//	
//	printf("start \n");
//	
//	
//	while(1)
//	{
//			key_id=key_scanf(0);
//			switch(key_id)
//			{
//				case 1: 
//					LCD_Clear(BLACK);
//					break;
//				case 2:
//					LCD_Clear(BLUE);
//					break;
//				case 3:
//					LCD_Clear(RED);
//					break;
//				case 4:
//					LCD_Clear(MAGENTA);
//					break;
//				
//			}
//		
//	}

//}

	 



