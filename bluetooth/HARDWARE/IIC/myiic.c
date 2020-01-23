#include "myiic.h"

void IIC_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	//GPIOB8,B9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//
	IIC_SCL=1;
	IIC_SDA=1;
}
//��ʼ�ź�
void IIC_start(void)
{
	SDA_OUT(); //sda�����
	IIC_SDA=1;
	IIC_SCL=1;
	delay_us(4);
	IIC_SDA=0;//START:when CLK is high,DATA change form high to low
	delay_us(4);
	
	IIC_SCL=0;//ǯסIIC���ߣ�׼�����ͻ��߽�������
}

void IIC_stop(void)
{
	SDA_OUT();//sda �����
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//����I2C���߽����ź� 
	delay_us(4);
}

//�ȴ�Ӧ���źŵĵ���
//����ֵ��1����ʾӦ��ʧ�� 0����ʾӦ��ɹ�
u8 IIC_wait_ack(void)
{
	u8 ucErrTime=0;
	SDA_IN(); //SDA ����
	IIC_SDA=1;
	delay_us(1); 
	IIC_SCL=1;
	delay_us(1);
	
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_stop();
			return 1;
		}
	}
	
	IIC_SCL=0;
	return 0;
}


void IIC_ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
void IIC_nack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}

//iic����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
void IIC_Send_Byte(u8 txd)
{
	u8 t;
	SDA_OUT(); 
	IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
	for(t=0;t<8;t++)
	{
		IIC_SDA=(txd&0x80)>>7;//һ���ֽ�һ���ֽڷ���
		txd<<=1; 
		delay_us(2); // ����TEA5767��������ʱ���Ǳ����
		IIC_SCL=1;
		delay_us(2);
		IIC_SCL=0; 
		delay_us(2);
	}
}
//��һ���ֽ� ack=1 ����ack 
//ack=0 ����nack
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA ?????
	for(i=0;i<8;i++ )
	{
		IIC_SCL=0; 
		delay_us(2);
		IIC_SCL=1;
		receive<<=1;
		if(READ_SDA)receive++;
		delay_us(1);
	}
	if (!ack) 
		IIC_nack();
	else
		IIC_ack(); 
	return receive;
}
