#include "24cxx.h"

void AT24CXX_init(void)
{
	IIC_init();//IIC ��ʼ��

}
//ָ����ַ����һ������
//readaddr ��ʼ�����ĵ�ַ
//����ֵ������������
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{ 
	u8 temp=0;
	IIC_start();

	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0); //����д����
		IIC_wait_ack();
		IIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ
	}else
	{
		IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));
	} //����������ַ 0XA0,д����
	IIC_wait_ack();
	IIC_Send_Byte(ReadAddr%256); //���͵͵�ַ
	IIC_wait_ack(); 
	IIC_start(); 
	IIC_Send_Byte(0XA1); //�������ģʽ
	IIC_wait_ack();
	temp=IIC_Read_Byte(0); 
	IIC_stop();
	return temp;
}
//��AT24CXX ָ����ַд��һ������
//WriteAddr :д�����ݵ�Ŀ�ĵ�ַ
//DataToWrite:Ҫд�������
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{
	IIC_start();
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0); //����д����
		IIC_wait_ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
	}else 
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));
	} //����������ַ 0XA0,д����
	IIC_wait_ack(); 
	IIC_Send_Byte(WriteAddr%256); //���͵͵�ַ
	IIC_wait_ack(); 
	IIC_Send_Byte(DataToWrite); //�����ֽ� 
	IIC_wait_ack(); 
	IIC_stop();//
	delay_ms(10); //EEPROM д����̱Ƚ���
	//��Ҫ�ȴ�һ��ʱ�䣬��д��һ��
}

// ��AT24CXX�����ָ���ĵ�ַ��ʼд�볤��Ϊlen������
//�ú�������д��16bit ����32 bit ������
//WriteAddr :��ʼд��ĵ�ַ
//DataToWrite:����������׵�ַ
//Len :Ҫд������ݵĳ���2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{

	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	} 
}
// ��AT24CXX�����ָ���ĵ�ַ��ʼ��������Ϊlen������
//�ú������ڶ���16bit ����32 bit ������
//ReadAddr :��ʼ�����ĵ�ַ
//����ֵ:����
//Len :Ҫд������ݵĳ���2,4
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
//���AT24CXX �Ƿ�����
//��������24XX �����һ����ַ255�������־��
//���ʹ����������24Cϵ�У������ַҪ�޸�
// ����1:���ʧ��
//����0:���ɹ�
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//����ÿ�ο�����дAT24CXX 
	if(temp==0X55)return 0; 
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(255,0X55);
		temp=AT24CXX_ReadOneByte(255); 
	if(temp==0X55)return 0;
	}
	return 1; 
}
//��AT24CXX ����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ����24c02 Ϊ 0~255
//pBuffer :
//NumToRead:Ҫ�������ݵĸ���

void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);
		NumToRead--;
	}
}
//��AT24CXX ����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ����24c02 Ϊ 0~255
//pBuffer :
//NumToWrite:Ҫ�������ݵĸ���
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++; pBuffer++;
	}
}
