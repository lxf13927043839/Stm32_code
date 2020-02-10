#ifndef __IMAGE2LCD_H
#define __IMAGE2LCD_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//FLASH图片显示		   
////////////////////////////////////////////////////////////////////////////////// 	

/*
(1)		typedef __packed struct READ_Command
			{
				u_char code;
				u_int addr;
				u_char len;
			} READ_Command;
(2)		typedef  struct READ_Command
			{
				u_char code;
				u_int addr;
				u_char len;
			} READ_Command
没有__packed的会出现字对齐等也就是，char型的有能是占用4个字节的
长度的内存空间
有packed的就不会,就肯定是1个字节的内存空间
__packed只是某种编译器的格式压缩，有的是pack,对不同的cpu压缩的对齐
方式也是不一样
		 
*/

//4096色/16位真彩色/18位真彩色/24位真彩色/32位真彩色
//图像数据头结构体  
__packed typedef struct _HEADCOLOR
{
   unsigned char scan;
   unsigned char gray;
   unsigned short w;
   unsigned short h;
   unsigned char is565;
   unsigned char rgb;
}HEADCOLOR;  
//scan: 扫描模式 	    
//Bit7: 0:自左至右扫描，1:自右至左扫描。 
//Bit6: 0:自顶至底扫描，1:自底至顶扫描。 
//Bit5: 0:字节内象素数据从高位到低位排列，1:字节内象素数据从低位到高位排列。 
//Bit4: 0:WORD类型高低位字节顺序与PC相同，1:WORD类型高低位字节顺序与PC相反。 
//Bit3~2: 保留。 
//Bit1~0: [00]水平扫描，[01]垂直扫描，[10]数据水平,字节垂直，[11]数据垂直,字节水平。 
//gray: 灰度值 
//   灰度值，1:单色，2:四灰，4:十六灰，8:256色，12:4096色，16:16位彩色，24:24位彩色，32:32位彩色。
//w: 图像的宽度。	 
//h: 图像的高度。
//is565: 在4096色模式下为0表示使用[16bits(WORD)]格式，此时图像数据中每个WORD表示一个象素；为1表示使用[12bits(连续字节流)]格式，此时连续排列的每12Bits代表一个象素。
//在16位彩色模式下为0表示R G B颜色分量所占用的位数都为5Bits，为1表示R G B颜色分量所占用的位数分别为5Bits,6Bits,5Bits。
//在18位彩色模式下为0表示"6Bits in Low Byte"，为1表示"6Bits in High Byte"。
//在24位彩色和32位彩色模式下is565无效。	  
//rgb: 描述R G B颜色分量的排列顺序，rgb中每2Bits表示一种颜色分量，[00]表示空白，[01]表示Red，[10]表示Green，[11]表示Blue。

void image_display(u16 x,u16 y,u8 * imgx);//在指定位置显示图片
void image_show(u16 xsta,u16 ysta,u16 xend,u16 yend,u8 scan,u8 *p);//在指定区域开始显示图片
u16 image_getcolor(u8 mode,u8 *str);//获取颜色

#endif

//#include "sys.h"
//#include "usart.h"		
//#include "delay.h"	
//#include "led.h"   
//#include "lcd.h" 
//#include "key.h"
//#include "image2lcd.h"
//#include "stdlib.h"

// 
//extern const u8 gImage_test[];//图片提取出来的颜色值
//extern const u8 gImage_test_gif[];//测试gif格式图片，不行
//extern const u8 gImage_red[];

//int main(void)
//{        
//	u8 i=0;
//	int key_status=-1;
//  HEADCOLOR *imginfo;
//	u16 x=0,y=0;
//	u16 x0,y0;
// 	imginfo=(HEADCOLOR*)gImage_test;	//
//	
//	Systick_init(168);
//	
//	UART1_init(115200);	
//	LED_init();			
// 	LCD_init();	
//	KEY_init();
// 	if(imginfo->w>=240||imginfo->h>320)
//	{
//		printf("w = %d h =%d \n",imginfo->w,imginfo->h);  	
//		delay_ms(200);
//		printf("The Picture is too large");	
//		delay_ms(200);
//	} 
//	image_display(0,0,(u8*)gImage_test);
// 	while(1)
//	{	
//		LED0=!LED0;					 
//		delay_ms(500);	
//		key_status=key_scanf(0);
//		if(key_status==1)
//		{
//			imginfo=(HEADCOLOR*)gImage_red;
//			image_display(20,0,(u8*)gImage_red);
//			printf("change color test \n");
//		}
//		if(key_status==2)
//		{
//			imginfo=(HEADCOLOR*)gImage_test;
//			image_display(0,0,(u8*)gImage_test);
//			printf("original\n");
//		}

//			
//	} 
//}











