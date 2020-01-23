#ifndef __ESP8266_USART4_H

#define __ESP8266_USART4_H
#include "stm32f4xx.h"

/*
	esp8266�Ĳ�������115200
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

//����4 ���жϷ������ ====esp8266����4 ���ֻ��˽��յ���Ϣ
void UART4_IRQHandler(void)
{
	static uint16_t data=0;//���м��书��,��������һ���Ƶ�
	static unsigned char wifi_state=0;
	
	int i=0;
	//���յ�������esp8266 ģ������ݣ�ͨ��USART1����PC
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		data = USART_ReceiveData(UART4); //������
		
		//���USART1	��һ�η�����ɣ��Ȳ�Ҫֱ�ӷ��͸�PC
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
		
//		//���Կ���LED��
		if(data=='1')
		{
			LED2=0;
		}
		else if(data=='0')
		{
			LED2=1;
		}
	}
		//���UART4�Ľ����ж�
		USART_ClearITPendingBit(UART4, USART_IT_RXNE); 
} 
//����1 ���Ը�esp8266 ͨ��====ok
void USART1_IRQHandler(void)
{
	uint16_t data;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		data = USART_ReceiveData(USART1); //������
				
		//���UART4	��һ�η������
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET);
		USART_SendData(UART4, data);
		
		//���USART1�Ľ����ж�
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
	i=strlen((const char*)BUF);//���ݷ��͵ĳ���
	printf("i = %d\n",i);
	for(j=0;j<i;j++)//ѭ������
	{
	  while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET);  //�ȴ���һ�εĴ������
		USART_SendData(UART4,(uint8_t)BUF[j]); 	 //�������ݵ�����4
	}
}

void ESP8266_init(void)//����Ҫ5s
{
		
		
	  delay_ms(500);//�ȴ�ESP8266����
		
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

