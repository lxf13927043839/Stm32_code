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

void delay_ms(u16 nms);
//---------------------显示单个汉字与汉字字符串--------------------------
void LCD_DisplayChinese_one(u16 x,u16 y,u8 word,u8 size);
void LCD_DisplayChinese_string(u16 x,u16 y,u8 size,int *p);
//---------------------ESP8266--115200--------------------------------------
void ESP8266_UART4_init(u32 bound); 



//-------------------------------------------------------------------------

/*
		data_buff={#11,22,33,44,55,1,0#};
		温度、湿度、光照、光照阈值、窗帘状态、窗帘模式、是否更新数据
*/

#define SIZE_of_DATA 21
unsigned char data_buff[SIZE_of_DATA]="#11,22,33,44,55,1,0#";




#define SIZE_from_SEVRVER 50
char data_fromserver[SIZE_from_SEVRVER];
int num_from_server=0;
int link_success=0; //初始化esp8266模块时，看指令是否成功执行
int link_flag=0; //在连接过程中检测心跳包，在tim4中断函数中控制看门狗，

typedef enum{FALSE = 0,TRUE = 1} bool;

bool feeddog_flag=FALSE; //喂狗标志


int first_ok_flag=0; //用来判断第一次的ok设置定时器的值
int ok_flag=0;	//用来反应心跳包的
unsigned char rece_status=0;

int five_second_flag=0;

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
				
				link_flag=1;
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
											TIM3_init(5*10000-1,8400-1);//重新定时器5秒，跟心跳包同
											five_second_flag=1;
											ok_flag=1;
										}
										else
										{
											ok_flag=1;//这是第二个心跳包，第一个忽略,无法知道第一个心跳包的到来时间
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
		
		if(five_second_flag==1)
		{
			TIM2_init(10*10000-1,8400-1);
			TIM_Cmd(TIM3,DISABLE); //失能3
		}
		
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //清除中断标志位
}

int judge_ok_flag=-1;//用来判断心跳包ok 的发送次数是否有变化

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
	{	
		if(ok_flag==1)
		{
		  ok_flag=0;//清除一下ok的标志
			printf("检测成功\n");
		}
		else
		{
			//SENDstr_to_server("+++");
			//printf("the change data1111111111 is sent\n");
			
			//link_flag=0;//不进行喂狗操作
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



int main()
{
	unsigned char reset[]="AT+RST\r\n";
	int key_status=-1;
	Systick_init(168);  //初始化延时函数，没有初始化会导致程序卡死
	
	LED_init();
  KEY_init();
	
	UART1_init(115200);
	printf("reset ----\n");
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	
	TIM3_init(5*10000-1,8400-1);//定时器5秒检测开看门狗

	ESP8266_UART4_init(115200);
	WIFI_Server_Init();
	
	printf("start \n");
	while(1)
	{

		//通过按键测试数据的上传
		key_status=key_scanf(0);
		if(key_status==1)
		{
			SENDstr_to_server((char *)data_buff);
			printf("the data is sent\n");
		}
		else if(key_status==2)
		{
			data_buff[18]=1;
			SENDstr_to_server((char *)data_buff);
			printf("the change data is sent\n");
		}
		else if(key_status==4)
		{
			SENDstr_to_server((char *)reset);
			printf("the change data22222222 is sent\n");
		}
	}

}
