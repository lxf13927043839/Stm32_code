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
//---------------------��ʾ���������뺺���ַ���--------------------------
void LCD_DisplayChinese_one(u16 x,u16 y,u8 word,u8 size);
void LCD_DisplayChinese_string(u16 x,u16 y,u8 size,int *p);

void LCD_DisplayChar(u16 x,u16 y,u8 word,u8 size); //��ʾһ���ַ�
void LCD_DisplayString(u16 x,u16 y,u8 size,u8 *p); //��ʾһ��12/16/24�����ַ���


//---------------------ESP8266--115200--------------------------------------
void front_rotate(void);
void area_rotate(void);
void stop_rotate(void);

void get_sensor_data(char *data_buff);
/*
		data_buff={#11,22,33,44,55,1,0#};
		�¶ȡ�ʪ�ȡ����ա�������ֵ������״̬������ģʽ���Ƿ��������
		12    45   7 9    11  13 
*/

#define SIZE_of_DATA 22
unsigned char data_buff[SIZE_of_DATA]={"#11,22,3.2,4.0,55,1,0#"};

char Weekday[][15]={{"Monday"},{"Tuesday"},{"Wednesday"},{"Thursday"},{"Friday"},{"Saturday"},{"Sunday"}}; 
            
          
           
             
           
             

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
				//TIM_Cmd(TIM3,DISABLE);
				TIM3_init(10000-1,8400-1);//��ʱ1�룬������ʱ�ϴ������Լ�ɨ��LCD��Ļ
				TIM3->CNT=0;
				
				link_flag=1;//���ӳɹ�
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
											TIM2_init(14*10000-1,8400-1);//���¶�ʱ��TIM2-----14�룬��������ͬ
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
int five_second_send_flag=0;
/*
	������esp8266�Լ����ӷ�������������5��
	�����ӳɹ���ʱ�򣬸���Ϊ1�룬������ʱ�ϴ����� 
*/
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
		if(link_flag==1) //��ʱ5���ϴ�����
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
			front_rotate();
			
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
			
			area_rotate();
			
			//data_buff[18]='1';
			//SENDstr_to_server((char *)data_buff);
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
			
			stop_rotate();
			
			//data_buff[18]='0';
			//SENDstr_to_server((char *)data_buff);
			printf("\nkey1 test by interrupt\n");
		}		
		//����жϱ�־λ�������һֱ�����жϺ���
		EXTI_ClearITPendingBit(EXTI_Line8);
	}
	
}

//-----------������Ƴ�ʼ��---------

#define ENA PBout(10)	
#define IN1 PBout(11)	// D1	 
#define IN2 PCout(6)	// D1	 
void ELECTRI_motor_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//����GPIOʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	//��ʼ��IO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOB11  GPIOB10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
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

//��ʼ��dht11 ������RTC
void ALL_SENSOR_init(void)
{
	//****************************************//
  //���û��dht11 �ƻ���˸
	while(DHT11_init())
	{
		LED1=!LED1;
	}
	//����
	ADC_init();      
	//RTC
	My_RTC_init();
	//����������ѡ����ck_spre��ʱ��Ƶ�ʣ���1HZ��Ƶ��
	//���и�0 �����1S
	//�ɿ��Ĵ���������,10x��ѡ����ck_spre��ʱ��Ƶ��
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);//WAKE UP
	
	ELECTRI_motor_init();
	
	People_init();
	

	
}
//��ȡdht11 ������RTC����
void get_sensor_data(char *data_buff)
{
	u8 temperature;//�¶�
  u8 humidity;   //ʪ��
	u16 adcx=0;			 //adc��ȡֵ
	float light_streng=0;
	RTC_TimeTypeDef RTC_TimeStruct;//ʱ��
	RTC_DateTypeDef RTC_DateStruct;//����
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
	
	adcx=Get_Adc_Average(ADC_Channel_9,20);//��ȡͨ��9��ת��ֵ��20��ȡƽ��
	light_streng=(float)adcx*(3.3/4096);          //��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111
	//printf("����:light_streng = %f\n",light_streng);
	
	DHT11_Read_Data(&temperature,&humidity);
	//printf("�¶�temperature = %d \n",temperature);
	//printf("ʪ��humidity    = %d \n",humidity);
	
	data_buff[1]=(temperature/10)+48;
	data_buff[2]=(temperature%10)+48;
	
	data_buff[4]=(humidity/10)+48;
	data_buff[5]=(humidity%10)+48;
	
	data_buff[7]=((int)(light_streng*10)/10)+48;
	data_buff[9]=((int)(light_streng*10)%10)+48;
	
	printf("���ɼ�������data = %s \n",data_buff);
}

 /*------------------------------------------------------------------- 
	1�����ִ�С��24*24�� 72�ֽ�
  2����ʾ����ʱ���ö�Ӧ��ţ�
	��(0) ʪ(1) ��(2) ��(3) ��(4) ǿ(5) ��(6) ��(7) ��(8) ״(9) ̬(10) ��(11) ��(12) ��(13) ��(14) ģ(15)
       ʽ(16) �У�17�� ֵ��18�� ���϶ȣ�19�� �ٷֺţ�20�� ����21�� �У�22�� ǿ��23��
 */
int chinese[]={0,2,-1};//�¶�
int chinese1[]={1,2,-1};//ʪ��
int chinese2[]={3,4,5,6,-1};//����ǿ��
int chinese3[]={3,4,17,18,-1};//������ֵ

