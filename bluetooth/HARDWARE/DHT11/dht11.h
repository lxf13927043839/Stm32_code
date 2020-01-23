#ifndef __DHT11_H

#define __DHT11_H

#include "stm32f4xx.h"
#include "sys.h"
#include "systick.h"

//IO方向设置
#define  DHT11_IO_IN()  {GPIOA->MODER&=~(3<<(15*2));GPIOA->MODER|=0<<15*2;}	//PA15   设置输入
#define DHT11_IO_OUT()  {GPIOA->MODER&=~(3<<(15*2));GPIOA->MODER|=1<<15*2;} 	//PA15 设置输出						  

#define	DHT11_DQ_OUT PAout(15) //数据端口	PA15
#define	DHT11_DQ_IN  PAin(15)  //数据端口	PA15 

void dht11_rst(void);

u8 DHT11_Check(void);

u8 DHT11_Read_Bit(void);

u8 DHT11_Read_Byte(void);

u8 DHT11_Read_Data(u8 *temp,u8 *humi);

u8 DHT11_init(void);

#endif
