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
		�¶ȡ�ʪ�ȡ�����(tlink����ô���)��������ֵ(��ȷ��)������״̬��ϵͳģʽ��������⡢�Ƿ��������
		12    45   7 9    11  13 
*/

#define SIZE_of_DATA 22
unsigned char data_buff[SIZE_of_DATA]={"#11,22,3.2,4.0,55,1,0#"};

//char Weekday[][15]={{"Monday"},{"Tuesday"},{"Wednesday"},{"Thursday"},{"Friday"},{"Saturday"},{"Sunday"}}; 
            
          
           
             
           
             

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
       ʽ(16) �У�17�� ֵ��18�� ���϶ȣ�19�� �ٷֺţ�20�� ����21�� �У�22�� ǿ��23�� ����24�� �أ�25��
 */

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


extern const u8 gImage_set_light[];//���ù�����ֵ����
extern const u8 gImage_set_time[]; //���ö�ʱ

extern const u8 gImage_smart_mode[];
extern const u8 gImage_hand_mode[];


u8 curtain_status;//0:�����ر�



/*
   ���ܣ��Թ�����ֵ�������ã���д�뵽at24c02��

		�ڴ治��������
*/

void SET_lightcondition(void)
{
	//1����ʾͼƬ
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
		//��
		if(Xdown>20&&Xdown<80&&Ydown>85&&Ydown<125)
		{
			delay_ms(200);
			LCD_DisplayChinese_one(105,45,21,24);
			choose=1;
		}
		//��
		if(Xdown>85&&Xdown<145&&Ydown>85&&Ydown<125)
		{
			delay_ms(200);
			LCD_DisplayChinese_one(105,45,22,24);
			choose=2;
		}
		//ǿ
		if(Xdown>150&&Xdown<210&&Ydown>85&&Ydown<125)
		{
			delay_ms(200);
			LCD_DisplayChinese_one(105,45,23,24);
			choose=3;
		}
		//ȷ��-----------֮�󷵻���������
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
		
		//���ذ�ť--------������������
		if(Xdown>0&&Xdown<36&&Ydown>0&&Ydown<38)
		{
			printf("(x,y)=(%d,%d)\n",Xdown,Ydown);
			delay_ms(200);
			break;
		}
	}
}	

/*
	���ܣ���ʱ�����Ŀ����߹�---ע�⣺����ʾ�������ĵ�

*/
/*

* ˵    ����������KEY3����ʱ���������KEY0������ֵ�ļ�1����KEY1��ֵ���м�1��  
            ��KEY2����ѡ��Ҫ���õ�ѡ��ڵ������̰�KEY3��ȷ�ϵ���ʱ��         
*********************************************************************************/

//u8 const *weekdate[7]={"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};
u8 const *set_option[7]={"Year","Month","Day","Hours","Minutes","Seconds"," "};

