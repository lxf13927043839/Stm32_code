#ifndef __IMAGE2LCD_H
#define __IMAGE2LCD_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//FLASHͼƬ��ʾ		   
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
û��__packed�Ļ�����ֶ����Ҳ���ǣ�char�͵�������ռ��4���ֽڵ�
���ȵ��ڴ�ռ�
��packed�ľͲ���,�Ϳ϶���1���ֽڵ��ڴ�ռ�
__packedֻ��ĳ�ֱ������ĸ�ʽѹ�����е���pack,�Բ�ͬ��cpuѹ���Ķ���
��ʽҲ�ǲ�һ��
		 
*/

//4096ɫ/16λ���ɫ/18λ���ɫ/24λ���ɫ/32λ���ɫ
//ͼ������ͷ�ṹ��  
__packed typedef struct _HEADCOLOR
{
   unsigned char scan;
   unsigned char gray;
   unsigned short w;
   unsigned short h;
   unsigned char is565;
   unsigned char rgb;
}HEADCOLOR;  
//scan: ɨ��ģʽ 	    
//Bit7: 0:��������ɨ�裬1:��������ɨ�衣 
//Bit6: 0:�Զ�����ɨ�裬1:�Ե�����ɨ�衣 
//Bit5: 0:�ֽ����������ݴӸ�λ����λ���У�1:�ֽ����������ݴӵ�λ����λ���С� 
//Bit4: 0:WORD���͸ߵ�λ�ֽ�˳����PC��ͬ��1:WORD���͸ߵ�λ�ֽ�˳����PC�෴�� 
//Bit3~2: ������ 
//Bit1~0: [00]ˮƽɨ�裬[01]��ֱɨ�裬[10]����ˮƽ,�ֽڴ�ֱ��[11]���ݴ�ֱ,�ֽ�ˮƽ�� 
//gray: �Ҷ�ֵ 
//   �Ҷ�ֵ��1:��ɫ��2:�Ļң�4:ʮ���ң�8:256ɫ��12:4096ɫ��16:16λ��ɫ��24:24λ��ɫ��32:32λ��ɫ��
//w: ͼ��Ŀ�ȡ�	 
//h: ͼ��ĸ߶ȡ�
//is565: ��4096ɫģʽ��Ϊ0��ʾʹ��[16bits(WORD)]��ʽ����ʱͼ��������ÿ��WORD��ʾһ�����أ�Ϊ1��ʾʹ��[12bits(�����ֽ���)]��ʽ����ʱ�������е�ÿ12Bits����һ�����ء�
//��16λ��ɫģʽ��Ϊ0��ʾR G B��ɫ������ռ�õ�λ����Ϊ5Bits��Ϊ1��ʾR G B��ɫ������ռ�õ�λ���ֱ�Ϊ5Bits,6Bits,5Bits��
//��18λ��ɫģʽ��Ϊ0��ʾ"6Bits in Low Byte"��Ϊ1��ʾ"6Bits in High Byte"��
//��24λ��ɫ��32λ��ɫģʽ��is565��Ч��	  
//rgb: ����R G B��ɫ����������˳��rgb��ÿ2Bits��ʾһ����ɫ������[00]��ʾ�հף�[01]��ʾRed��[10]��ʾGreen��[11]��ʾBlue��

void image_display(u16 x,u16 y,u8 * imgx);//��ָ��λ����ʾͼƬ
void image_show(u16 xsta,u16 ysta,u16 xend,u16 yend,u8 scan,u8 *p);//��ָ������ʼ��ʾͼƬ
u16 image_getcolor(u8 mode,u8 *str);//��ȡ��ɫ

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
//extern const u8 gImage_test[];//ͼƬ��ȡ��������ɫֵ
//extern const u8 gImage_test_gif[];//����gif��ʽͼƬ������
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











