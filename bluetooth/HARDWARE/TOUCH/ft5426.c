#include "ft5426.h"
#include "xpt2046.h"
#include "touch.h"
#include "string.h" 
#include "lcd.h"
#include "usart1.h"

/*********************************************************************************
*********************启明欣欣 STM32F407应用开发板(高配版)*************************
**********************************************************************************
* 文件名称: ft5426.c                                                             *
* 文件简述：电容屏触摸驱动程序                                                   *
* 创建日期：2018.08.30                                                           *
* 版    本：V1.0                                                                 *
* 作    者：Clever                                                               *
* 说    明：                                                                     * 
**********************************************************************************
*********************************************************************************/
	 
// 0X03	  //第一个触摸点数据地址
// 0X09		//第二个触摸点数据地址
// 0X0F		//第三个触摸点数据地址
// 0X15		//第四个触摸点数据地址
// 0X1B		//第五个触摸点数据地址 
const u16 FT5426_Touch_Addr[5]={0X03,0X09,0X0F,0X15,0X1B};
const u16 Touch_Point_COLOR[5]={RED,BLUE,BLACK,GREEN,YELLOW}; //5点触控对应颜色值 
 

/*******************************************************************************
*************************以下为IO口模拟IIC通信**********************************
*******************************************************************************/
//FT5426芯片IIC接口初始化
void FT_IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;	
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);         //初始化
		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;      
	GPIO_Init(GPIOB, &GPIO_InitStructure); 

	 FT_IIC_Stop();
}

//配置SDA数据线为输入
void FT_IICSDA_IN(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //输入模式
	GPIO_Init(GPIOB, &GPIO_InitStructure);      
}	

//配置SDA数据线为输出
void FT_IICSDA_OUT()
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出模式
	GPIO_Init(GPIOB, &GPIO_InitStructure);  
}

//IIC起始信号
void FT_IIC_Start(void)
{
	FT_IICSDA_OUT();      
	FT_IIC_SDAOUT=1;	  	  
	FT_IIC_SCL=1;
	delay_us(28);
 	FT_IIC_SDAOUT=0; 
	delay_us(2);
	FT_IIC_SCL=0; 
}	 

//IIC停止信号
void FT_IIC_Stop(void)
{
	FT_IICSDA_OUT(); 
	FT_IIC_SCL=1;
	delay_us(28);
	FT_IIC_SDAOUT=0; 
  delay_us(2);
	FT_IIC_SDAOUT=1; 
}

/****************************************************************************
* 名    称: u8 FT_MCU_WaitAck(void)
* 功    能：等待应答信号
* 入口参数：无
* 返回参数: 1：应答失败
            0：应答成功  
* 说    明：       
****************************************************************************/ 
u8 FT_MCU_WaitAck(void)
{
	u8 ack=0;
	
	FT_IICSDA_IN();       
	FT_IIC_SDAOUT=1;	   
	FT_IIC_SCL=1;
	delay_us(2);
	if(FT_IIC_SDAIN)
	{
    ack = 1;
	}
	else
	{
	  ack = 0;
	}
	FT_IIC_SCL=0; 
	delay_us(2);
	return ack;  
} 

//产生ACK应答
void FT_MCU_Send_Ack(void)
{
	FT_IIC_SCL=0;
	FT_IICSDA_OUT();
	delay_us(2);
	FT_IIC_SDAOUT=0;
	delay_us(2);
	FT_IIC_SCL=1;
	delay_us(2);
	FT_IIC_SCL=0;
}

//不产生ACK应答		    
void FT_MCU_NOAck(void)
{
	FT_IIC_SCL=0;
	FT_IICSDA_OUT();
	delay_us(2);
	FT_IIC_SDAOUT=1;
	delay_us(2);
	FT_IIC_SCL=1;
	delay_us(2);
	FT_IIC_SCL=0;
}	

/****************************************************************************
* 名    称: void FT_IIC_write_OneByte(u8 send_date)
* 功    能：IIC写一个字节
* 入口参数：send_date：发送的数据
* 返回参数: 1：有应答 
            0：无应答  
* 说    明：       
****************************************************************************/ 	  
void FT_IIC_write_OneByte(u8 send_date)
{                        
  u8 i;   

	FT_IICSDA_OUT(); 	      
	FT_IIC_SCL=0;  
	delay_us(2);
	for(i=0;i<8;i++)
    {              
      FT_IIC_SDAOUT=(send_date&0x80)>>7;
      send_date<<=1; 	      
		  FT_IIC_SCL=1; 
		  delay_us(2);
		  FT_IIC_SCL=0;	
		  delay_us(2);
    }	 
} 

/****************************************************************************
* 名    称: void FT_IIC_write_OneByte(u8 send_date)
* 功    能：IIC读一个字节
* 入口参数：ack：1：发送ACK
                 0：发送nACK 
* 返回参数: 读到的数据 
* 说    明：       
****************************************************************************/ 
u8 FT_IIC_Read_OneByte(u8 ack)
{
	u8 i;
	u8 receivedata=0;
 	
	FT_IICSDA_IN(); 
	delay_us(28);
	for(i=0;i<8;i++ )
	{ 
		FT_IIC_SCL=0; 	    	   
		delay_us(2);
		FT_IIC_SCL=1;	 
		receivedata<<=1;
		if(FT_IIC_SDAIN)
			 receivedata++;   
	}	  				 
	if(!ack) 
		 FT_MCU_NOAck();  
	else 
		 FT_MCU_Send_Ack();  
 	return receivedata;
}
/*******************************IO口模拟IIC*************************************
*******************************************************************************/


