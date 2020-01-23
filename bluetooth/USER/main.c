#include "stm32f4xx.h" 
#include "Delay.h" 
#include "led.h" 
#include "key.h" 
#include "beep.h" 
#include "exti.h"
#include "usart.h"
#include "stdio.h"
#include "adc.h"
#include "dht11.h"
#include "tim3.h"
#include "iwdg.h"
#include "rtc.h"
#include "pwm.h"
#include "24cxx.h"
#include "myiic.h"
#include "esp8266_uart4.h"
#include "string.h"
#include "lcd.h"


int main()
{
	int key_id=-1;
	
	LED_init();
	Systick_init(168);  //初始化延时函数，没有初始化会导致程序卡死
	
	UART1_init(115200);
	KEY_init();
	
 	LCD_init();           //初始化LCD FSMC接口和显示驱动
	BRUSH_COLOR=RED;      //设置画笔颜色为红色
	
	LCD_DisplayString(10,10,24,"Illuminati STM32F4");	
	LCD_DisplayString(20,40,16,"Author:Clever");
	LCD_DisplayString(30,80,24,"4.TFTLCD TEST");
	LCD_DisplayString(50,110,16,"KEY0:Screen Test");	
	LCD_DisplayString(50,130,16,"KEY1:Number Test");	
	 
	
	LCD_DisplayChinese_one(120 ,200 ,'3', 24);
	
	
	//LCD_DisplayChar(120,200,1,144);						      //显示一个字符
	
	printf("start \n");
	
	
	while(1)
	{
			key_id=key_scanf(0);
			switch(key_id)
			{
				case 1: 
					LCD_Clear(BLACK);
					break;
				case 2:
					LCD_Clear(BLUE);
					break;
				case 3:
					LCD_Clear(RED);
					break;
				case 4:
					LCD_Clear(MAGENTA);
					break;
				
			}
		
	}

}
