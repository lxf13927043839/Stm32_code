#include "beep.h"
//�������ĳ�ʼ����һ��ʼ�ǲ����


void BEEP_init()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);//����ʱ�ӵĳ�ʼ��

	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_7;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	//io�ڵĳ�ʼ��
	GPIO_Init(GPIOG,  &GPIO_InitStruct);
	
	//�÷�������ʼ��Ϊ�����״̬�����͵�ƽ
	
	GPIO_ResetBits(GPIOG,GPIO_Pin_7);
	
}
