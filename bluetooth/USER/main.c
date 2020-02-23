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
#include "people.h"
#include "touch.h"
#include "xpt2046.h"
#include "image2lcd.h"

//==========================================================
u8 light_streng_now=0;
u8 light_streng_set=0;
//========================================================

//还没有添加定时上传服务器功能

int label_mode;//用来窗帘模式远程控制，标签
int label_closetime;//远程关闭定时设置界面

void open_rotate(void);//电机转动打开窗帘
void close_rotate(void);
void LCD_showdate(void);//刷新lcd的数据界面，在TIM3的十秒到的时候


u8 enter_set_time=0;//进入设置定时界面

u8 has_set_time_online=0;
u8 set_time_buff[18];
//---------------------------------------------------------------------------
u8 beep_status; //蜂鸣器状态位 1：开 0：关

u8 curtain_status;//0:窗帘关闭

//---------------------------------------------------------------------------
void delay_ms(u16 nms);
//---------------------显示单个汉字与汉字字符串--------------------------
void LCD_DisplayChinese_one(u16 x,u16 y,u8 word,u8 size);
void LCD_DisplayChinese_string(u16 x,u16 y,u8 size,int *p);

void LCD_DisplayChar(u16 x,u16 y,u8 word,u8 size); //显示一个字符
void LCD_DisplayString(u16 x,u16 y,u8 size,u8 *p); //显示一个12/16/24字体字符串

void get_all_status_data(char *data_buff);
//---------------------ESP8266--115200--------------------------------------
//void front_rotate(void); //电机转动要把电池的电量考虑进去
//void area_rotate(void);
//void stop_rotate(void);

//void get_all_status_data(char *data_buff);
/*
		功能：与tlink对接的数据协议
		
		温度、湿度、光照、光照阈值、窗帘状态、智能模式开关，窗帘开关，夜晚蜂鸣器警报，窗帘定时功能，获取最新数据    夜晚蜂鸣器警报
		 12----45----78-----10 11-----13----------15-----------17-----------19------------21-------------23 ---------25----------------
*/
#define SIZE_of_DATA 28
unsigned char data_buff[SIZE_of_DATA]="#11,22,33,44,0,0,0,0,0,0,0#";
            

#define SIZE_from_SEVRVER 50
char data_fromserver[SIZE_from_SEVRVER];//接收服务器的字符串控制指令
int num_from_server=0; //接收字符数 序号
int link_success=0; //初始化esp8266模块时，看指令是否成功执行
int link_flag=0; //在连接过程中检测心跳包，在tim4中断函数中控制看门狗，

int first_ok_flag=0; //用来判断第一次的ok设置定时器的值
int ok_flag=0;	//用来反应心跳包的
unsigned char rece_status=0;//接收服务器控制指令


//串口4 的中断服务程序 ====esp8266串口4 从手机端接收的信息
void UART4_IRQHandler(void)
{
	int i=0;
	uint16_t data;
	u8 temp=5;
	//把收到的来自esp8266 模块的数据，通过USART1发给PC
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		data = USART_ReceiveData(UART4); //读数据
	
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
		USART_SendData(USART1, data);
		
		if(link_success==0)  //判断wifi是否成功连接到服务器上，连接不上----看门狗重启
		{
			if(data=='>')
			{
				LED2=0;
				link_success=1;
				feed_dog();
				TIM4_init(4*10000-1,8400-1); //定时器跟看门狗时间同步
				//TIM_Cmd(TIM3,DISABLE);
				TIM3_init(10000-1,8400-1);//定时1秒，用来定时上传数据以及扫描LCD屏幕
				TIM3->CNT=0;
				
				link_flag=1;//连接成功
				
				
			}
		}else if(link_success==1)  //对tlink下行控制指令、以及心跳包的处理
		{
			switch(rece_status)
			{
				case 0://0,1是对控制指令的处理
							if(data=='{')
							{
								LED0=!LED0;
								rece_status++;
							}
							else
							{
								//这里可进行优化
								data_fromserver[num_from_server]=data;
								num_from_server++;
								if(data_fromserver[num_from_server-1]=='k')
								{
									data_fromserver[num_from_server]='\0';
									if(strcmp(data_fromserver,"ok")==0)
									{
										num_from_server=0;
										data_fromserver[0]=0;
										data_fromserver[1]=0;
										data_fromserver[2]=0;
										
										if(first_ok_flag==0)
										{
											first_ok_flag=1;
											TIM2_init(14*10000-1,8400-1);//重新定时器TIM2-----14秒，跟心跳包同
											//printf("first ok is come\n");
										}
										else
										{
											//printf("重新设置计时\n");							
											TIM2->CNT=0;
										}
										
									}
								}
							}
							break;
				case 1:
							data_fromserver[num_from_server]=data;
							num_from_server++;
							if(data_fromserver[num_from_server-1]=='}')
							{
								data_fromserver[num_from_server-1]='\0';
								LED1=!LED1;
								//对命令的处理
								
								printf("order from server = %s=%d \n",data_fromserver,strlen(data_fromserver));
								
								
								
								if(strcmp(data_fromserver,"curt_on")==0)//窗帘开
								{
									AT24CXX_Read(3,(u8 *)&temp,1);//在手动模式下才能控制 , 打漏了取地址的符号
									
									if(temp==0)
									{
								
										if(curtain_status==0)
										{
											curtain_status=1;
											LCD_DisplayChinese_one(140,143,24,24);
//===========================================================================================											
											//add 电机控制
											open_rotate();
											curtain_status=1;
											printf(" open curtain from server\n");
										}
										
									}
									
									
									
								}
								else if(strcmp(data_fromserver,"curt_off")==0)//窗帘关
								{
									AT24CXX_Read(3,(u8 *)&temp,1);
									if(temp==0)
									{	
										if(curtain_status==1)
										{
											curtain_status=0;
											LCD_DisplayChinese_one(140,143,25,24);
//===========================================================================================
											close_rotate();
											printf("close curtain from server\n");
										}
								
										//add 电机控制
									}
								
									
								}else if(strcmp(data_fromserver,"smart_on")==0)//开智能模式
								{
									//先判断现在是处于什么模式，如果是手动模式在执行
									AT24CXX_Read(3,(u8 *)&temp,1);
									if(temp==0)
									{
										label_mode=1;
									}
									
								}
								else if(strcmp(data_fromserver,"smart_off")==0)//关智能模式
								{
									AT24CXX_Read(3,(u8 *)&temp,1);
									if(temp==1)
									{
										label_mode=1;
									}
									
								}
								else if(strcmp(data_fromserver,"setti_on ")==0)//设置定时开
								{
									//开不了啥都不用做，空着
								}
								else if(strcmp(data_fromserver,"setti_off")==0)//设置定时关
								{
									temp=4;
									AT24CXX_Write(4,(u8*)&temp,1);
									
									if(enter_set_time==1)
									{
										label_closetime=1;
									}
									
								}
								else if(strcmp(data_fromserver,"alarm_on")==0)//开夜晚蜂鸣器警报
								{
									LCD_DisplayChinese_one(205,264,24,24);
									beep_status=1;
									AT24CXX_Write(2,&beep_status,1);
								}
								else if(strcmp(data_fromserver,"alarm_off")==0)//关夜晚蜂鸣器警报
								{
									LCD_DisplayChinese_one(205,264,25,24);
									beep_status=0;
									AT24CXX_Write(2,&beep_status,1);
								}
								else if(strcmp(data_fromserver,"update_on")==0)//获取最新数据
								{
									get_all_status_data(data_buff);
									SENDstr_to_server(data_buff);
									printf("update_on successful\n");
								}
								
								//##################
								for(i=0;i<SIZE_from_SEVRVER;i++)
								{
									data_fromserver[i]=0;
								}
								num_from_server=0;
								rece_status=0;
							}
							break;
				
			}
		}
		
		USART_ClearITPendingBit(UART4, USART_IT_RXNE); //清除UART4的接收中断	
	}

} 


