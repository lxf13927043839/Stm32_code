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
char server_to_stm[SIZE_from_SEVRVER];
int num_from_server=0;
unsigned char rece_status=0;

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
		
		switch(rece_status)
		{
			case 0:
				if(data=='{')
				{
					LED0=!LED0;
					rece_status++;
					
				}
			break;
			case 1:
				server_to_stm[num_from_server]=data;
				num_from_server++;
				if(server_to_stm[num_from_server-1]=='}')
				{
					server_to_stm[num_from_server-1]='\0';
					LED1=!LED1;
					//##################
					
					printf("#####-----%s=%d",server_to_stm,strlen(server_to_stm));
					
					//##################
					for(i=0;i<SIZE_from_SEVRVER;i++)
					{
						server_to_stm[i]=0;
					}
					num_from_server=0;
					rece_status=0;
				}
			break;
		
		}
		
	//	{"sensorsId":200327142,"switcher":1}
		USART_ClearITPendingBit(UART4, USART_IT_RXNE); 
	}
		//清除UART4的接收中断
		
} 

unsigned char UART4_SET_STATION[] = {"AT+CWMODE=1\r\n"};
unsigned char UART4_CONNECT_AP[]={"AT+CWJAP=\"LI\",\"13927043839\"\r\n"};
unsigned char UART4_CONNECT_SERVICE[]={"AT+CIPSTART=\"TCP\",\"tcp.tlink.io\",8647\r\n"};
unsigned char UART4_serial_mode[]={"AT+CIPMODE=1\r\n"};
unsigned char UART4_serial_mode1[]={"AT+CIPSEND\r\n"};
unsigned char UART4_device_num[]={"7GW46RC1I769E4GX"};


void SENDstr_to_server(char* BUF)  
{  
	u16 i,j;
	i=strlen((const char*)BUF);//数据发送的长度
	printf("the length is %d\n",i);
	for(j=0;j<i;j++)//循环发送
	{
	  while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET);  //等待上一次的传输结束
		USART_SendData(UART4,(uint8_t)BUF[j]); 	 //数据通过串口4发送出去
	}
}

void WIFI_Server_Init(void)
{
	delay_ms(500);//要等待ESP8266完全启动后才开始发指令
	delay_ms(500);
	
	SENDstr_to_server((char *)UART4_SET_STATION);   
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	/*需要一定时间去连接热点，然后在去连接服务器
	出现wifi connect---------wifi get ip之后在连接tlink
	
	*/
	SENDstr_to_server((char *)UART4_CONNECT_AP); 
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);      
	delay_ms(500);	
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	
	SENDstr_to_server((char *)UART4_CONNECT_SERVICE);    
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);                         
	
	SENDstr_to_server((char *)UART4_serial_mode);  
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);

	SENDstr_to_server((char *)UART4_serial_mode1);
	delay_ms(500);
	delay_ms(500); 
	delay_ms(500);
                      
	SENDstr_to_server((char *)UART4_device_num);  
	delay_ms(500);
	delay_ms(500);   
	delay_ms(500);

}

int main()
{
	int key_status=-1;
	
	LED_init();
	Systick_init(168);  //初始化延时函数，没有初始化会导致程序卡死
	LCD_init();
  KEY_init();
	
	UART1_init(115200);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	
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
	}

}
