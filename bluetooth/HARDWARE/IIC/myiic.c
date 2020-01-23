#include "myiic.h"

void IIC_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	//GPIOB8,B9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//
	IIC_SCL=1;
	IIC_SDA=1;
}
//起始信号
void IIC_start(void)
{
	SDA_OUT(); //sda线输出
	IIC_SDA=1;
	IIC_SCL=1;
	delay_us(4);
	IIC_SDA=0;//START:when CLK is high,DATA change form high to low
	delay_us(4);
	
	IIC_SCL=0;//钳住IIC总线，准备发送或者接受数据
}

void IIC_stop(void)
{
	SDA_OUT();//sda 线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//发送I2C总线结束信号 
	delay_us(4);
}

//等待应答信号的到来
//返回值：1、表示应答失败 0、表示应答成功
u8 IIC_wait_ack(void)
{
	u8 ucErrTime=0;
	SDA_IN(); //SDA 输入
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

//iic发送一个字节
//返回从机有无应答
//1、有应答
//0、无应答
void IIC_Send_Byte(u8 txd)
{
	u8 t;
	SDA_OUT(); 
	IIC_SCL=0;//拉低时钟开始数据传输
	for(t=0;t<8;t++)
	{
		IIC_SDA=(txd&0x80)>>7;//一个字节一个字节发送
		txd<<=1; 
		delay_us(2); // 对于TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(2);
		IIC_SCL=0; 
		delay_us(2);
	}
}
//读一个字节 ack=1 发送ack 
//ack=0 发送nack
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