/*
	定时器3中断服务函数
	//定时器时钟是84M 
	//分频系数是8400，所以 84M/8400=10KHZ的计数频率，计数5000次为500ms
  // 1/10 000 * 5000 = 0.5s = 500ms
  TIM3_init(5000-1,8400-1);

	------------------------看门狗----------------------------
  //时间计算：((4*2^4))/32000=0.2ms 计数一次0.2ms 500次就是1s
	IWDG_init(4,1000);

*/

int opendog_flag=-1;//用来计数10s后打开看门狗，一开始初始化定时器，会自动进一次中断----无解
int initdog=0;
int ten_second_send_flag=0;//十秒定时上传数据
/*
	在配置esp8266以及连接服务器，配置是5秒
	在连接成功的时候，更改为1秒，用来定时上传数据 
*/
void TIM3_IRQHandler(void) 
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		if(initdog==0)
		{
			opendog_flag=opendog_flag+1;    
			//printf("opendog_flag = %d\n",opendog_flag);
		}
		if(opendog_flag==2)
		{
			IWDG_init(4,500*5);
			initdog=1;
			opendog_flag=0;
			//printf("dog successful \n");
		}
		if(link_flag==1) //定时10秒上传数据
		{
			ten_second_send_flag++;
			if(ten_second_send_flag==10)
			{
				ten_second_send_flag=0;
				get_all_status_data((char *)data_buff);
				//printf("ten second send \n");
				SENDstr_to_server((char *)data_buff);
				//顺便刷新LCD的数据,在进入定时界面的时候不能去刷
				if(enter_set_time==0)
				LCD_showdate();
			}
			
		}
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //清除中断标志位
}

int tim2flag=-1;
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
	{	
			if(tim2flag==-1)
			{
				tim2flag=1;
			}
			else
			{
					printf("检测失败 启动看门狗\n");
					SENDstr_to_server("+++");	
					link_flag=0;//不进行喂狗操作
					
			}

	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update); //清除中断标志位
}

/*
	功能：当出现 > 符号即连接成功，TIM4一直是在喂狗
				看门狗是5秒-----TIM4是4秒，因为看门狗开启后无法关闭，所有必须喂狗
*/
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //溢出中断
	{	
		if(link_flag==1)	
		{
			feed_dog();
			//printf("in TIM4 feeddog\n");
		}
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update); //清除中断标志位
}



//按键3 的中断处理函数
void EXTI9_5_IRQHandler(void)
{
	//key3
	if(RESET != EXTI_GetITStatus(EXTI_Line6))
	{
		delay_ms(10);	//去抖动
		if(key3==0)	 
		{
			//front_rotate();
		  printf("key3 test by interrupt\n");
		}		
		//清楚中断标志位，否则会一直进入中断函数
		EXTI_ClearITPendingBit(EXTI_Line6);
	}
	
	//key2
	if(RESET != EXTI_GetITStatus(EXTI_Line7))
	{
		delay_ms(10);	//去抖动
		if(key2==0)	 
		{
			
			//area_rotate();
			
			printf("\nkey2 test by interrupt\n");
		}		
		//清楚中断标志位，否则会一直进入中断函数
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
	//key1
	
	if(RESET != EXTI_GetITStatus(EXTI_Line8))
	{
		delay_ms(10);	//去抖动
		if(key1==0)	 
		{	
			
			//stop_rotate();
			
			printf("\nkey1 test by interrupt\n");
		}		
		//清楚中断标志位，否则会一直进入中断函数
		EXTI_ClearITPendingBit(EXTI_Line8);
	}
	
}

////-----------电机控制初始化---------

#define ENA PBout(10)	
#define IN1 PBout(11)	// D1	 
#define IN2 PCout(6)	// D1	 
void ELECTRI_motor_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//开启GPIO时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	//初始化IO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOB11  GPIOB10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOB,&GPIO_InitStructure); //PB10, PB11
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //GPIOC6
	GPIO_Init(GPIOC,&GPIO_InitStructure); //PC6
	
	GPIO_ResetBits(GPIOC,GPIO_Pin_6);
	GPIO_ResetBits(GPIOB,GPIO_Pin_10 | GPIO_Pin_11);
}

