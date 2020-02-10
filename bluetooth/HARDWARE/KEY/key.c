#include "key.h"
/*
	key0 1 2 3 对应了 PF 9 8 7 6 

*/

void KEY_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	//按键PF9 8 7 6 的初始化
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9 | GPIO_Pin_8 | GPIO_Pin_7 | GPIO_Pin_6;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
	
}

int key_scanf(int mode)
{
	
	static int key_mode=1;//1支持长按 0 不支持 相当于是一个标志位
	if(mode) key_mode=1;
	if(key_mode&&((key0==0)||(key1==0)||(key2==0)||(key3==0)))
	{
		delay_ms(10);
		if(key0==0)
		{
			key_mode=0;
			return 1;
			
		}else if(key1==0)
		{
			key_mode=0;
			return 2;
			
		}else if(key2==0)
		{
			key_mode=0;
			return 3;
		}else if(key3==0)
		{
			key_mode=0;
			return 4;
		}
		
	//没有按键时，要置位一下，才能按第一次	
	}else if((key0==1)&&(key1==1)&&(key2==1)&&(key3==1))
	{
		key_mode=1;
	}
	return 0;
}

/****************************************************************************
* 名    称: void key_scan(u8 mode)
* 功    能：按键扫描函数-----功能更强大点
* 入口参数：mode：0：单按 
                  1: 连按
* 返回参数：无
* 说    明：响应优先级,KEY0>KEY1>KEY2>KEY3
****************************************************************************/
u8  keydown_data=0x00;    //按键按下后就返回的值
u8  keyup_data=0x00;      //按键抬起返回值
u16  key_time=0x00;       //按键按下之后的时间计数，该值乘以扫描一次按键函数的时间就等于按键按下的时间

u8  key_tem=0x00;         //长按的按键值与按键扫描程序过渡变量
u8  key_bak=0x00;         //按键扫描程序过渡变量

void key_scan_advanced(u8 mode)
{	   
	 keyup_data=0;         //键抬起后按键值一次有效
	
	if(key0==0||key1==0||key2==0||key3==0)   //有键正按下
	{
		if(key0==0)      key_tem=1;
		else if(key1==0) key_tem=2;
		else if(key2==0) key_tem=3;
		else if(key3==0) key_tem=4;
		
		if (key_tem == key_bak)      //有键按下后第一次扫描不处理，与else配合第二次扫描有效，这样实现了去抖动
		{
			 key_time++;             //有键按下后执行一次扫描函数，该变量加1
			 keydown_data=key_tem;   //按键值赋予keydown_data
			
			 if( (mode==0)&&(key_time>1) )//key_time>1按键值无效，这就是单按，如果mode为1就为连按
					keydown_data=0;
		}
		else                             //去抖动      
		{
			 key_time=0;
			 key_bak=key_tem;
		}
	}
	else       //键抬起
	{
		 if(key_time>2)            //按键抬起后返回一次按键值
			{
				keyup_data=key_tem;  //键抬起后按键值赋予keydown_data            						
			}
			key_bak=0;               //要清零，不然下次执行扫描程序时按键的值跟上次按的值一样，就没有去抖动处理了
			key_time=0;
			keydown_data=0;		
	}    
}