void Time_Display()//�ѱ仯��ʱ��д��
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
void Time_has_set()//��ʾ��ʱ��ʱ��
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
		if(shanshuo%50==0)  //��ʾ����Ҫ���õ�ѡ��
		{
		  LCD_DisplayString_color(30,280,24,"Please Set          ",BLUE,WHITE);
		  LCD_DisplayString_color(162,280,24,(u8 *)set_option[option],BLUE,WHITE);
		}
		
		//����ʱ��ֵ�ļӼ�1
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
		//��˸��ʾ����ʾ
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
		//��˸��ʾ������ʾ
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
	//ע����һЩ�����ǳ�ʼ�����˵ģ�����ʱ��Ҫɾ��
	
	u8 process=0;  //������������
	u8 option=6;     //����ʱ��ѡ��
	u8 t=0,j;      //��ʱ����
	u8 time_set_flag=0;//�Ƿ������˰����Ķ�ʱ��־
	u8 now_date_buff[40];//��ǰ������
	u8 now_time_buff[40];//��ǰ��ʱ��

	u8 time_buff[40];//��Ŷ�ʱ��ʱ��
	u8 date_buff[40];//��Ŷ�ʱ������
	u8 test[40];//��24c02 ��ȡ���ݲ���
	u8 curtain_status=3;//����״̬,ֻ��0/1 ״̬����Ч�ģ�������������Ч��
	short set_tim_success=-1; //ʱ��������������� 
	u8 had_settimeflag=3; //����Ƿ���������
	u8 temp;
	/*
	char temp[]="20200215";
	char temp1[]="20200315";
	
	ret=Judge_correct_time(temp1,temp);
	printf("ret = %d\n",ret);
	*/ 
	
	/*
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	//Systick_init(168);         //��ʼ����ʱ����
	//UART1_init(115200);
	
	//LED_init();					  //��ʼ��LED
 	//LCD_init();					  //��ʼ��LCD
	//KEY_init();           //��ʼ��KEY
  */
	BRUSH_COLOR=RED;
	
	//��ɾ������
	/*
	LCD_DisplayString(10,10,24,"Illuminati STM32F4");	
	LCD_DisplayString(10,40,16,"Author:Clever");
	LCD_DisplayString(30,70,24,"14.RTC TEST");
	LCD_DisplayString(30,100,16,"KEY3 Long :Adjust time");
	LCD_DisplayString(30,120,16,"     Short:Adjust OK");
	LCD_DisplayString(30,140,16,"KEY0: V++  KEY1: V--");
	LCD_DisplayString(30,160,16,"KEY2: Right Move");
	*/
	RTC_GetTimes(RTC_Format_BIN);//���ϵͳ��ʱ��
	Time_Display(); //�ڵ��½��еĶ�̬��ʾ
	

	//��ʾ����
	
	image_display(0,0,(u8*)gImage_set_time);
	
	//�ж�֮ǰ�Ƿ��������˶�ʱ
	AT24CXX_Read(4,&had_settimeflag,1);
	if(had_settimeflag==1||had_settimeflag==0)
	{
		//printf("���ڻ�ͼ\n");
		
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
		
		//����
		if(Xdown>0&&Xdown<36&&Ydown>0&&Ydown<38)
		{
			delay_ms(200);
			
			//printf("break successful\n");
			break;
		}
		//��
		if(Xdown>30&&Xdown<86&&Ydown>145&&Ydown<183)
		{
			delay_ms(200);
			LCD_DisplayChinese_one(110,108,24,24);
			curtain_status=1;
		}
		//��
		if(Xdown>154&&Xdown<210&&Ydown>145&&Ydown<183)
		{
			delay_ms(200);
			LCD_DisplayChinese_one(110,108,25,24);
			curtain_status=0;
		}
		//�ر�/ȡ����ʱ
		if(Xdown>197&&Xdown<240&&Ydown>0&&Ydown<43)
		{
			delay_ms(200);
			
			image_display(0,0,(u8*)gImage_set_time);
			process=0;
			curtain_status=3;
			//������ϰ�at24c02��ߵ�ֵ����ɾ�
			
			AT24CXX_Write(4,(u8*)&curtain_status,1);
		}
		
		
		switch(process)
		{
			case 0:   // ����0��ʱ����ʾ
			     {
						 if(key_tem==4&&key_time>250)//����1.25�룬�����ͷ�ʱ��key_time�Զ�����0����key.c
							{
								process=1;   //����KEY3 �������ʱ������
								break;
							}
						 if( ((t%50)==0) && (time_set_flag==0) )	//ÿ250ms������ʾ,����û�ж�ʱ
							{
								RTC_GetTimes(RTC_Format_BIN);//��ȡϵͳʱ��
								Time_Display();
							} 
						break;
			     }
			case 1:    // ����1������ʱ��
					{
					 if(keydown_data==KEY2_DATA) //��KEY2ѡ������
					 {
							 option++;
							 if(option>5)   //ѭ��
								 option=0;
					 }
					 					 
					 ADJUST_time(option,t);//����ʱ�亯��
					 if(keydown_data==KEY3_DATA)
					 {
						 //Time_Display();// ��ֹ����˸����ʾ��ʱ�򣬶̰�KEY3ȷ�ϵ����� ��ʾ��
						 //RTC_SetTimes(RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date,RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
						 //RTC_GetTimes(RTC_Format_BIN);
						 //Time_Display();
						 

						 Time_has_set();//����ʾ�û����õ�ʱ�䣬���ж��������
						 
						 //������õ�ʱ������
						 sprintf((char*)date_buff,"20%02d-%02d-%02d",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
						 //printf("%s  length = %d\n",date_buff,strlen(date_buff));
						 sprintf((char*)time_buff,"%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
						 //printf("%s  length = %d\n",time_buff,strlen(time_buff));
						
						//��ȡ��ǰ��ʱ������,������ʱ��
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
						 
						 //���õĶ�ʱû�����⣬��ȷ����ѡ�������أ�д��at24c02��ʱ��ȴ����Ŀ��ظ��б�Ҫ
						 if(set_tim_success==1)
						 {
							 if(curtain_status!=3)
							 {
									printf("set and choose ok��curtain_status= %d \n",curtain_status);
									LCD_DisplayString_color(30,280,24,"Adjust TIM OK          ",BLUE,WHITE);
									//for(j=0;j<100;j++) delay_ms(10);  // Adjust OK ����OK��ʾ1��
									//LCD_DisplayString(30,280,24,"                        ");
									
								 /*
										��ʱok������ѡ��ok����������Ҫд���뵽at24c02��
										���ӣ�
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
									
								  option=0;    //ѡ���ͷ��
									process=0;   //�̰�KEY3ʱ��������� ���ص�ʱ����ʾ
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
		delay_ms(5); //ϵͳ��ʱ��Ҳ����ϵͳ��ʱ
	}	
}




int main()
{
	int only_test=0;
	int people=0;
	int i=0,cycle=0,T=2048;
	/*
		�����־����
		��ʼֵ����-1
		�ڱ�����ߣ�������ֵ����˵��
	*/
	u8 status;
	u8 beep_status=0;//1:���� 0���ر�
	u8 system_mode=0;//1:���� 0���ֶ�
	int chinese_mode[3]={11,12,-1};//ģʽ������:����
	u8 light_status=0;//1:�� 2���� 3��ǿ
	
	//test
	Systick_init(168);  //��ʼ����ʱ������û�г�ʼ���ᵼ�³�����
	
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
		printf("AT24CXX ���ʧ��\n");
  	LED0=!LED0;
	}
	
	
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
	LCD_showdate();
	
	//�ȶ�24C02�洢��ֵ�����жϣ��ȳ�ʼ��һ��
	//AT24CXX_Write(0,(u8*)date_buff,strlen(date_buff));
	
	//������ֵ
	AT24CXX_Read(1,&status,1);
	if(status!=1&&status!=2&&status!=3)
	{
		status=1;
		AT24CXX_Write(1,&status,1);
	}
	//ҹ�������
	AT24CXX_Read(2,&status,1);
	if(status!=0&&status!=1)
	{
		status=0;
		AT24CXX_Write(2,&status,1);
	}
	//ģʽ
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
			case 1://------------------------------����ģʽ--------------------
				
				image_display(0,0,(u8*)gImage_smart_mode);
				
				AT24CXX_Read(1,&light_status,1);
				if(light_status==1)//��
				{
					LCD_DisplayChinese_one(167,148,21,24);
				}else if(light_status==2)//��
				{
					LCD_DisplayChinese_one(167,148,22,24);
				}else if(light_status==3)//ǿ
				{
					LCD_DisplayChinese_one(167,148,23,24);
				}
			
				//����״̬----��ʱ�����ȥ��ȡ��ֵ��
				if(curtain_status==0)
				{
					LCD_DisplayChinese_one(135,185,25,24);
				}
				else if(curtain_status==1)
				{
					LCD_DisplayChinese_one(135,185,24,24);
				}
				
				//��ʾģʽ
				chinese_mode[0]=11;
				chinese_mode[1]=12;
				LCD_DisplayChinese_string(135,223,24,chinese_mode);
			
				//ҹ�������
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
					//------------�ӹ�����ֵ-------------
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
					//-------------��������ֵ-------------------
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
					
					//�л�ģʽ
					if(Xdown>5&&Xdown<45&&Ydown>206&&Ydown<246)
					{
						delay_ms(200);
						status=0;
						
						system_mode=0;
						AT24CXX_Write(3,&system_mode,1);
						
						break;
					}
					//ҹ�������
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
						
						
						printf("������\n");
					}
				}
				break;
			
			case 0://-------------------------�ֶ�-------------------------
				image_display(0,0,(u8*)gImage_hand_mode);

//-----------------------------------------------------------			
				//����״̬----��ʱ�����ȥ��ȡ��ֵ��
				if(curtain_status==0)
				{
					LCD_DisplayChinese_one(140,143,25,24);
				}
				else if(curtain_status==1)
				{
					LCD_DisplayChinese_one(140,143,24,24);
				}
			
				
				//��ʾģʽ
				chinese_mode[0]=13;
				chinese_mode[1]=14;
				LCD_DisplayChinese_string(140,173,24,chinese_mode);
				
				
				//ҹ�������
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
					
					//�л�ģʽ
					if(Xdown>0&&Xdown<40&&Ydown>168&&Ydown<208)
					{
						delay_ms(300);
						status=1;
						
						system_mode=1;
						AT24CXX_Write(3,&system_mode,1);
						//printf("switch \n");
						break;
					}
					
					//����ON
					if(Xdown>100&&Xdown<143&&Ydown>203&&Ydown<245)
					{
						delay_ms(200);
						LCD_DisplayChinese_one(140,143,24,24);
						
						curtain_status=1;
						//add ���Ϸ�����Ϣ���������ϣ���Ҫ��̫��
						
						
						//printf("on \n");
					}
					
					//����OFF
						if(Xdown>147&&Xdown<190&&Ydown>203&&Ydown<245)
					{
						delay_ms(200);
						LCD_DisplayChinese_one(140,143,25,24);
						
						curtain_status=0;
						//add ���Ϸ�����Ϣ���������ϣ���Ҫ��̫��
						
						//printf("off \n");
					}
					
					//���ö�ʱ����
					if(Xdown>190&&Xdown<230&&Ydown>226&&Ydown<266)
					{
						delay_ms(200);
						SET_time();
						break;//���½����ֶ�ģʽ��ˢ�½���
					}
					
					//ҹ�������
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
						
						
						//printf("������\n");
					}
					
				}
				break;
		}
	}

}