void open_rotate(void)//电机转动打开窗帘
{
	ENA=1;
	IN1=1;
	IN2=0;
	//判断条件
	
	
	printf("electrical motor open successfully\n");
}
void close_rotate(void)
{
	ENA=1;
	IN1=0;
	IN2=1;
	//判断条件
	
	printf("electrical motor close successfully\n");
}
void stop_rotate(void)
{
	ENA=1;
	IN1=0;
	IN2=0;
}

////初始化dht11 光敏、RTC
//void ALL_SENSOR_init(void)
//{
//	//****************************************//
//  //如果没有dht11 灯会闪烁
//	while(DHT11_init())
//	{
//		LED1=!LED1;
//	}
   
//	//RTC
//	My_RTC_init();
//	//在这里设置选择了ck_spre的时钟频率，即1HZ的频率
//	//所有给0 会产生1S
//	//可看寄存器的配置,10x是选择了ck_spre的时钟频率
//	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);//WAKE UP
//	
//	ELECTRI_motor_init();
//	
//	People_init();
//	
//}

///*
//	功能：获取各个传感器数据，以及控制状态值
//	
//*/



//获取dht11 光敏、
void get_all_status_data(char *data_buff)
{
	u8 temperature=0;//温度
  u8 humidity=0;   //湿度
	u16 adcx=0;		 //adc读取值  12位精度adc  2^12=4096
	int light_streng=0;
	u8 light_limit=0;//光照阈值

	u8 rtc_buf[40];
	u8 mode_of_system=5;//系统控制的模式
	u8 beep_flag=3;
	u8 time_temp=3;
	
	//没有接传感器，或者没有调用对应的初始化函数会导致出错，使得tlink，那边卡住，然后重启
	
	adcx=Get_Adc_Average(ADC_Channel_9,20);//获取通道9的转换值，20次取平均
	light_streng=(4096-adcx)*100/4096;          //获取计算后的带小数的实际电压值，比如3.1111 变化值太小，不好测量。换成百分比
	//printf("光敏:light_streng = %d\n",light_streng);
	
	//DHT11_Read_Data(&temperature,&humidity);
	//printf("温度temperature = %d \n",temperature);
	//printf("湿度humidity    = %d \n",humidity);
	
	data_buff[1]=(temperature/10)+48;
	data_buff[2]=(temperature%10)+48;
	
	data_buff[4]=(humidity/10)+48;
	data_buff[5]=(humidity%10)+48;
	
	data_buff[7]=(light_streng/10)+48;
	data_buff[8]=(light_streng%10)+48;
	
	light_streng_now=light_streng;
	printf("light_streng_now = %d\n",light_streng_now);
	
	//光照阈值----------后面再进行优化，先实现功能
	
	
	//----------------------------------------------------------要根据到时测试的值进行修改
	AT24CXX_Read(1,&light_limit,1);
	switch(light_limit)
	{
		case 1:
						data_buff[10]=4+48;
						data_buff[11]=0+48;
						break;
		case 2:
						data_buff[10]=6+48;
						data_buff[11]=0+48;
						break;
			break;
		case 3:
						data_buff[10]=9+48;
						data_buff[11]=0+48;
						break;
			break;
	}
	light_streng_set=(data_buff[10]-48)*10+data_buff[11]-48;
	printf("light_streng_set = %d \n",light_streng_set);
	
	
	//窗帘的状态----窗帘开关------是统一的
	if(curtain_status==1)
	{
		data_buff[13]=1+48;
		data_buff[17]=1+48;
	}
	else if(curtain_status==0)
	{
		data_buff[13]=0+48;
		data_buff[17]=0+48;
	}
	
	//智能模式开关
	AT24CXX_Read(3,&mode_of_system,1);
	if(mode_of_system==1)
	{
		data_buff[15]=1+48;
	}
	else
	{
		data_buff[15]=0+48;
	}
	
	//蜂鸣器警报
	AT24CXX_Read(2,&beep_flag,1);
	if(beep_flag==1)
	{
		data_buff[19]=1+48;
	}
	else
	{
		data_buff[19]=0+48;
	}
	//窗帘定时功能
	AT24CXX_Read(4,&time_temp,1);
	if(time_temp==1||time_temp==0)
	{
		data_buff[21]=1+48;
	}
	else
	{
		data_buff[21]=0+48;
	}
	
	data_buff[23]=0+48;
	//		温度、湿度、光照、光照阈值、窗帘状态、智能模式开关，窗帘开关，夜晚蜂鸣器警报，窗帘定时功能，获取最新数据
	//	 12----45----78-----10 11-----13----------15-----------17-----------19------------21-------------23 
	
	printf("所采集的数据data = %s \n",data_buff);
}

// /*------------------------------------------------------------------- 
//	1、汉字大小：24*24共 72字节
//  2、显示汉字时调用对应序号：
//	温(0) 湿(1) 度(2) 光(3) 照(4) 强(5) 度(6) 窗(7) 帘(8) 状(9) 态(10) 智(11) 能(12) 手(13) 动(14) 模(15)
//       式(16) 阈（17） 值（18） 摄氏度（19） 百分号（20） 弱（21） 中（22） 强（23） 开（24） 关（25）
// */