/****************************************************************************
* 名    称: void FT5426_WriteReg(u16 FT_reg,u8 date)
* 功    能：向FT5426相应寄存器写入数据
* 入口参数：FT_reg：寄存器
            date  ：所写数据
* 返回参数: 无
* 说    明：       
****************************************************************************/
void FT5426_WriteReg(u16 FT_reg,u8 date)
{
	FT_IIC_Start();	 
	FT_IIC_write_OneByte(FT_WR_CMD);	 //写命令 	 
	FT_MCU_WaitAck(); 	 										  		   
	FT_IIC_write_OneByte(FT_reg&0XFF); //低8位地址
	FT_MCU_WaitAck();     
  FT_IIC_write_OneByte(date);        //发数据
	FT_MCU_WaitAck();
  FT_IIC_Stop();					  
}

/****************************************************************************
* 名    称: u8 FT5426_ReadReg(u16 FT_reg)
* 功    能：读出FT5426相应寄存器中的数据
* 入口参数：FT_reg：寄存器
* 返回参数: 读出的数据
* 说    明：       
****************************************************************************/		  
u8 FT5426_ReadReg(u16 FT_reg)
{
	u8 date; 
	
 	FT_IIC_Start();	
 	FT_IIC_write_OneByte(FT_WR_CMD);   	 //写命令 	 
	FT_MCU_WaitAck(); 	 										  		   
 	FT_IIC_write_OneByte(FT_reg&0XFF);   //低8位地址
	FT_MCU_WaitAck();  
 	FT_IIC_Start();  	 	   
	FT_IIC_write_OneByte(FT_RD_CMD);   	 //读命令		   
	FT_MCU_WaitAck();	      
  date=FT_IIC_Read_OneByte(0);         //发数据	  
  FT_IIC_Stop();  
	return date;
} 

/****************************************************************************
* 名    称: void Read_Touch_Point(u16 addr,u8 *pBuffer)
* 功    能：读出触摸点的4个寄存器数据
* 入口参数：addr：触摸点首个寄存器 
            pBuffer：存放读出来的四个寄存器数据
* 返回参数: 无
* 说    明：       
****************************************************************************/	
void Read_Touch_Point(u16 addr,u8 *pBuffer)
{
 u8 i;
	for(i=0;i<4;i++)
	{
	  *pBuffer++=FT5426_ReadReg(addr+i);
	}
}

//初始化FT5426
void FT5426_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOF, ENABLE); 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;     //输入模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   //推挽 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;     //上拉
  GPIO_Init(GPIOF, &GPIO_InitStructure); 
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;    //输出 
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	FT_IIC_Init();   
	
	FT_RST=0;				 //复位	
	delay_ms(25);
 	FT_RST=1;				    
	delay_ms(50);  	

  FT5426_WriteReg(0x00,0);	  //工作状态设置 0:进入正常工作模式
	FT5426_WriteReg(0xA4,1);		//1:中断模式 	 0:查询模式					 
	FT5426_WriteReg(0x80,18);	  //触摸有效值设置为18	越小越灵敏						
	FT5426_WriteReg(0x88,12);   //激活周期 不能小于12,最大14
}

u8 Touch_num;  //触摸点有效个数  最大5个
/****************************************************************************
* 名    称: void FT5426_Scan(void)
* 功    能：FT5426触摸扫描函数
* 入口参数：无
* 返回参数: 无
* 说    明：       
****************************************************************************/ 
void FT5426_Scan(void)
{
	u8 touch_buf[4];
	u8 i=0;
	u8 j=0;
  u8 num;
	
	if(FT_INT==0)  //有触摸按下
	{
		Touch_num=FT5426_ReadReg(FT_Read_TouchNum_REG); //读取有效触摸点的个数
    num=Touch_num;
		
		for(i=0;i<num;i++)
			{				
				Read_Touch_Point(FT5426_Touch_Addr[i],touch_buf);

				if(dir_flag==1)//横屏
					{
						x[i]=((u16)(touch_buf[0]&0X0F)<<8)+touch_buf[1];
						y[i]=((u16)(touch_buf[2]&0X0F)<<8)+touch_buf[3];						
					}else        //竖屏
					{
						x[i]=480-(((u16)(touch_buf[2]&0X0F)<<8)+touch_buf[3]);
						y[i]=((u16)(touch_buf[0]&0X0F)<<8)+touch_buf[1];
					} 
        	Draw_Point(x[i],y[i],Touch_Point_COLOR[i]);		//画板	
			}		
	}
	else  //触摸抬起清空屏幕坐标  如读者需要触摸抬起后的有效值 可参考xpt2046那样操作
	{                    //也可根据读出触摸点的首个寄存器的高两位判断,具体参看FT手册
   for(j=0;j<num;j++)
		{
			x[j]=0xffff;
			y[j]=0xffff;
		}
	}	 
}






 



























