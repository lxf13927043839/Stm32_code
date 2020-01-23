#include "stm32f4xx.h"
#include "stdio.h"

#include "usart.h"

void UART1_init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//1.����ʱ��ʹ��
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	
	
	//2. �������Ÿ���
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9, GPIO_AF_USART1); //GPIOA9-->USART1_TX
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10-->USART1_RX
	
	
	//3.GPIO�˿ڳ�ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9| GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	//4.���ڲ�����ʼ��
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
	
	//5. ��ʼ���жϿ�����--NVIC
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn ; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 0x02;  //��ռ���ȼ�2��0~3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority= 0x02;    //��Ӧ���ȼ�2�� 0~3
	NVIC_InitStructure.NVIC_IRQChannelCmd= ENABLE;      //ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);   //�ж����ȼ������ʼ��
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�򿪴��ڽ����ж�
	
	//6.ʹ�ܴ���
	USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1

}


void UART3_init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//1.����3ʱ��ʹ��
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	
	
	//2. �������Ÿ���
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11, GPIO_AF_USART3); //GPIOB11-->USART3_RX
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOB10-->USART3_TX
	
	
	//3.GPIO�˿ڳ�ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	//4.����3������ʼ��
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //�շ�ģʽ
	USART_Init(USART3, &USART_InitStructure); //��ʼ������1
	
	
	//5. ��ʼ���жϿ�����--NVIC
	NVIC_InitStructure.NVIC_IRQChannel=USART3_IRQn ; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 0x02;  //��ռ���ȼ�2��0~3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority= 0x03;    //��Ӧ���ȼ�2�� 0~3
	NVIC_InitStructure.NVIC_IRQChannelCmd= ENABLE;      //ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);   //�ж����ȼ������ʼ��
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�򿪴��ڽ����ж�
	
	//6.ʹ�ܴ���
	USART_Cmd(USART3, ENABLE);  //ʹ�ܴ���1

}




void Put_Char(USART_TypeDef* USARTx, uint16_t Data)
{	
	USART_SendData(USARTx,Data);
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET); //�ȴ����ݷ���
}

uint16_t Get_Char(USART_TypeDef* USARTx)
{	
	while(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET); //�ȴ���������
	return USART_ReceiveData(USARTx);
}

int fputc(int c, FILE *stream)
{
	USART_SendData(USART1,c); //��������
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
//		data = USART_ReceiveData(USART1); //������
//				
//		//���USART3	��һ�η������
//		while(USART_GetFlagStatus(USART3, USART_FLAG_TC)==RESET);
//			
//		USART_SendData(USART3, data);
//		
//		//���USART1�Ľ����ж�
//		USART_ClearITPendingBit(USART1, USART_IT_RXNE);

//	}

//} 


//����֮���ǲ��е�

//���ڷ��ͻ�����

/*

u16  USART3_TX_BUF[USART3_MAX_RECV_LEN];
//����3,printf ����
//ȷ��һ�η��͵����ݲ�����USART3_MAX_SEND_LEN�ֽ�

//va_start ����ADS �г�ͻ����Ҫ���õ��ԵĻ���������������ʹ��
// ARMCC5INC   C:\Keil\ARM\ARMCC\include(keil�İ�װ·��)
void u3_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);//���ݷ��͵ĳ���
	for(j=0;j<i;j++)//ѭ������
	{
	  while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);  //�ȴ���һ�εĴ������
		USART_SendData(USART3,(uint8_t)USART3_TX_BUF[j]); 	 //�������ݵ�����3
	}
	
}
*/
