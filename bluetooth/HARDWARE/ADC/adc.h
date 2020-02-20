#ifndef __ADC_H
#define __ADC_H	
#include "sys.h" 

/*

	ADC 光敏对应的是PF3 的通道9
	
*/

void ADC_init(void); 				//ADC通道初始化
u16  Get_Adc(u8 ch); 				//获得某个通道值 
u16 Get_Adc_Average(u8 ch,u8 times);//得到某个通道给定次数采样的平均值  

void  ADC310_init(void);

#endif 


//int main()
//{
//	u16 adcx;
//	u8 temperature;//温度
//	u8 humidity;//湿度
//	float temp;
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
//	Systick_init(168);  //初始化延时函数
//	UART1_init(115200);	//初始化串口波特率为115200
//	LED_init();			//初始化LED 
//	ADC_init();         //初始化ADC
//	
//	//如果没有dht11 灯会闪烁
//	while(DHT11_init())
//	{
//		LED1=!LED1;	
//	}
//	
//	while(1)
//	{ 
//		adcx=Get_Adc_Average(ADC_Channel_9,20);//获取通道9的转换值，20次取平均
//		temp=(float)adcx*(3.3/4096);          //获取计算后的带小数的实际电压值，比如3.1111
//		printf("光敏light_ADC = %f \r\n",temp);
//		//说明 亮度暗
//		if((double)temp>1.3)
//		{
//			LED1=0;
//		}
//		else
//		{
//			LED1=1;
//		}
//		DHT11_Read_Data(&temperature,&humidity);
//		printf("    温度temperature = %d 摄氏度 \r\n",temperature);
//		printf("        湿度humidity = %d %%\r\n",humidity);
//		
//		LED0=!LED0;
//		delay_ms(500);
//	}

//}











