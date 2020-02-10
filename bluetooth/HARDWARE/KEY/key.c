#include "key.h"
/*
	key0 1 2 3 ��Ӧ�� PF 9 8 7 6 

*/

void KEY_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	//����PF9 8 7 6 �ĳ�ʼ��
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9 | GPIO_Pin_8 | GPIO_Pin_7 | GPIO_Pin_6;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
	
}

int key_scanf(int mode)
{
	
	static int key_mode=1;//1֧�ֳ��� 0 ��֧�� �൱����һ����־λ
	if(mode) key_mode=1;
	if(key_mode&&((key0==0)||(key1==0)||(key2==0)||(key3==0)))
	{
		delay_ms(10);
		if(key0==0)
		{
			key_mode=0;
			return 1;
			
		}else if(key1==0)
		{
			key_mode=0;
			return 2;
			
		}else if(key2==0)
		{
			key_mode=0;
			return 3;
		}else if(key3==0)
		{
			key_mode=0;
			return 4;
		}
		
	//û�а���ʱ��Ҫ��λһ�£����ܰ���һ��	
	}else if((key0==1)&&(key1==1)&&(key2==1)&&(key3==1))
	{
		key_mode=1;
	}
	return 0;
}

/****************************************************************************
* ��    ��: void key_scan(u8 mode)
* ��    �ܣ�����ɨ�躯��-----���ܸ�ǿ���
* ��ڲ�����mode��0������ 
                  1: ����
* ���ز�������
* ˵    ������Ӧ���ȼ�,KEY0>KEY1>KEY2>KEY3�B
****************************************************************************/
u8  keydown_data=0x00;    //�������º�ͷ��ص�ֵ
u8  keyup_data=0x00;      //����̧�𷵻�ֵ
u16  key_time=0x00;       //��������֮���ʱ���������ֵ����ɨ��һ�ΰ���������ʱ��͵��ڰ������µ�ʱ��

u8  key_tem=0x00;         //�����İ���ֵ�밴��ɨ�������ɱ���
u8  key_bak=0x00;         //����ɨ�������ɱ���

void key_scan_advanced(u8 mode)
{	   
	 keyup_data=0;         //��̧��󰴼�ֵһ����Ч
	
	if(key0==0||key1==0||key2==0||key3==0)   //�м�������
	{
		if(key0==0)      key_tem=1;
		else if(key1==0) key_tem=2;
		else if(key2==0) key_tem=3;
		else if(key3==0) key_tem=4;
		
		if (key_tem == key_bak)      //�м����º��һ��ɨ�費������else��ϵڶ���ɨ����Ч������ʵ����ȥ����
		{
			 key_time++;             //�м����º�ִ��һ��ɨ�躯�����ñ�����1
			 keydown_data=key_tem;   //����ֵ����keydown_data
			
			 if( (mode==0)&&(key_time>1) )//key_time>1����ֵ��Ч������ǵ��������modeΪ1��Ϊ����
					keydown_data=0;
		}
		else                             //ȥ����      
		{
			 key_time=0;
			 key_bak=key_tem;
		}
	}
	else       //��̧��
	{
		 if(key_time>2)            //����̧��󷵻�һ�ΰ���ֵ
			{
				keyup_data=key_tem;  //��̧��󰴼�ֵ����keydown_data            						
			}
			key_bak=0;               //Ҫ���㣬��Ȼ�´�ִ��ɨ�����ʱ������ֵ���ϴΰ���ֵһ������û��ȥ����������
			key_time=0;
			keydown_data=0;		
	}    
}