#ifndef __ADC_H
#define __ADC_H	
#include "sys.h" 

/*

	ADC ������Ӧ����PF3 ��ͨ��9
	
*/

void ADC_init(void); 				//ADCͨ����ʼ��
u16  Get_Adc(u8 ch); 				//���ĳ��ͨ��ֵ 
u16 Get_Adc_Average(u8 ch,u8 times);//�õ�ĳ��ͨ����������������ƽ��ֵ  

void  ADC310_init(void);

#endif 


//int main()
//{
//	u16 adcx;
//	u8 temperature;//�¶�
//	u8 humidity;//ʪ��
//	float temp;
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
//	Systick_init(168);  //��ʼ����ʱ����
//	UART1_init(115200);	//��ʼ�����ڲ�����Ϊ115200
//	LED_init();			//��ʼ��LED 
//	ADC_init();         //��ʼ��ADC
//	
//	//���û��dht11 �ƻ���˸
//	while(DHT11_init())
//	{
//		LED1=!LED1;	
//	}
//	
//	while(1)
//	{ 
//		adcx=Get_Adc_Average(ADC_Channel_9,20);//��ȡͨ��9��ת��ֵ��20��ȡƽ��
//		temp=(float)adcx*(3.3/4096);          //��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111
//		printf("����light_ADC = %f \r\n",temp);
//		//˵�� ���Ȱ�
//		if((double)temp>1.3)
//		{
//			LED1=0;
//		}
//		else
//		{
//			LED1=1;
//		}
//		DHT11_Read_Data(&temperature,&humidity);
//		printf("    �¶�temperature = %d ���϶� \r\n",temperature);
//		printf("        ʪ��humidity = %d %%\r\n",humidity);
//		
//		LED0=!LED0;
//		delay_ms(500);
//	}

//}











