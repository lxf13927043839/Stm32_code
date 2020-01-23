#include "dht11.h"


//�������Ϳ�ʼ�����ź�
void dht11_rst(void)
{
	DHT11_IO_OUT(); //����PG9 ���
	DHT11_DQ_OUT=0; //������
	delay_ms(20); //1����������18ms
	DHT11_DQ_OUT=1; //����
	delay_us(30); //2������ 20~40us
}	
//�ȴ�DHT11�Ļ�Ӧ
//����1:δ���DHT11
//����0:����
u8 DHT11_Check(void) 	   
{   
  u8 retry=0;
  DHT11_IO_IN();//��������	 
  while (DHT11_DQ_IN&&retry<100)//DHT11 40~80us
  //3��dht11�����Ȱѵ�ƽ����40-50us
	{
    retry++;
		delay_us(1);
  };	 
  if(retry>=100)return 1;
  else retry=0;
  while (!DHT11_DQ_IN&&retry<100)//DHT11   40~80us
  //4��dht11���ٰѵ�ƽ����40-50us
	{
       retry++; 
  		delay_us(1);
  };
  if(retry>=100)return 1;	    
  return 0;
}

//��ʼ��ȡ����λ һ��λ��һ��λ
u8 DHT11_Read_Bit(void)
{
	u8 retry=0;
	while(DHT11_DQ_IN&&retry<100)//��������͵�ƽ
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	while(!DHT11_DQ_IN&&retry<100)//Ȼ������ߵ�ƽ
	{
		retry++;
		delay_us(1);
	}
	delay_us(40);//�ȴ� 40us
	if(DHT11_DQ_IN)  return 1;
	else return 0;
}
//��ȡһ���ֽ�
u8 DHT11_Read_Byte(void)
{
	u8 i,dat;
	dat=0;
	for (i=0;i<8;i++)
	{
		dat<<=1;
		dat|=DHT11_Read_Bit();
	} 
	return dat;
}

u8 DHT11_Read_Data(u8 *temp,u8 *humi)
{
	u8 buf[5];
	u8 i;
	dht11_rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//��ȡ40��λ  5*8=40
		{
			buf[i]=DHT11_Read_Byte();
		}
		//У���
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0];
			
			
			*temp=buf[2];
		
		}
	}else return 1;
	return 0;
}

//��ʼ���˿�  PA15 ����1 ��������========== ����0 ������
u8 DHT11_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	dht11_rst();
	return DHT11_Check();
}


