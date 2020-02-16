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



void delay_ms(u16 nms);
//---------------------显示单个汉字与汉字字符串--------------------------
void LCD_DisplayChinese_one(u16 x,u16 y,u8 word,u8 size);
void LCD_DisplayChinese_string(u16 x,u16 y,u8 size,int *p);

void LCD_DisplayChar(u16 x,u16 y,u8 word,u8 size); //显示一个字符
void LCD_DisplayString(u16 x,u16 y,u8 size,u8 *p); //显示一个12/16/24字体字符串


//---------------------ESP8266--115200--------------------------------------
void front_rotate(void);
void area_rotate(void);
void stop_rotate(void);

void get_sensor_data(char *data_buff);
/*
		data_buff={#11,22,33,44,55,1,0#};
		温度、湿度、光照(tlink能怎么设计)、光照阈值(待确定)、窗帘状态、系统模式、人体红外、是否更新数据
		12    45   7 9    11  13 
*/

#define SIZE_of_DATA 22
unsigned char data_buff[SIZE_of_DATA]={"#11,22,3.2,4.0,55,1,0#"};

//char Weekday[][15]={{"Monday"},{"Tuesday"},{"Wednesday"},{"Thursday"},{"Friday"},{"Saturday"},{"Sunday"}}; 
            
          
           
             
           
             

#define SIZE_from_SEVRVER 50
char data_fromserver[SIZE_from_SEVRVER];
int num_from_server=0;
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
								//##################
								
								printf("the order from server-----%s=%d",data_fromserver,strlen(data_fromserver));
								
								if(strcmp(data_fromserver,"hello")==0)
								{
									printf("the order has received\n");
									front_rotate();
								}
								else if(strcmp(data_fromserver,"world")==0)
								{
									stop_rotate();
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
	//	{"sensorsId":200327142,"switcher":1}
		USART_ClearITPendingBit(UART4, USART_IT_RXNE); 
	}
		//清除UART4的接收中断	
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

int opendog_flag=-1;
int initdog=0;
int five_second_send_flag=0;
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
		if(link_flag==1) //定时5秒上传数据
		{
			five_second_send_flag++;
			if(five_second_send_flag==10)
			{
				five_second_send_flag=0;
				get_sensor_data((char *)data_buff);
				SENDstr_to_server((char *)data_buff);
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
					SENDstr_to_server("+++");	
					link_flag=0;//不进行喂狗操作
					printf("检测失败 启动看门狗\n");
			}

	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update); //清除中断标志位
}

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //溢出中断
	{	
		if(link_flag==1)	
		feed_dog();
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
			front_rotate();
			
		  printf("\nkey3 test by interrupt\n");
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
			
			area_rotate();
			
			//data_buff[18]='1';
			//SENDstr_to_server((char *)data_buff);
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
			
			stop_rotate();
			
			//data_buff[18]='0';
			//SENDstr_to_server((char *)data_buff);
			printf("\nkey1 test by interrupt\n");
		}		
		//清楚中断标志位，否则会一直进入中断函数
		EXTI_ClearITPendingBit(EXTI_Line8);
	}
	
}

//-----------电机控制初始化---------

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

void front_rotate(void)
{
	ENA=1;
	IN1=1;
	IN2=0;
}
void area_rotate(void)
{
	ENA=1;
	IN1=0;
	IN2=1;
}
void stop_rotate(void)
{
	ENA=1;
	IN1=0;
	IN2=0;
}

