#include "stm32f4xx.h"
#include "stdio.h"

#include "usart.h"

void UART1_init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//1.串口时钟使能
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	
	
	//2. 设置引脚复用
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9, GPIO_AF_USART1); //GPIOA9-->USART1_TX
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10-->USART1_RX
	
	
	//3.GPIO端口初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9| GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	//4.串口参数初始化
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
	
	//5. 初始化中断控制器--NVIC
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn ; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 0x02;  //抢占优先级2，0~3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority= 0x02;    //响应优先级2， 0~3
	NVIC_InitStructure.NVIC_IRQChannelCmd= ENABLE;      //使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);   //中断优先级分组初始化
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//打开串口接收中断
	
	//6.使能串口
	USART_Cmd(USART1, ENABLE);  //使能串口1

}


void UART3_init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//1.串口3时钟使能
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	
	
	//2. 设置引脚复用
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11, GPIO_AF_USART3); //GPIOB11-->USART3_RX
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOB10-->USART3_TX
	
	
	//3.GPIO端口初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	//4.串口3参数初始化
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式
	USART_Init(USART3, &USART_InitStructure); //初始化串口1
	
	
	//5. 初始化中断控制器--NVIC
	NVIC_InitStructure.NVIC_IRQChannel=USART3_IRQn ; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 0x02;  //抢占优先级2，0~3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority= 0x03;    //响应优先级2， 0~3
	NVIC_InitStructure.NVIC_IRQChannelCmd= ENABLE;      //使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);   //中断优先级分组初始化
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//打开串口接收中断
	
	//6.使能串口
	USART_Cmd(USART3, ENABLE);  //使能串口1

}




void Put_Char(USART_TypeDef* USARTx, uint16_t Data)
{	
	USART_SendData(USARTx,Data);
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET); //等待数据发走
}

uint16_t Get_Char(USART_TypeDef* USARTx)
{	
	while(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET); //等待接收数据
	return USART_ReceiveData(USARTx);
}

int fputc(int c, FILE *stream)
{
	USART_SendData(USART1,c); //发送数据
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

	return c;
}


 int fgetc(FILE *stream)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	return USART_ReceiveData(USART1);
}


//void USART1_IRQHandler(void)
//{
//	uint16_t data;

//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//	{
//		data = USART_ReceiveData(USART1); //读数据
//				
//		//如果USART3	上一次发送完成
//		while(USART_GetFlagStatus(USART3, USART_FLAG_TC)==RESET);
//			
//		USART_SendData(USART3, data);
//		
//		//清除USART1的接收中断
//		USART_ClearITPendingBit(USART1, USART_IT_RXNE);

//	}

//} 


//测试之后是不行的

//串口发送缓冲区

/*

u16  USART3_TX_BUF[USART3_MAX_RECV_LEN];
//串口3,printf 函数
//确保一次发送的数据不超过USART3_MAX_SEND_LEN字节

//va_start 会与ADS 有冲突，需要设置电脑的环境变量才能正常使用
// ARMCC5INC   C:\Keil\ARM\ARMCC\include(keil的安装路径)
void u3_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);//数据发送的长度
	for(j=0;j<i;j++)//循环发送
	{
	  while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);  //等待上一次的传输结束
		USART_SendData(USART3,(uint8_t)USART3_TX_BUF[j]); 	 //发送数据到串口3
	}
	
}
*/
