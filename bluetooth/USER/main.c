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
//---------------------��ʾ���������뺺���ַ���--------------------------
void LCD_DisplayChinese_one(u16 x,u16 y,u8 word,u8 size);
void LCD_DisplayChinese_string(u16 x,u16 y,u8 size,int *p);
//---------------------ESP8266--115200--------------------------------------
void ESP8266_UART4_init(u32 bound); 



//-------------------------------------------------------------------------

/*
		data_buff={#11,22,33,44,55,1,0#};
		�¶ȡ�ʪ�ȡ����ա�������ֵ������״̬������ģʽ���Ƿ��������
*/

#define SIZE_of_DATA 21
unsigned char data_buff[SIZE_of_DATA]="#11,22,33,44,55,1,0#";

#define SIZE_from_SEVRVER 50
char server_to_stm[SIZE_from_SEVRVER];
int num_from_server=0;
unsigned char rece_status=0;

//����4 ���жϷ������ ====esp8266����4 ���ֻ��˽��յ���Ϣ
void UART4_IRQHandler(void)
{
	int i=0;
	uint16_t data;
	//���յ�������esp8266 ģ������ݣ�ͨ��USART1����PC
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		data = USART_ReceiveData(UART4); //������
	
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
		//���UART4�Ľ����ж�
		
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
	i=strlen((const char*)BUF);//���ݷ��͵ĳ���
	printf("the length is %d\n",i);
	for(j=0;j<i;j++)//ѭ������
	{
	  while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET);  //�ȴ���һ�εĴ������
		USART_SendData(UART4,(uint8_t)BUF[j]); 	 //����ͨ������4���ͳ�ȥ
	}
}

void WIFI_Server_Init(void)
{
	delay_ms(500);//Ҫ�ȴ�ESP8266��ȫ������ſ�ʼ��ָ��
	delay_ms(500);
	
	SENDstr_to_server((char *)UART4_SET_STATION);   
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	/*��Ҫһ��ʱ��ȥ�����ȵ㣬Ȼ����ȥ���ӷ�����
	����wifi connect---------wifi get ip֮��������tlink
	
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
	Systick_init(168);  //��ʼ����ʱ������û�г�ʼ���ᵼ�³�����
	LCD_init();
  KEY_init();
	
	UART1_init(115200);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	
	ESP8266_UART4_init(115200);
	WIFI_Server_Init();
	
	printf("start \n");
	while(1)
	{
		//ͨ�������������ݵ��ϴ�
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
