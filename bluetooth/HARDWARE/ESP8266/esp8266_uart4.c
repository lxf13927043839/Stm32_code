#include "esp8266_uart4.h"
/*
	1������ʱ��ʹ�ܣ�GPIOsʱ��ʹ�ܡ�
	2���������Ÿ�����ӳ�䣬����GPIO_PinAFConfig����
	3��GPIO��ʼ�����ã�����ģʽΪ����ģʽ
	4�����ڵĲ����ĳ�ʼ�������ò����ʡ��ֳ�����żУ��Ȳ���
	5�������жϲ��ҳ�ʼ��NVIC,ʹ���ж�
	6��ʹ�ܴ���
	7����д�жϴ�����������������ʽ ��USARTxIRQHandler
	
	
	PA0 : TX
	PA1 : RX
*/



void ESP8266_UART4_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
	//1������GPIOʱ���Լ� uare4��ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	
	USART_DeInit(UART4);  //��λ����4
	
	//2����ʼ��IO ,���Ÿ���
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_UART4); // PA0 TX
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_UART4); // PA1 RX
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; //GPIOA0  GPIOA1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //PA0, PA1
	
	//3������4 �ĳ�ʼ��
	USART_InitStructure.USART_BaudRate = bound;//���� ������;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�8 λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;// 1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�շ�ģʽ
	USART_Init(UART4, &USART_InitStructure); //��ʼ������4 
	
	
	//4�������ж�
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//
	USART_ClearFlag(UART4, USART_FLAG_TC);
	
	//Uart4 NVIC 
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ� 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0; //��Ӧ���ȼ� 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ ʹ��
	NVIC_Init(&NVIC_InitStructure); 
	
	USART_Cmd(UART4 , ENABLE); //ʹ�ܴ��ڵ��ú���
}

/*
	ͨ��esp8266������4������ȡ�����ݣ�ת�����ַ������͵�tlink��
*/

void SENDstr_to_server(char* BUF)  
{  
	u16 i,j;
	i=strlen((const char*)BUF);//���ݷ��͵ĳ���
	//printf("the length is %d\n",i);   //������
	for(j=0;j<i;j++)//ѭ������
	{
	  while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET);  //�ȴ���һ�εĴ������
		USART_SendData(UART4,(uint8_t)BUF[j]); 	 //����ͨ������4���ͳ�ȥ
	}
}

/*
	��ʼ��esp8266��ָ����Լ����Ż�
*/
/*
	����esp8266�Ķ�Ӧ��ATָ��
*/
unsigned char UART4_SET_STATION[] = {"AT+CWMODE=1\r\n"};
unsigned char UART4_CONNECT_AP[]={"AT+CWJAP=\"LI\",\"13927043839\"\r\n"};
unsigned char UART4_CONNECT_SERVICE[]={"AT+CIPSTART=\"TCP\",\"tcp.tlink.io\",8647\r\n"};
unsigned char UART4_serial_mode[]={"AT+CIPMODE=1\r\n"};
unsigned char UART4_serial_mode1[]={"AT+CIPSEND\r\n"};
unsigned char UART4_device_num[]={"7GW46RC1I769E4GX"};

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


	SENDstr_to_server((char *)UART4_serial_mode1);
	delay_ms(500);
	delay_ms(500); 

                      
	SENDstr_to_server((char *)UART4_device_num);  
	delay_ms(500);
	delay_ms(500);   


}


