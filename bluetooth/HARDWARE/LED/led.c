#include "led.h" 

//��ʼ�� PE3 4 ��PG9 Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG, ENABLE);			//ʹ��GPIOE��GPIOFʱ��

  //GPIOE3,E4 ��ʼ������ 
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3 | GPIO_Pin_4;		//LED0��LED1��ӦIO��
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;			    //��ͨ���ģʽ��
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//�������������LED��Ҫ��������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		    //100MHz
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				    //����
  GPIO_Init(GPIOE, &GPIO_InitStructure);						//��ʼ��GPIOE�������õ�����д��Ĵ���
  
  //GPIOG9   ��ʼ������ 
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;	//LED2��LED3��ӦIO��
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;				//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//100MHz
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;					//����
  GPIO_Init(GPIOG, &GPIO_InitStructure);						//��ʼ��GPIOG�������õ�����д��Ĵ���
	
  GPIO_SetBits(GPIOE,GPIO_Pin_3  | GPIO_Pin_4);			  //GPIOE3,E4   ���øߣ�����
  GPIO_SetBits(GPIOG,GPIO_Pin_9);				//GPIOG9      ���øߣ�����
}






