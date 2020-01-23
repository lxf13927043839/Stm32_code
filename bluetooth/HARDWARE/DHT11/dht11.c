#include "dht11.h"


//主机发送开始发送信号
void dht11_rst(void)
{
	DHT11_IO_OUT(); //设置PG9 输出
	DHT11_DQ_OUT=0; //先拉低
	delay_ms(20); //1、至少拉低18ms
	DHT11_DQ_OUT=1; //拉高
	delay_us(30); //2、拉高 20~40us
}	
//等待DHT11的回应
//返回1:未检测DHT11
//返回0:存在
u8 DHT11_Check(void) 	   
{   
  u8 retry=0;
  DHT11_IO_IN();//设置输入	 
  while (DHT11_DQ_IN&&retry<100)//DHT11 40~80us
  //3、dht11，首先把电平拉低40-50us
	{
    retry++;
		delay_us(1);
  };	 
  if(retry>=100)return 1;
  else retry=0;
  while (!DHT11_DQ_IN&&retry<100)//DHT11   40~80us
  //4、dht11，再把电平拉高40-50us
	{
       retry++; 
  		delay_us(1);
  };
  if(retry>=100)return 1;	    
  return 0;
}

//开始读取数据位 一个位、一个位
u8 DHT11_Read_Bit(void)
{
	u8 retry=0;
	while(DHT11_DQ_IN&&retry<100)//首先输出低电平
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	while(!DHT11_DQ_IN&&retry<100)//然后输出高电平
	{
		retry++;
		delay_us(1);
	}
	delay_us(40);//等待 40us
	if(DHT11_DQ_IN)  return 1;
	else return 0;
}
//读取一个字节
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
		for(i=0;i<5;i++)//读取40个位  5*8=40
		{
			buf[i]=DHT11_Read_Byte();
		}
		//校验和
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0];
			
			
			*temp=buf[2];
		
		}
	}else return 1;
	return 0;
}

//初始化端口  PA15 返回1 ：不存在========== 返回0 ：存在
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


