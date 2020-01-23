#include "esp8266_uart4.h"
/*
	1、串口时钟使能，GPIOs时钟使能。
	2、设置引脚复用器映射，调用GPIO_PinAFConfig函数
	3、GPIO初始化设置，设置模式为复用模式
	4、串口的参数的初始化：设置波特率、字长、奇偶校验等参数
	5、开启中断并且初始化NVIC,使能中断
	6、使能串口
	7、编写中断处理函数：函数命名格式 ：USARTxIRQHandler
	
	
	PA0 : TX
	PA1 : RX
*/



void ESP8266_UART4_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
	//1、开启GPIO时钟以及 uare4的使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	
	//2、初始化IO ,引脚复用
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_UART4); // PA0 TX
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_UART4); // PA1 RX
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; //GPIOA90  GPIOA1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //PA0, PA1
	
	//3、串口4 的初始化
	USART_InitStructure.USART_BaudRate = bound;//设置 波特率;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长8 位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;// 1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式
	USART_Init(UART4, &USART_InitStructure); //初始化串口4 
	
	
	//4、开启中断
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//
	//Uart4 NVIC 
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2; //响应优先级 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ 使能
	NVIC_Init(&NVIC_InitStructure); 
	
	//CH_PD要置高电平
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; // 
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;        //普通输出模式
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       //推挽输出
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   //100MHz
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //上拉
//	GPIO_Init(GPIOA,&GPIO_InitStructure);          // 
//  GPIO_SetBits(GPIOC, GPIO_Pin_1); 
	
	
	USART_Cmd(UART4 , ENABLE); //使能串口调用函数
}
