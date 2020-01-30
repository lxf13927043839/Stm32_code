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


void delay_ms(u16 nms);
//---------------------显示单个汉字与汉字字符串--------------------------
void LCD_DisplayChinese_one(u16 x,u16 y,u8 word,u8 size);
void LCD_DisplayChinese_string(u16 x,u16 y,u8 size,int *p);
//---------------------ESP8266--115200--------------------------------------
void front_rotate(void);
void area_rotate(void);
void stop_rotate(void);

void get_sensor_data(void);
/*
		data_buff={#11,22,33,44,55,1,0#};
		温度、湿度、光照、光照阈值、窗帘状态、窗帘模式、是否更新数据
		12    45   7 9    11  13 
*/

#define SIZE_of_DATA 22
unsigned char data_buff[SIZE_of_DATA]={"#11,22,3.2,4.0,55,1,0#"};


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
											TIM2_init(12*10000-1,8400-1);//重新定时器TIM2-----12秒，跟心跳包同
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
			if(five_second_send_flag==5)
			{
				five_second_send_flag=0;
				get_sensor_data();
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
			
			data_buff[18]='1';
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
			
			data_buff[18]='0';
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
void get_sensor_data(void)
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



int main()
{
	
	int people=0;
	int i=0,cycle=0,T=2048;
	Systick_init(168);  //初始化延时函数，没有初始化会导致程序卡死
	
	BEEP_init();
	LED_init();
  KEY_init();
	EXTI_init( );
	
	UART1_init(115200);
	printf("reset ----\n");
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	
	TIM3_init(5*10000-1,8400-1);//定时器5秒检测开看门狗
	ESP8266_UART4_init(115200);
	WIFI_Server_Init();
	
	ALL_SENSOR_init();
	
	printf("start \n");
	
	while(1)
	{
		//delay_ms(500);
		//delay_ms(500);
		//get_sensor_data();
	}

}