void LCD_show_RTC(void);
void LCD_showdate(void)
{
	int x=0,y=0;
	u8 temp[2];
	RTC_DateTypeDef RTC_DateStruct;
	u8 temp_buff[40]; 
	
	
	BRUSH_COLOR=RED;      //���û�����ɫΪ��ɫ
	//�¶�
	x=95,y=47;
	temp[0]=data_buff[1];
	temp[1]=data_buff[2];
	LCD_DisplayString(x,y,24,temp); //��ʾһ��24�����ַ�������ֻռ��12,�ַ���ʾʱ��
	LCD_DisplayChinese_one(x+12*2,y,19,24);
	
	//ʪ��
	x=95;y=80;
	temp[0]=data_buff[4];
	temp[1]=data_buff[5];
	LCD_DisplayString(x,y,24,temp); //��ʾһ��12/16/24�����ַ���
	LCD_DisplayChinese_one(x+12*2,y,20,24);
	
	//����ǿ��----���������С�ǿ����ʾ���������岻��
	x=95;y=113;
	temp[0]=data_buff[7];
	temp[1]=data_buff[9];
	LCD_DisplayString(x,y,24,temp); //��ʾһ��12/16/24�����ַ���
	
	//��ʾ����
	x=5;y=295;
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
	sprintf((char*)temp_buff,"20%02d-%02d-%02d",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
	//printf("%s\n",temp_buff);
	LCD_DisplayString(x,y,24,temp_buff); //��ʾһ��12/16/24�����ַ���
	
	//��ʾʱ�����λ��,��ʾok---�ú���������Ļ��ѷ�����
	//LCD_show_RTC();
	
	/*	
	//����������ѡ����ck_spre��ʱ��Ƶ�ʣ���1HZ��Ƶ��
	//���и�0 �����1S
	//�ɿ��Ĵ���������,10x��ѡ����ck_spre��ʱ��Ƶ��
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);//WAKE UP 


	
	
	//��ʾ����
	y=24*4;
	sprintf((char*)temp_buff,"Week:%s",Weekday[RTC_DateStruct.RTC_WeekDay-1]);
	//printf("%s\n",temp_buff);
	LCD_DisplayString(x,y,24,temp_buff); //��ʾһ��12/16/24�����ַ���

	//��ʾ������ֵ
	y=24*6;
	LCD_DisplayChinese_string(x,y,24,chinese3);
	LCD_DisplayChar(x+24*4,y,':',24);//���ֻ��8
	LCD_DisplayChinese_one(x+24*5,y,23,24);
	*/
}
void LCD_show_RTC(void)
{
	int x=0,y=0;
	RTC_TimeTypeDef RTC_TimeStruct;
	u8 temp_buff[40]; 

	//��ʾʱ��
	x=142;y=295;
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
	sprintf((char*)temp_buff,"%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
	//printf("%s\n",temp_buff);
	LCD_DisplayString(x,y,24,temp_buff); //��ʾһ��12/16/24�����ַ���
}

//��ת�����ù�����ֵ�Ľ���

extern const u8 gImage_smart_on[];//ͼƬ��ȡ��������ɫֵ
extern const u8 gImage_smart_off[];
char smart_change_flag=1;

int main()
{
	int only_test=0;
	int people=0;
	int i=0,cycle=0,T=2048;
	//test
	HEADCOLOR *imginfo;
	imginfo=(HEADCOLOR*)gImage_smart_on;
	
	Systick_init(168);  //��ʼ����ʱ������û�г�ʼ���ᵼ�³�����
	
	BEEP_init();
	LED_init();
  KEY_init();
	EXTI_init( );
	
	UART1_init(115200);
	printf("reset ----\n");
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	
	//TIM3_init(5*10000-1,8400-1);//��ʱ��5���⿪���Ź�
	//ESP8266_UART4_init(115200);
	//WIFI_Server_Init();
	
	//ALL_SENSOR_init();
	LCD_init();  //��ʼ��LCD FSMC�ӿں���ʾ����

	
	Touch_Init();				//�������ĳ�ʼ��
	
	printf("start \n");
	
	My_RTC_init();
	//����������ѡ����ck_spre��ʱ��Ƶ�ʣ���1HZ��Ƶ��
	//���и�0 �����1S
	//�ɿ��Ĵ���������,10x��ѡ����ck_spre��ʱ��Ƶ��
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);//WAKE UP 
	
	//R_Touch_test(); //�����м���һ��ѭ����ⴥ����
	image_display(0,0,(u8*)gImage_smart_on);
	LCD_showdate();
	
	while(1)
	{
		XPT2046_Scan(0);//�����з�Ӧ������� 		 
		/*
		if(Xdown>0&&Xdown<45&&Ydown>255&&Ydown<300)
		{
			printf("(x,y)=(%d,%d)\n",Xdown,Ydown);
			if(smart_change_flag==1)
			{
				imginfo=(HEADCOLOR*)gImage_smart_off;
				image_display(0,0,(u8*)gImage_smart_off);
				smart_change_flag=0;
			}
			else if(smart_change_flag==0)
			{
				imginfo=(HEADCOLOR*)gImage_smart_on;
				image_display(0,0,(u8*)gImage_smart_on);
				smart_change_flag=1;
			}
			delay_ms(200);
		}
		*/
		if(Xdown>0&&Xdown<240&&Ydown>0&&Ydown<320)
		{
			printf("(x,y)=(%d,%d)\n",Xdown,Ydown);
			delay_ms(200);
		}
	}

}
