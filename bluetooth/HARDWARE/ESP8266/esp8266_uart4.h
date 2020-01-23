#ifndef __ESP8266_USART4_H

#define __ESP8266_USART4_H
#include "stm32f4xx.h"

/*
	esp8266的波特率是115200
*/

void ESP8266_UART4_init(u32 bound);

#endif



/*
unsigned char esp8266_stm_str[2048];
int esp8266_stm_num=0;
void clear_buff(unsigned char esp8266_stm_str[1024])
{
	memset(esp8266_stm_str,0,1024);
}

//串口4 的中断服务程序 ====esp8266串口4 从手机端接收的信息
void UART4_IRQHandler(void)
{
	static uint16_t data=0;//具有记忆功能,用来测试一个灯的
	static unsigned char wifi_state=0;
	
	int i=0;
	//把收到的来自esp8266 模块的数据，通过USART1发给PC
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		data = USART_ReceiveData(UART4); //读数据
		
		//如果USART1	上一次发送完成，先不要直接发送给PC
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
//	  esp8266_stm_str[esp8266_stm_num]=data;	 
//		esp8266_stm_num++;
		USART_SendData(USART1, data);
		
//		printf("%c ",data);
	
		//printf("%d ",esp8266_stm_num);
		if(strstr((const char*)esp8266_stm_str,"ready")!=0)
	  {
			 printf("successful \n");
	     //clear_buff(esp8266_stm_str);
			 esp8266_stm_num = 0;
	  }
		
//		//测试控制LED灯
		if(data=='1')
		{
			LED2=0;
		}
		else if(data=='0')
		{
			LED2=1;
		}
	}
		//清除UART4的接收中断
		USART_ClearITPendingBit(UART4, USART_IT_RXNE); 
} 
//串口1 测试跟esp8266 通信====ok
void USART1_IRQHandler(void)
{
	uint16_t data;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		data = USART_ReceiveData(USART1); //读数据
				
		//如果UART4	上一次发送完成
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET);
		USART_SendData(UART4, data);
		
		//清除USART1的接收中断
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
} 

char UART4_SET_STATION[] = {"AT+CWMODE=1\r\n"};
char UART4_CONNECT_AP[]={"AT+CWJAP=\"LI\",\"13927043839\"\r\n"};
char UART4_CONNECT_SERVICE[]={"AT+CIPSTART=\"TCP\",\"tcp.tlink.io\",8647\r\n"};
char UART4_serial_mode[]={"AT+CIPMODE=1\r\n"};
char UART4_serial_mode1[]={"AT+CIPSEND\r\n"};
char UART4_device_num[]={"7GW46RC1I769E4GX"};

void uart4_send(char* BUF)  
{  
	u16 i,j;
	i=strlen((const char*)BUF);//数据发送的长度
	printf("i = %d\n",i);
	for(j=0;j<i;j++)//循环发送
	{
	  while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET);  //等待上一次的传输结束
		USART_SendData(UART4,(uint8_t)BUF[j]); 	 //发送数据到串口4
	}
}

void ESP8266_init(void)//启动要5s
{
		
		
	  delay_ms(500);//等待ESP8266启动
		
		uart4_send(UART4_SET_STATION);
		delay_ms(500);
		delay_ms(500);
		printf("hello 1\n");
	
		uart4_send(UART4_CONNECT_AP);
		delay_ms(500);
		delay_ms(500);	
		printf("hello 2\n");	
		uart4_send(UART4_CONNECT_SERVICE);
		delay_ms(500);
		delay_ms(500);
			printf("hello 3\n");
		uart4_send(UART4_serial_mode);
		delay_ms(500);
		delay_ms(500);
			printf("hello 4\n");
		uart4_send(UART4_serial_mode1);
		delay_ms(500);
		delay_ms(500);
				printf("hello 5\n");
		uart4_send(UART4_device_num);
		printf("hello 6\n");
}		
*/

