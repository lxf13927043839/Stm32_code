#ifndef __FT5206_H
#define __FT5206_H	
#include "common.h"	

//////////////////////////////////////////////////////////////////////////////////	 

 
extern u16 x[5]; 	//触摸点屏幕坐标  由于FT5426是5点触控的电容屏 所以这边定义数组成员为5个
extern u16 y[5];
extern u8 Touch_num; //触摸点有效个数  最大5个


//模拟IIC相关的IO口定义 
#define FT_IIC_SCL      PAout(5) 	 //SCL
#define FT_IIC_SDAOUT   PBout(1)   //SDA输出
#define FT_IIC_SDAIN    GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)  //SDA输入 

//其他FT5426相关IO口定义
#define FT_RST    	    PBout(0)	 //FT5426复位引脚
#define FT_INT    		  PFin(11) 	 //FT5426中断引脚	

//FT5426读写命令	
#define FT_WR_CMD 			0X70       //写命令
#define FT_RD_CMD 			0X71	     //读命令
  
//FT5426读取触摸有效点寄存器定义
#define FT_Read_TouchNum_REG      0x02		//触摸状态寄存器

//模拟IIC相关函数
void FT_IIC_Init(void);         //初始化IIC的IO口				 
void FT_IIC_Start(void);				// IIC开始信号
void FT_IIC_Stop(void);	  			// IIC停止信号
void FT_IIC_write_OneByte(u8 send_date);	//IIC写一个字节
u8 FT_IIC_Read_OneByte(u8 ack);	    //IIC读一个字节
u8 FT_MCU_WaitAck(void); 				    //等待ACK信号
void FT_MCU_Send_Ack(void);					//发送ACK信号
void FT_MCU_NOAck(void);					  //不发送ACK信号
 
//FT5426相关函数
void FT5426_WriteReg(u16 FT_reg,u8 date);
u8 FT5426_ReadReg(u16 FT_reg);
void FT5426_Init(void);
void FT5426_Scan(void);   //FT5426触摸扫描函数

#endif