extern u8 Zero_to_six_clock;
void LCD_showdate(void)
{
	int x=0,y=0;
	int level;
	u8 temp[2];
	RTC_DateTypeDef RTC_DateStruct;
	u8 temp_buff[40]; 
	
	BRUSH_COLOR=RED;      //设置画笔颜色为黑色
	//温度
	x=95,y=47;
	temp[0]=data_buff[1];
	temp[1]=data_buff[2];
	LCD_DisplayString(x,y,24,temp); //显示一个24字体字符串，宽只占了12,字符显示时候
	LCD_DisplayChinese_one(x+12*2,y,19,24);
	
	//湿度
	x=95;y=80;
	temp[0]=data_buff[4];
	temp[1]=data_buff[5];
	LCD_DisplayString(x,y,24,temp); //显示一个12/16/24字体字符串
	LCD_DisplayChinese_one(x+12*2,y,20,24);

	//光照强度----采用弱、中、强来表示，数字意义不大
	x=95;y=113;
	temp[0]=data_buff[7];
	temp[1]=data_buff[8];
	LCD_DisplayString(x,y,24,temp); //显示一个12/16/24字体字符串
	LCD_DisplayChinese_one(x+12*2,y,20,24);
	LCD_DisplayChar(x+12*2+24*1,y,'(',24);
	level=(temp[0]-48)*10+(temp[1]-48);
	if(level>=0&&level<=40)
	{
		LCD_DisplayChinese_one(x+12*2+24*2-12,y,21,24);
	}
	else if(level>40&&level<=70)
	{
		LCD_DisplayChinese_one(x+12*2+24*2-12,y,22,24);
	}
	else if(level>70&&level<=100)
	{
		LCD_DisplayChinese_one(x+12*2+24*2-12,y,23,24);
	}
	LCD_DisplayChar(x+12*2+24*3-12,y,')',24);
		
	//显示日期
	x=5;y=295;
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
	sprintf((char*)temp_buff,"20%02d-%02d-%02d",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
	//printf("%s\n",temp_buff);
	LCD_DisplayString(x,y,24,temp_buff); //显示一个12/16/24字体字符串
	
	//显示时间测试位置,显示ok---该函数放在秒的唤醒服务函数
	
}


////跳转到设置光照阈值的界面

extern const u8 gImage_set_light[];//设置光照阈值界面
extern const u8 gImage_set_time[]; //设置定时
extern const u8 gImage_smart_mode[];
extern const u8 gImage_hand_mode[];

/*
	功能：定时窗帘的开或者关---注意：把提示换成中文的

*/
/*

* 说    明：长按按KEY3进行时间调整，按KEY0进行数值的加1，按KEY1数值进行减1，  
            按KEY2右移选择要设置的选项，在调整完后短按KEY3，确认调整时间         
*********************************************************************************/

u8 const *set_option[7]={"Year","Month","Day","Hours","Minutes","Seconds"," "};

void Time_Display()//把变化的时间写入
{
 LCD_DisplayString(30,220,24,"Date:20  -  -  ");
 LCD_DisplayNum(114,220,RTC_DateStruct.RTC_Year,2,24,1);
 LCD_DisplayNum(150,220,RTC_DateStruct.RTC_Month,2,24,1);
 LCD_DisplayNum(186,220,RTC_DateStruct.RTC_Date,2,24,1);
 LCD_DisplayString(30,250,24,"Time:  :  :  ");
 LCD_DisplayNum(90,250,RTC_TimeStruct.RTC_Hours,2,24,1);	
 LCD_DisplayNum(126,250,RTC_TimeStruct.RTC_Minutes,2,24,1);
 LCD_DisplayNum(162,250,RTC_TimeStruct.RTC_Seconds,2,24,1);
 //LCD_DisplayString(30,260,16,"Week:        ");
 //LCD_DisplayString(70,260,16,(u8 *)weekdate[RTC_DateStruct.RTC_WeekDay-1]);
}
void Time_has_set()//显示定时的时间
{
 LCD_DisplayString(70,57,24,"20  -  -  ");
 LCD_DisplayNum(94,57,RTC_DateStruct.RTC_Year,2,24,1);
 LCD_DisplayNum(130,57,RTC_DateStruct.RTC_Month,2,24,1);
 LCD_DisplayNum(166,57,RTC_DateStruct.RTC_Date,2,24,1);
 LCD_DisplayString(70,81,24,"  :  :  ");
 LCD_DisplayNum(70,81,RTC_TimeStruct.RTC_Hours,2,24,1);	
 LCD_DisplayNum(106,81,RTC_TimeStruct.RTC_Minutes,2,24,1);
 LCD_DisplayNum(142,81,RTC_TimeStruct.RTC_Seconds,2,24,1);
}

void ADJUST_time(u8 option,u8 shanshuo)
{
		if(shanshuo%50==0)  //提示现在要设置的选项
		{
		  LCD_DisplayString_color(30,280,24,"Please Set          ",BLUE,WHITE);
		  LCD_DisplayString_color(162,280,24,(u8 *)set_option[option],BLUE,WHITE);
		}
		
		//设置时数值的加减1
		if( (keydown_data==KEY0_DATA)||(keydown_data==KEY1_DATA))
		{
		  if(keydown_data==KEY0_DATA)
			{
			  switch(option)
				{
				  case 0: RTC_DateStruct.RTC_Year+=1;   break;
					case 1:
								if(RTC_DateStruct.RTC_Month==12)
								{
									RTC_DateStruct.RTC_Month=1;
								}
								else
								RTC_DateStruct.RTC_Month+=1;  
								break;
					case 2: 
								if(RTC_DateStruct.RTC_Date==28&&RTC_DateStruct.RTC_Month==2&&RTC_DateStruct.RTC_Year%4!=0)
								{
									RTC_DateStruct.RTC_Date=1;
								}
								else if(RTC_DateStruct.RTC_Date==29&&RTC_DateStruct.RTC_Month==2&&RTC_DateStruct.RTC_Year%4==0)
								{
									RTC_DateStruct.RTC_Date=1;
								}
								else if(RTC_DateStruct.RTC_Date==30)
								{
									switch(RTC_DateStruct.RTC_Month)
									{
										case 4:
										case 6:
										case 9:
										case 11:
														RTC_DateStruct.RTC_Date=1;
														break;
										case 1:
										case 3:
										case 5:
										case 7:
										case 8:
										case 10:
										case 12:
														RTC_DateStruct.RTC_Date+=1;
														break;
									}
								}
								else if(RTC_DateStruct.RTC_Date==31)
								{
									RTC_DateStruct.RTC_Date=1;
								}
								else
								RTC_DateStruct.RTC_Date+=1;   
								break;
					case 3: 
								if(RTC_TimeStruct.RTC_Hours==23)
								{
									RTC_TimeStruct.RTC_Hours=0;
								}
								else
								RTC_TimeStruct.RTC_Hours+=1;  
								break;
					case 4: 
								if(RTC_TimeStruct.RTC_Minutes==59)
								{
									RTC_TimeStruct.RTC_Minutes=0;
								}
								else
								RTC_TimeStruct.RTC_Minutes+=1;
								break;
					case 5: 
								if(RTC_TimeStruct.RTC_Seconds==59)
								{
									RTC_TimeStruct.RTC_Seconds=0;
								}
								else
								RTC_TimeStruct.RTC_Seconds+=1;
								break;
				}	
			}
			if(keydown_data==KEY1_DATA)
			{
			  switch(option)
				{
				  case 0: RTC_DateStruct.RTC_Year-=1; break;
					case 1: 
								if(RTC_DateStruct.RTC_Month==1)
								{
									RTC_DateStruct.RTC_Month=12;
								}
								else
								RTC_DateStruct.RTC_Month-=1;
								break;
					case 2: 
								if(RTC_DateStruct.RTC_Date==1)
								{
									if((RTC_DateStruct.RTC_Year)%4==0&&(RTC_DateStruct.RTC_Month)==2)
									{
										RTC_DateStruct.RTC_Date=29;
									}
									else
									{
										if((RTC_DateStruct.RTC_Month)==2)
										{
											RTC_DateStruct.RTC_Date=28;
										}
										else
										{
											switch((RTC_DateStruct.RTC_Month))
											{
												case 1:
												case 3:
												case 5:
												case 7:
												case 8:
												case 10:
												case 12:
																RTC_DateStruct.RTC_Date=31;
												break;
												
												case 4:
												case 6:
												case 9:
												case 11:
																RTC_DateStruct.RTC_Date=30;
												break;
											}
										}
									}
								}
								else								
								RTC_DateStruct.RTC_Date-=1;//考虑到
							
								break;
					case 3: 
								if(RTC_TimeStruct.RTC_Hours==0)
								{
									RTC_TimeStruct.RTC_Hours=23;
								}
								else
								RTC_TimeStruct.RTC_Hours-=1;
							
								break;
					case 4: 
								if(RTC_TimeStruct.RTC_Hours==0)
								{
									RTC_TimeStruct.RTC_Minutes=59;
								}
								else
								RTC_TimeStruct.RTC_Minutes-=1;
								break;
					case 5: 
								if(RTC_TimeStruct.RTC_Seconds==0)
								{
									RTC_TimeStruct.RTC_Seconds=59;
								}
								else
								RTC_TimeStruct.RTC_Seconds-=1;
								break;
				}	
			}
		}
		//闪烁显示：显示
	if(shanshuo==150)
	{
			Time_Display();
//			LCD_DisplayNum(86,220,RTC_DateStruct.RTC_Year,2,16,1);
//			LCD_DisplayNum(110,220,RTC_DateStruct.RTC_Month,2,16,1);
//			LCD_DisplayNum(134,220,RTC_DateStruct.RTC_Date,2,16,1);
//			LCD_DisplayNum(70,240,RTC_TimeStruct.RTC_Hours,2,16,1);
//			LCD_DisplayNum(94,240,RTC_TimeStruct.RTC_Minutes,2,16,1);
//			LCD_DisplayNum(118,240,RTC_TimeStruct.RTC_Seconds,2,16,1);
	}	
		//闪烁显示：不显示
	switch(option)
	{  
		case 0: {  if(shanshuo==49)  LCD_DisplayString(114,220,24,"  ");	 break;  }
		case 1: {  if(shanshuo==49)  LCD_DisplayString(150,220,24,"  "); break;  }
		case 2: {  if(shanshuo==49)  LCD_DisplayString(186,220,24,"  "); break;  }
		case 3: {  if(shanshuo==49)  LCD_DisplayString(90,250,24,"  ");  break;  }
		case 4: {  if(shanshuo==49)	 LCD_DisplayString(126,250,24,"  ");	 break;	 }
		case 5: {  if(shanshuo==49)	 LCD_DisplayString(162,250,24,"  "); break;  }		
	}
}	


void SET_time(void)
{
	//注：有一些函数是初始化过了的，整合时候要删除
	
	u8 process=0;  //程序流程走向
	u8 option=6;     //调整时间选项
	u8 t=0,j;      //计时参数
	u8 time_set_flag=0;//是否设置了按键的定时标志
	u8 now_date_buff[40];//当前的日期
	u8 now_time_buff[40];//当前的时间

	u8 time_buff[40]={0};//存放定时的时间,没有初始化会出现一些意外的bug，如出现一斜杆
	u8 date_buff[40];//存放定时的日期
	u8 test[40];//从24c02 读取数据测试
	u8 curtain_status_insettime=3;//窗帘状态,只有0/1 状态是有效的，其他数字是无效的
	short set_tim_success=-1; //时间的设置是正常的 
	u8 had_settimeflag=3; //检测是否有设置了
	u8 temp;
	u8 set_curtain_status;//定时设置窗帘的开关，不是直接反应窗帘状态
	
	BRUSH_COLOR=RED;
	
	//待删除部分
	/*
	LCD_DisplayString(10,10,24,"Illuminati STM32F4");	
	LCD_DisplayString(10,40,16,"Author:Clever");
	LCD_DisplayString(30,70,24,"14.RTC TEST");
	LCD_DisplayString(30,100,16,"KEY3 Long :Adjust time");
	LCD_DisplayString(30,120,16,"     Short:Adjust OK");
	LCD_DisplayString(30,140,16,"KEY0: V++  KEY1: V--");
	LCD_DisplayString(30,160,16,"KEY2: Right Move");
	*/
	RTC_GetTimes(RTC_Format_BIN);//获得系统的时间
	Time_Display(); //在底下进行的动态显示
	
	//显示界面
	image_display(0,0,(u8*)gImage_set_time);
	
	//判断之前是否有设置了定时
	AT24CXX_Read(4,&had_settimeflag,1);
	if(had_settimeflag==1||had_settimeflag==0)
	{
		//printf("正在画图\n");
		
		AT24CXX_Read(5,(u8 *)date_buff,10);
		delay_ms(100);
		AT24CXX_Read(15,(u8 *)time_buff,8);
		
		LCD_DisplayString_color(70,57,24,date_buff,BLUE,WHITE);
		LCD_DisplayString_color(70,81,24,time_buff,BLUE,WHITE);
		
		//printf("######################time_buff = %s \n",time_buff);
		
		if(had_settimeflag==0)
		{
			LCD_DisplayChinese_one(110,108,25,24);
		}else if(had_settimeflag==1)
		{
			LCD_DisplayChinese_one(110,108,24,24);
		}
	}
	
  while(1) 
	{		
		key_scan_advanced(0);	
		t++;
		//add
		XPT2046_Scan(0);
		
		if(label_closetime==1)
		{
			goto close;
		}
		
		//返回
		if(Xdown>0&&Xdown<36&&Ydown>0&&Ydown<38)
		{
			delay_ms(200);
			enter_set_time=0;
			//printf("break successful\n");
			break;
		}
		//开
		if(Xdown>30&&Xdown<86&&Ydown>145&&Ydown<183)
		{
			delay_ms(200);
			LCD_DisplayChinese_one(110,108,24,24);
			curtain_status_insettime=1;
		}
		//关
		if(Xdown>154&&Xdown<210&&Ydown>145&&Ydown<183)
		{
			delay_ms(200);
			LCD_DisplayChinese_one(110,108,25,24);
			curtain_status_insettime=0;
		}
		//关闭/取消定时
		if(Xdown>197&&Xdown<240&&Ydown>0&&Ydown<43)
		{
			delay_ms(200);
close:			
			label_closetime=0;
			image_display(0,0,(u8*)gImage_set_time);
			process=0;
			set_curtain_status=3;
			//在添加上把at24c02里边的值清理干净
			AT24CXX_Write(4,(u8*)&set_curtain_status,1);
			has_set_time_online=0;//是否设置了定时，让rtc的秒中断实时去处理
		}
		
		
		switch(process)
		{
			case 0:   // 流程0：时钟显示
			     {
						 if(key_tem==4&&key_time>250)//长按1.25秒，按键释放时，key_time自动设置0，见key.c
							{
								process=1;   //长按KEY3 进入调整时间流程
								break;
							}
						 if( ((t%50)==0) && (time_set_flag==0) )	//每250ms更新显示,并且没有定时
							{
								RTC_GetTimes(RTC_Format_BIN);//获取系统时间
								Time_Display();
							} 
						break;
			     }
			case 1:    // 流程1：调整时间
					{
					 if(keydown_data==KEY2_DATA) //按KEY2选项右移
					 {
							 option++;
							 if(option>5)   //循环
								 option=0;
					 }
					 					 
					 ADJUST_time(option,t);//调整时间函数
					 if(keydown_data==KEY3_DATA)
					 {
						 //Time_Display();// 防止在闪烁不显示的时候，短按KEY3确认调整后 显示空
						 //RTC_SetTimes(RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date,RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
						 //RTC_GetTimes(RTC_Format_BIN);
						 //Time_Display();
						 

						 Time_has_set();//先显示用户设置的时间，在判断其合理性
						 
						 //存放设置的时间日期
						 sprintf((char*)date_buff,"20%02d-%02d-%02d",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
						 //printf("%s  length = %d\n",date_buff,strlen(date_buff));
						 sprintf((char*)time_buff,"%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
						 //printf("%s  length = %d\n",time_buff,strlen(time_buff));
						
						//获取当前的时间日期,更改了时间
						 RTC_GetTimes(RTC_Format_BIN);
						 
						 sprintf((char*)now_date_buff,"20%02d-%02d-%02d",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
						 //printf("%s  length = %d\n",now_date_buff,strlen(now_date_buff));
						 sprintf((char*)now_time_buff,"%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
						 //printf("%s  length = %d\n",now_time_buff,strlen(now_time_buff));
						 
						 
						 if(strcmp(date_buff,now_date_buff)>0)
						 {
								//printf("set date ok\n");	
								set_tim_success=1;
						 }
						 else if(strcmp(date_buff,now_date_buff)==0)
						 {
								if(strcmp(time_buff,now_time_buff)>0)
								{
									//printf("set time ok\n");
									set_tim_success=1;
								}
								else
								{
									//printf("set time no\n");
									LCD_DisplayString_color(30,280,24,"Please Reset time",BLUE,WHITE);
									for(j=0;j<200;j++) delay_ms(10);
								}
						 }
						 else
						 {
								//printf("set date no\n");
								LCD_DisplayString_color(30,280,24,"Please Reset date",BLUE,WHITE);
							  for(j=0;j<200;j++) delay_ms(10);
						 }
						 
						 //设置的定时没有问题，再确认有选择窗帘开关，写入at24c02，时间比窗帘的开关更有必要
						 if(set_tim_success==1)
						 {
							 if(curtain_status_insettime!=3)
							 {
									printf("set and choose ok，curtain_status= %d \n",curtain_status_insettime);
									LCD_DisplayString_color(30,280,24,"Adjust TIM OK          ",BLUE,WHITE);
									//for(j=0;j<100;j++) delay_ms(10);  // Adjust OK 调整OK显示1秒
									//LCD_DisplayString(30,280,24,"                        ");
									
								 /*
										定时ok，窗帘选择ok，接下来就要写进入到at24c02中
										例子：
								  //AT24CXX_Write(0,(u8*)date_buff,strlen(date_buff));
									//AT24CXX_Read(0,test,strlen(date_buff));
									//printf("test = %s\n",test);
								 */
									AT24CXX_Write(4,(u8*)&curtain_status_insettime,1);
									//delay_ms(100);
								  //AT24CXX_Read(4,&temp,1);
									//printf("temp = %d \n",temp);
									
									AT24CXX_Write(5,(u8*)date_buff,strlen(date_buff));
									delay_ms(50);
									AT24CXX_Write(15,(u8*)time_buff,strlen(time_buff));
									delay_ms(50);
									
									AT24CXX_Read(5,set_time_buff,strlen(date_buff)+strlen(time_buff));
									
									//printf("test = %s\n",test);
									has_set_time_online=1;
									
								  option=0;    //选项从头来
									process=0;   //短按KEY3时间设置完成 返回到时间显示
									break;
								 
							 }
							 else
							 {
									LCD_DisplayString_color(0,280,24,"Please choose on/off",BLUE,WHITE);
									for(j=0;j<200;j++) delay_ms(10);
									image_display(0,0,(u8*)gImage_set_time);
							 }
						 }

						 
					 }
					break;
			   }
	  }
		delay_ms(5); //系统延时，也算是系统计时
	}	
}

/*
	功能：在00：00-06：00利用人体红外检测检测蜂鸣器警报
	用户开启后才起作用
*/
void beepalarm_in_night(void)
{
	int haspeople=0;
	haspeople=People_scan();
	if(haspeople==1)
	{
		
		if(beep_status==1)
		{
			//BEEP=1;
		  printf("beep is ringing\n");
		}
		
		data_buff[25]=5+48;
	}
	else
	{
		//BEEP=0;
		//printf("no people \n");
		data_buff[25]=0+48;
	}
}
//-*****************************************//



//尽量最终程序减少不必要的打印

int main()
{
	int only_test=0;
	int people=0;
	int i=0,cycle=0,T=2048;
	int adcx;
	/*
		定义标志变量
		初始值都是-1
		在变量后边，对所赋值进行说明
	*/
	u8 status;

	u8 system_mode=0;//1:智能 0：手动
	int chinese_mode[3]={11,12,-1};//模式的名字:智能
	u8 light_status=0;//1:弱 2：中 3：强
	
	//test
	Systick_init(168);  //初始化延时函数，没有初始化会导致程序卡死
	
	BEEP_init();
	LED_init();
  KEY_init();
	EXTI_init( );
	
	
	UART1_init(115200);
	printf("reset ----\n");
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	
	AT24CXX_init(); 
	while(AT24CXX_Check())
	{
		printf("AT24CXX 检测失败\n");
  	LED0=!LED0;
	}
	
	People_init();
	
	//光敏
	ADC_init();   
	adcx=Get_Adc_Average(ADC_Channel_9,20);//获取通道9的转换值，20次取平均
	light_streng_now=(4096-adcx)*100/4096;
	
	My_RTC_init();
	
	//在这里设置选择了ck_spre的时钟频率，即1HZ的频率
	//所有给0 会产生1S
	//可看寄存器的配置,10x是选择了ck_spre的时钟频率
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);//WAKE UP 
	
	
	TIM3_init(5*10000-1,8400-1);//定时器5秒检测开看门狗
	ESP8266_UART4_init(115200);
	WIFI_Server_Init();
	
	//ALL_SENSOR_init();
	LCD_init();  //初始化LCD FSMC接口和显示驱动
	
	Touch_Init();				//触摸屏的初始化
	
	printf("start \n");
	
ELECTRI_motor_init();
	//先对24C02存储的值进行判断，先初始化一下，可以进行优化一下

	//光照阈值
	AT24CXX_Read(1,&status,1);
	if(status!=1&&status!=2&&status!=3)
	{
		status=1;
		AT24CXX_Write(1,&status,1);
	}
	//夜晚蜂鸣器
	AT24CXX_Read(2,&beep_status,1);
	if(beep_status!=0&&beep_status!=1)
	{
		beep_status=0;
		AT24CXX_Write(2,&beep_status,1);
	}
	//模式
	AT24CXX_Read(3,&status,1);
	if(status!=0&&status!=1)
	{
		status=0;
		AT24CXX_Write(3,&status,1);
	}
	

	while(1)
	{
		switch(status)
		{
			case 1://------------------------------智能模式--------------------
				
				image_display(0,0,(u8*)gImage_smart_mode);
				LCD_showdate();//五秒刷新一次，到时候
			
				AT24CXX_Read(1,&light_status,1);
				if(light_status==1)//弱
				{
					LCD_DisplayChinese_one(167,148,21,24);
				}else if(light_status==2)//中
				{
					LCD_DisplayChinese_one(167,148,22,24);
				}else if(light_status==3)//强
				{
					LCD_DisplayChinese_one(167,148,23,24);
				}
			
				//窗帘状态----到时候会先去读取该值的
				if(curtain_status==0)
				{
					LCD_DisplayChinese_one(135,185,25,24);
				}
				else if(curtain_status==1)
				{
					LCD_DisplayChinese_one(135,185,24,24);
				}
				
				//显示模式
				chinese_mode[0]=11;
				chinese_mode[1]=12;
				LCD_DisplayChinese_string(135,223,24,chinese_mode);
			
				//夜晚蜂鸣器
				AT24CXX_Read(2,&beep_status,1);
				if(beep_status==0)
				{
					LCD_DisplayChinese_one(205,264,25,24);
				}
				else if(beep_status==1)
				{
					LCD_DisplayChinese_one(205,264,24,24);
				}
//---------------------------------------------------------------------			
				while(1)
				{
					XPT2046_Scan(0);
					//------------加光照阈值-------------
					if(Xdown>128&&Xdown<164&&Ydown>143&&Ydown<179)
					{
						delay_ms(200);
						
						light_status++;
						if(light_status>3)
						{
							light_status=1;
							LCD_DisplayChinese_one(167,148,21,24);
						}
						else
						{
							if(light_status==1)
							{
								LCD_DisplayChinese_one(167,148,21,24);
							}
							else if(light_status==2)
							{
								LCD_DisplayChinese_one(167,148,22,24);
							}
							else if(light_status==3)
							{
								LCD_DisplayChinese_one(167,148,23,24);
							}
						}
						
						AT24CXX_Write(1,&light_status,1);
						
					}
					//-------------减光照阈值-------------------
					if(Xdown>195&&Xdown<231&&Ydown>143&&Ydown<179)
					{
						delay_ms(200);
						
						light_status--;
						if(light_status<1)
						{
							light_status=3;
							LCD_DisplayChinese_one(167,148,23,24);
						}
						else
						{
							if(light_status==1)
							{
								LCD_DisplayChinese_one(167,148,21,24);
							}
							else if(light_status==2)
							{
								LCD_DisplayChinese_one(167,148,22,24);
							}
							else if(light_status==3)
							{
								LCD_DisplayChinese_one(167,148,23,24);
							}
						}
						AT24CXX_Write(1,&light_status,1);
					}
					
					if(label_mode==1)
					{
						goto smart_mode;
					}
					
					//切换模式
					if(Xdown>5&&Xdown<45&&Ydown>206&&Ydown<246)
					{
						
						delay_ms(200);
smart_mode:
						label_mode=0;
						status=0;
						
						system_mode=0;
						AT24CXX_Write(3,&system_mode,1);
						
						break;
					}
					//夜晚蜂鸣器
					if(Xdown>0&&Xdown<45&&Ydown>255&&Ydown<300)
					{
						delay_ms(200);
						
						if(beep_status==0)
						{
							LCD_DisplayChinese_one(205,264,24,24);
							beep_status=1;
							AT24CXX_Write(2,&beep_status,1);
						}
						else if(beep_status==1)
						{
							LCD_DisplayChinese_one(205,264,25,24);
							beep_status=0;
							AT24CXX_Write(2,&beep_status,1);
						}
						printf("蜂鸣器\n");
					}
					if(Zero_to_six_clock==1)
		 			{
						beepalarm_in_night();
					}
					else
					{
						data_buff[25]=0+48;
					}
//=================================================================================
					if(light_streng_now>light_streng_set)//光照强，关窗帘 light_streng_set会每十秒检测有没有改动
					{
						if(curtain_status==1)
						{
							close_rotate();
							curtain_status=0;
							LCD_DisplayChinese_one(135,185,25,24);
							printf("curtain close because light strength too high\n");
						}
					}
					else	//当前光照不强，打开窗帘
					{
						if(curtain_status==0)
						{
							open_rotate();
							curtain_status=1;
							LCD_DisplayChinese_one(135,185,24,24);
							printf("curtain open beacause light weak\n");
						}
					}
					
					
//===================================================================================					
					
					
				}
				break;
			
			case 0://-------------------------手动-------------------------
				image_display(0,0,(u8*)gImage_hand_mode);
				LCD_showdate();
//-----------------------------------------------------------			
				//窗帘状态----到时候会先去读取该值的
				if(curtain_status==0)
				{
					LCD_DisplayChinese_one(140,143,25,24);
				}
				else if(curtain_status==1)
				{
					LCD_DisplayChinese_one(140,143,24,24);
				}
			
				
				//显示模式
				chinese_mode[0]=13;
				chinese_mode[1]=14;
				LCD_DisplayChinese_string(140,173,24,chinese_mode);
				
				
				//夜晚蜂鸣器
				AT24CXX_Read(2,&beep_status,1);
				if(beep_status==0)
				{
					LCD_DisplayChinese_one(205,264,25,24);
				}
				else if(beep_status==1)
				{
					LCD_DisplayChinese_one(205,264,24,24);
				}
//---------------------------------------------------------------------			
			
				while(1)
				{
					XPT2046_Scan(0);
					if(label_mode==1)
					{
						goto hand_mode;
					}
					//切换模式
					if(Xdown>0&&Xdown<40&&Ydown>168&&Ydown<208)
					{

						delay_ms(300);
hand_mode:
						label_mode=0;
						status=1;
					
						system_mode=1;
						AT24CXX_Write(3,&system_mode,1);
						//printf("switch \n");
						break;
					}
					
					//窗帘ON
					if(Xdown>100&&Xdown<143&&Ydown>203&&Ydown<245)
					{
						delay_ms(200);
						LCD_DisplayChinese_one(140,143,24,24);
						
						if(curtain_status==0)
						{
							curtain_status=1;
						//add 马上发送消息到服务器上，不要切太快
							get_all_status_data(data_buff);
							SENDstr_to_server(data_buff);
//=======================================================
							open_rotate();
							
							printf("curtain open from hand mode\n");
						}
						
						//printf("on \n");
					}
					
					//窗帘OFF
						if(Xdown>147&&Xdown<190&&Ydown>203&&Ydown<245)
					{
						delay_ms(200);
						LCD_DisplayChinese_one(140,143,25,24);
						
						if(curtain_status==1)
						{
							curtain_status=0;
							//add 马上发送消息到服务器上，不要切太快
							get_all_status_data(data_buff);
							SENDstr_to_server(data_buff);
//====================================================================	
							close_rotate();
							printf("curtain off by hand mode\n");
						}
						//printf("off \n");

					}
					
					//设置定时开关
					if(Xdown>190&&Xdown<230&&Ydown>226&&Ydown<266)
					{
						delay_ms(200);
						enter_set_time=1;
						SET_time();
						
						break;//重新进入手动模式，刷新界面
					}
					
					//夜晚蜂鸣器
					if(Xdown>0&&Xdown<45&&Ydown>255&&Ydown<300)
					{
						delay_ms(300);
						
						if(beep_status==0)
						{
							LCD_DisplayChinese_one(205,264,24,24);
							beep_status=1;
							AT24CXX_Write(2,&beep_status,1);
						}
						else if(beep_status==1)
						{
							LCD_DisplayChinese_one(205,264,25,24);
							beep_status=0;
							AT24CXX_Write(2,&beep_status,1);
						}
						//printf("蜂鸣器\n");
					}
					if(Zero_to_six_clock==1)
					{
						beepalarm_in_night();
					}
					else
					{
						data_buff[25]=0+48;
					}	
				}
				break;
		}
	}

}

