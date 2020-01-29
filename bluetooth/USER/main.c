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

/*
		data_buff={#11,22,33,44,55,1,0#};
		�¶ȡ�ʪ�ȡ����ա�������ֵ������״̬������ģʽ���Ƿ��������
*/

#define SIZE_of_DATA 21
unsigned char data_buff[SIZE_of_DATA]={"#11,22,33,44,55,1,0#"};


#define SIZE_from_SEVRVER 50
char data_fromserver[SIZE_from_SEVRVER];
int num_from_server=0;
int link_success=0; //��ʼ��esp8266ģ��ʱ����ָ���Ƿ�ɹ�ִ��
int link_flag=0; //�����ӹ����м������������tim4�жϺ����п��ƿ��Ź���

int first_ok_flag=0; //�����жϵ�һ�ε�ok���ö�ʱ����ֵ
int ok_flag=0;	//������Ӧ��������
unsigned char rece_status=0;//���շ���������ָ��


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
		
		if(link_success==0)  //�ж�wifi�Ƿ�ɹ����ӵ��������ϣ����Ӳ���----���Ź�����
		{
			if(data=='>')
			{
				LED2=0;
				link_success=1;
				feed_dog();
				TIM4_init(4*10000-1,8400-1); //��ʱ�������Ź�ʱ��ͬ��
				TIM_Cmd(TIM3,DISABLE); //ʧ��3
				link_flag=1;
			}
		}else if(link_success==1)  //��tlink���п���ָ��Լ��������Ĵ���
		{
			switch(rece_status)
			{
				case 0://0,1�ǶԿ���ָ��Ĵ���
							if(data=='{')
							{
								LED0=!LED0;
								rece_status++;
							}
							else
							{
								//����ɽ����Ż�
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
											TIM2_init(12*10000-1,8400-1);//���¶�ʱ��TIM2-----12�룬��������ͬ
										}
										else
										{
											//printf("�������ü�ʱ\n");							
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
		//���UART4�Ľ����ж�	
} 


/*
	��ʱ��3�жϷ�����
	//��ʱ��ʱ����84M 
	//��Ƶϵ����8400������ 84M/8400=10KHZ�ļ���Ƶ�ʣ�����5000��Ϊ500ms
  // 1/10 000 * 5000 = 0.5s = 500ms
  TIM3_init(5000-1,8400-1);

	------------------------���Ź�----------------------------
  //ʱ����㣺((4*2^4))/32000=0.2ms ����һ��0.2ms 500�ξ���1s
	IWDG_init(4,1000);

*/

int opendog_flag=-1;
int initdog=0;

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
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
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //����жϱ�־λ
}

int tim2flag=-1;
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //����ж�
	{	
			if(tim2flag==-1)
			{
				tim2flag=1;
			}
			else
			{
					SENDstr_to_server("+++");	
					link_flag=0;//������ι������
					printf("���ʧ�� �������Ź�\n");
			}

	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update); //����жϱ�־λ
}

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //����ж�
	{	
		if(link_flag==1)	
		feed_dog();
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update); //����жϱ�־λ
}



//����3 ���жϴ�����
void EXTI9_5_IRQHandler(void)
{
	//key3
	if(RESET != EXTI_GetITStatus(EXTI_Line6))
	{
		delay_ms(10);	//ȥ����
		if(key3==0)	 
		{
		  printf("\nkey3 test by interrupt\n");
		}		
		//����жϱ�־λ�������һֱ�����жϺ���
		EXTI_ClearITPendingBit(EXTI_Line6);
	}
	
	//key2
	if(RESET != EXTI_GetITStatus(EXTI_Line7))
	{
		delay_ms(10);	//ȥ����
		if(key2==0)	 
		{
			data_buff[18]='1';
			SENDstr_to_server((char *)data_buff);
			printf("\nkey2 test by interrupt\n");
		}		
		//����жϱ�־λ�������һֱ�����жϺ���
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
	//key1
	
	if(RESET != EXTI_GetITStatus(EXTI_Line8))
	{
		delay_ms(10);	//ȥ����
		if(key1==0)	 
		{	
			data_buff[18]='0';
			SENDstr_to_server((char *)data_buff);
			printf("\nkey1 test by interrupt\n");
		}		
		//����жϱ�־λ�������һֱ�����жϺ���
		EXTI_ClearITPendingBit(EXTI_Line8);
	}
	
}





int main()
{
	Systick_init(168);  //��ʼ����ʱ������û�г�ʼ���ᵼ�³�����
	
	LED_init();
  KEY_init();
	EXTI_init();
	
	UART1_init(115200);
	printf("reset ----\n");
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	
	TIM3_init(5*10000-1,8400-1);//��ʱ��5���⿪���Ź�
	ESP8266_UART4_init(115200);
	WIFI_Server_Init();
	
	printf("start \n");
	while(1)
	{
		
	}

}