//初始化dht11 光敏、RTC
void ALL_SENSOR_init(void)
{
	//****************************************//
  //如果没有dht11 灯会闪烁
	while(DHT11_init())
	{
		LED1=!LED1;
	}
	//光敏
	ADC_init();      
	//RTC
	My_RTC_init();
	//在这里设置选择了ck_spre的时钟频率，即1HZ的频率
	//所有给0 会产生1S
	//可看寄存器的配置,10x是选择了ck_spre的时钟频率
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);//WAKE UP
	
	ELECTRI_motor_init();
	
	People_init();
	

	
}
//获取dht11 光敏、RTC数据
void get_sensor_data(char *data_buff)
{
	u8 temperature;//温度
  u8 humidity;   //湿度
	u16 adcx=0;			 //adc读取值
	float light_streng=0;
	RTC_TimeTypeDef RTC_TimeStruct;//时间
	RTC_DateTypeDef RTC_DateStruct;//日期
	u8 rtc_buf[40];
	
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
	sprintf((char*)rtc_buf,"Date:20%02d-%02d-%02d",RTC_DateStruct.RTC_Year,
	RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
	//printf("%s\n",rtc_buf);

	
	sprintf((char*)rtc_buf,"Week:%d",RTC_DateStruct.RTC_WeekDay);
	//printf("%s\n",rtc_buf);
	
	
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
	sprintf((char*)rtc_buf,"Time:%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,
	RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
	//printf("%s\n",rtc_buf);
	
	adcx=Get_Adc_Average(ADC_Channel_9,20);//获取通道9的转换值，20次取平均
	light_streng=(float)adcx*(3.3/4096);          //获取计算后的带小数的实际电压值，比如3.1111
	//printf("光敏:light_streng = %f\n",light_streng);
	
	DHT11_Read_Data(&temperature,&humidity);
	//printf("温度temperature = %d \n",temperature);
	//printf("湿度humidity    = %d \n",humidity);
	
	data_buff[1]=(temperature/10)+48;
	data_buff[2]=(temperature%10)+48;
	
	data_buff[4]=(humidity/10)+48;
	data_buff[5]=(humidity%10)+48;
	
	data_buff[7]=((int)(light_streng*10)/10)+48;
	data_buff[9]=((int)(light_streng*10)%10)+48;
	
	printf("所采集的数据data = %s \n",data_buff);
}

 /*------------------------------------------------------------------- 
	1、汉字大小：24*24共 72字节
  2、显示汉字时调用对应序号：
	温(0) 湿(1) 度(2) 光(3) 照(4) 强(5) 度(6) 窗(7) 帘(8) 状(9) 态(10) 智(11) 能(12) 手(13) 动(14) 模(15)
       式(16) 阈（17） 值（18） 摄氏度（19） 百分号（20） 弱（21） 中（22） 强（23） 开（24） 关（25）
 */

void LCD_show_RTC(void);
void LCD_showdate(void)
{
	int x=0,y=0;
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
	temp[1]=data_buff[9];
	LCD_DisplayString(x,y,24,temp); //显示一个12/16/24字体字符串
	
	//显示日期
	x=5;y=295;
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
	sprintf((char*)temp_buff,"20%02d-%02d-%02d",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
	//printf("%s\n",temp_buff);
	LCD_DisplayString(x,y,24,temp_buff); //显示一个12/16/24字体字符串
	
	//显示时间测试位置,显示ok---该函数放在秒的唤醒服务函数
	//LCD_show_RTC();
	
	/*	
	//在这里设置选择了ck_spre的时钟频率，即1HZ的频率
	//所有给0 会产生1S
	//可看寄存器的配置,10x是选择了ck_spre的时钟频率
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);//WAKE UP 


	
	
	//显示星期
	y=24*4;
	sprintf((char*)temp_buff,"Week:%s",Weekday[RTC_DateStruct.RTC_WeekDay-1]);
	//printf("%s\n",temp_buff);
	LCD_DisplayString(x,y,24,temp_buff); //显示一个12/16/24字体字符串

	//显示光照阈值
	y=24*6;
	LCD_DisplayChinese_string(x,y,24,chinese3);
	LCD_DisplayChar(x+24*4,y,':',24);//宽度只有8
	LCD_DisplayChinese_one(x+24*5,y,23,24);
	*/
}
void LCD_show_RTC(void)
{
	int x=0,y=0;
	RTC_TimeTypeDef RTC_TimeStruct;
	u8 temp_buff[40]; 

	//显示时间
	x=142;y=295;
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
	sprintf((char*)temp_buff,"%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
	//printf("%s\n",temp_buff);
	LCD_DisplayString(x,y,24,temp_buff); //显示一个12/16/24字体字符串
}

//跳转到设置光照阈值的界面


extern const u8 gImage_set_light[];//设置光照阈值界面
extern const u8 gImage_set_time[]; //设置定时

extern const u8 gImage_smart_mode[];
extern const u8 gImage_hand_mode[];


u8 curtain_status;//0:窗帘关闭



/*
   功能：对光照阈值进行设置，并写入到at24c02中

		内存不足舍弃了
*/

void SET_lightcondition(void)
{
	//1、显示图片
	u8 choose=0;
	u8 buff;
	
	Xdown=-1;
	Ydown=-1;
	
	
	AT24CXX_Read(1,(u8 *)&buff,1);
	if(buff==1)
	{
		LCD_DisplayChinese_one(105,45,21,24);
	}
	else if(buff==2)
	{
		LCD_DisplayChinese_one(105,45,22,24);
	}
	else if(buff==3)
	{
		
		LCD_DisplayChinese_one(105,45,23,24);
	}
	
	while(1)
	{
		XPT2046_Scan(0);
		//弱
		if(Xdown>20&&Xdown<80&&Ydown>85&&Ydown<125)
		{
			delay_ms(200);
			LCD_DisplayChinese_one(105,45,21,24);
			choose=1;
		}
		//中
		if(Xdown>85&&Xdown<145&&Ydown>85&&Ydown<125)
		{
			delay_ms(200);
			LCD_DisplayChinese_one(105,45,22,24);
			choose=2;
		}
		//强
		if(Xdown>150&&Xdown<210&&Ydown>85&&Ydown<125)
		{
			delay_ms(200);
			LCD_DisplayChinese_one(105,45,23,24);
			choose=3;
		}
		//确认-----------之后返回至主界面
		if(Xdown>97&&Xdown<147&&Ydown>134&&Ydown<187)
		{
			delay_ms(200);
			
			AT24CXX_WriteOneByte(1,choose);
			printf("light successful\n");
			
			//delay_ms(100);
			//AT24CXX_Read(1,(u8 *)&buff,1);
			//printf("choose = %d\n",choose);
			
			//add 
			
			break;
			
		}
		
		//返回按钮--------返回至主界面
		if(Xdown>0&&Xdown<36&&Ydown>0&&Ydown<38)
		{
			printf("(x,y)=(%d,%d)\n",Xdown,Ydown);
			delay_ms(200);
			break;
		}
	}
}	

/*
	功能：定时窗帘的开或者关---注意：把提示换成中文的

*/
/*

* 说    明：长按按KEY3进行时间调整，按KEY0进行数值的加1，按KEY1数值进行减1，  
            按KEY2右移选择要设置的选项，在调整完后短按KEY3，确认调整时间         
*********************************************************************************/

//u8 const *weekdate[7]={"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};
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
					case 1: RTC_DateStruct.RTC_Month+=1;  break;
					case 2: RTC_DateStruct.RTC_Date+=1;   break;
					case 3: RTC_TimeStruct.RTC_Hours+=1;  break;
					case 4: RTC_TimeStruct.RTC_Minutes+=1;break;
					case 5: RTC_TimeStruct.RTC_Seconds+=1;break;
				}	
			}
			if(keydown_data==KEY1_DATA)
			{
			  switch(option)
				{
				  case 0: RTC_DateStruct.RTC_Year-=1;   break;
					case 1: RTC_DateStruct.RTC_Month-=1;  break;
					case 2: RTC_DateStruct.RTC_Date-=1;   break;
					case 3: RTC_TimeStruct.RTC_Hours-=1;  break;
					case 4: RTC_TimeStruct.RTC_Minutes-=1;break;
					case 5: RTC_TimeStruct.RTC_Seconds-=1;break;
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

	u8 time_buff[40];//存放定时的时间
	u8 date_buff[40];//存放定时的日期
	u8 test[40];//从24c02 读取数据测试
	u8 curtain_status=3;//窗帘状态,只有0/1 状态是有效的，其他数字是无效的
	short set_tim_success=-1; //时间的设置是正常的 
	u8 had_settimeflag=3; //检测是否有设置了
	u8 temp;
	/*
	char temp[]="20200215";
	char temp1[]="20200315";
	
	ret=Judge_correct_time(temp1,temp);
	printf("ret = %d\n",ret);
	*/ 
	
	/*
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	//Systick_init(168);         //初始化延时函数
	//UART1_init(115200);
	
	//LED_init();					  //初始化LED
 	//LCD_init();					  //初始化LCD
	//KEY_init();           //初始化KEY
  */
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
		delay_ms(50);
		AT24CXX_Read(15,(u8 *)time_buff,8);
		
		LCD_DisplayString_color(70,57,24,date_buff,BLUE,WHITE);
		LCD_DisplayString_color(70,81,24,time_buff,BLUE,WHITE);
		
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
		
		//返回
		if(Xdown>0&&Xdown<36&&Ydown>0&&Ydown<38)
		{
			delay_ms(200);
			
			//printf("break successful\n");
			break;
		}
		//开
		if(Xdown>30&&Xdown<86&&Ydown>145&&Ydown<183)
		{
			delay_ms(200);
			LCD_DisplayChinese_one(110,108,24,24);
			curtain_status=1;
		}
		//关
		if(Xdown>154&&Xdown<210&&Ydown>145&&Ydown<183)
		{
			delay_ms(200);
			LCD_DisplayChinese_one(110,108,25,24);
			curtain_status=0;
		}
		//关闭/取消定时
		if(Xdown>197&&Xdown<240&&Ydown>0&&Ydown<43)
		{
			delay_ms(200);
			
			image_display(0,0,(u8*)gImage_set_time);
			process=0;
			curtain_status=3;
			//在添加上把at24c02里边的值清理干净
			
			AT24CXX_Write(4,(u8*)&curtain_status,1);
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
							 if(curtain_status!=3)
							 {
									printf("set and choose ok，curtain_status= %d \n",curtain_status);
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
									AT24CXX_Write(4,(u8*)&curtain_status,1);
									//delay_ms(100);
								  //AT24CXX_Read(4,&temp,1);
									//printf("temp = %d \n",temp);
									
									AT24CXX_Write(5,(u8*)date_buff,strlen(date_buff));
									delay_ms(50);
									AT24CXX_Write(15,(u8*)time_buff,strlen(time_buff));
									//delay_ms(50);
									
									//AT24CXX_Read(5,test,strlen(date_buff)+strlen(time_buff));
									
									//printf("test = %s\n",test);
									
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




int main()
{
	int only_test=0;
	int people=0;
	int i=0,cycle=0,T=2048;
	/*
		定义标志变量
		初始值都是-1
		在变量后边，对所赋值进行说明
	*/
	u8 status;
	u8 beep_status=0;//1:开启 0：关闭
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
	
	
	//TIM3_init(5*10000-1,8400-1);//定时器5秒检测开看门狗
	//ESP8266_UART4_init(115200);
	//WIFI_Server_Init();
	
	//ALL_SENSOR_init();
	LCD_init();  //初始化LCD FSMC接口和显示驱动

	
	Touch_Init();				//触摸屏的初始化
	
	printf("start \n");
	
	My_RTC_init();
	//在这里设置选择了ck_spre的时钟频率，即1HZ的频率
	//所有给0 会产生1S
	//可看寄存器的配置,10x是选择了ck_spre的时钟频率
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);//WAKE UP 
	
	//R_Touch_test(); //函数中间有一个循环检测触摸屏
	LCD_showdate();
	
	//先对24C02存储的值进行判断，先初始化一下
	//AT24CXX_Write(0,(u8*)date_buff,strlen(date_buff));
	
	//光照阈值
	AT24CXX_Read(1,&status,1);
	if(status!=1&&status!=2&&status!=3)
	{
		status=1;
		AT24CXX_Write(1,&status,1);
	}
	//夜晚蜂鸣器
	AT24CXX_Read(2,&status,1);
	if(status!=0&&status!=1)
	{
		status=0;
		AT24CXX_Write(2,&status,1);
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
					
					//切换模式
					if(Xdown>5&&Xdown<45&&Ydown>206&&Ydown<246)
					{
						delay_ms(200);
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
				}
				break;
			
			case 0://-------------------------手动-------------------------
				image_display(0,0,(u8*)gImage_hand_mode);

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
					
					//切换模式
					if(Xdown>0&&Xdown<40&&Ydown>168&&Ydown<208)
					{
						delay_ms(300);
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
						
						curtain_status=1;
						//add 马上发送消息到服务器上，不要切太快
						
						
						//printf("on \n");
					}
					
					//窗帘OFF
						if(Xdown>147&&Xdown<190&&Ydown>203&&Ydown<245)
					{
						delay_ms(200);
						LCD_DisplayChinese_one(140,143,25,24);
						
						curtain_status=0;
						//add 马上发送消息到服务器上，不要切太快
						
						//printf("off \n");
					}
					
					//设置定时开关
					if(Xdown>190&&Xdown<230&&Ydown>226&&Ydown<266)
					{
						delay_ms(200);
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
					
				}
				break;
		}
	}

}
