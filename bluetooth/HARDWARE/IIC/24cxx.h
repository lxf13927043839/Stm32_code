#ifndef __24CXX_H

#define __24CXX_H

#include "myiic.h"

#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767  
#define EE_TYPE AT24C02

void AT24CXX_init(void);
u8 AT24CXX_ReadOneByte(u16 ReadAddr);
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite);

void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len);
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len);
u8 AT24CXX_Check(void);

void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);

#endif


//const u8 TEXT_Buffer[]={"University of Electronic Science and Technology of China,Zhongshan Institute"};
//const u8 TEXT_Buffer1[]={"16嵌入式B班"};

//#define SIZE sizeof(TEXT_Buffer)
//#define SIZE1 sizeof(TEXT_Buffer1)


//int main(void)
//{ 
//	u8 key;
//	u8 datatemp[SIZE1];
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	//如果使用到延时函数就要初始化系统时钟，否则会卡死在循环中，无关delay.c，使用的是systick.c
//	Systick_init(168);
//	UART1_init(115200);  
//	KEY_init(); 
//	AT24CXX_init(); 
//	
//	while(AT24CXX_Check())
//	{
//		printf("AT24CXX 检测失败\n");
//  	LED0=!LED0;
//	}
//	
//	 while(1)
//	 {
//			key=key_scanf(0);
//			if(key==1)//按键0 写入
//			{
//				
//				AT24CXX_Write(0,(u8*)TEXT_Buffer,SIZE);
//				printf("按键0=成功写入数据\n");
//				
//			}
//			if(key==2)//按键1 读出
//			{
//				AT24CXX_Read(0,datatemp,SIZE);//这里限定了读取多少个字节
//				printf("读取到的数据：%s  \n",datatemp);
//			}
//			if(key==3)//按键2 写入
//			{	
//				AT24CXX_Write(0,(u8*)TEXT_Buffer1,SIZE);
//				printf("按键2=成功写入数据\n");
//			}
//	 }
//}
