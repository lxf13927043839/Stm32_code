#include "24cxx.h"

void AT24CXX_init(void)
{
	IIC_init();//IIC 初始化

}
//指定地址读出一个数据
//readaddr 开始读数的地址
//返回值：读到的数据
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{ 
	u8 temp=0;
	IIC_start();

	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0); //发送写命令
		IIC_wait_ack();
		IIC_Send_Byte(ReadAddr>>8);//发送高地址
	}else
	{
		IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));
	} //发送器件地址 0XA0,写数据
	IIC_wait_ack();
	IIC_Send_Byte(ReadAddr%256); //发送低地址
	IIC_wait_ack(); 
	IIC_start(); 
	IIC_Send_Byte(0XA1); //进入接受模式
	IIC_wait_ack();
	temp=IIC_Read_Byte(0); 
	IIC_stop();
	return temp;
}
//在AT24CXX 指定地址写入一个数据
//WriteAddr :写入数据的目的地址
//DataToWrite:要写入的数据
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{
	IIC_start();
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0); //发送写命令
		IIC_wait_ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址
	}else 
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));
	} //发送器件地址 0XA0,写数据
	IIC_wait_ack(); 
	IIC_Send_Byte(WriteAddr%256); //发送低地址
	IIC_wait_ack(); 
	IIC_Send_Byte(DataToWrite); //发送字节 
	IIC_wait_ack(); 
	IIC_stop();//
	delay_ms(10); //EEPROM 写入过程比较慢
	//需要等待一点时间，在写下一次
}

// 在AT24CXX里面的指定的地址开始写入长度为len的数据
//该函数用于写入16bit 或者32 bit 的数据
//WriteAddr :开始写入的地址
//DataToWrite:数据数组的首地址
//Len :要写入的数据的长度2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{

	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	} 
}
// 在AT24CXX里面的指定的地址开始读出长度为len的数据
//该函数用于读出16bit 或者32 bit 的数据
//ReadAddr :开始读出的地址
//返回值:数据
//Len :要写入的数据的长度2,4
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{
	u8 t; u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 
	}
	return temp; 
}
//检测AT24CXX 是否正常
//这里用了24XX 的最后一个地址255来储存标志字
//如果使用了其他的24C系列，这儿地址要修改
// 返回1:检测失败
//返回0:检测成功
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//避免每次开机都写AT24CXX 
	if(temp==0X55)return 0; 
	else//排除第一次初始化的情况
	{
		AT24CXX_WriteOneByte(255,0X55);
		temp=AT24CXX_ReadOneByte(255); 
	if(temp==0X55)return 0;
	}
	return 1; 
}
//在AT24CXX 里面指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对于24c02 为 0~255
//pBuffer :
//NumToRead:要读出数据的个数

void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);
		NumToRead--;
	}
}
//在AT24CXX 里面指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对于24c02 为 0~255
//pBuffer :
//NumToWrite:要读出数据的个数
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++; pBuffer++;
	}
}
