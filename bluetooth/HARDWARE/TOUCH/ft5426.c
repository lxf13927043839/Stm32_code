#include "ft5426.h"
#include "xpt2046.h"
#include "touch.h"
#include "string.h" 
#include "lcd.h"
#include "usart1.h"

/*********************************************************************************
*********************�������� STM32F407Ӧ�ÿ�����(�����)*************************
**********************************************************************************
* �ļ�����: ft5426.c                                                             *
* �ļ�������������������������                                                   *
* �������ڣ�2018.08.30                                                           *
* ��    ����V1.0                                                                 *
* ��    �ߣ�Clever                                                               *
* ˵    ����                                                                     * 
**********************************************************************************
*********************************************************************************/
	 
// 0X03	  //��һ�����������ݵ�ַ
// 0X09		//�ڶ������������ݵ�ַ
// 0X0F		//���������������ݵ�ַ
// 0X15		//���ĸ����������ݵ�ַ
// 0X1B		//��������������ݵ�ַ 
const u16 FT5426_Touch_Addr[5]={0X03,0X09,0X0F,0X15,0X1B};
const u16 Touch_Point_COLOR[5]={RED,BLUE,BLACK,GREEN,YELLOW}; //5�㴥�ض�Ӧ��ɫֵ 
 

/*******************************************************************************
*************************����ΪIO��ģ��IICͨ��**********************************
*******************************************************************************/
//FT5426оƬIIC�ӿڳ�ʼ��
void FT_IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;	
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //����
  GPIO_Init(GPIOA, &GPIO_InitStructure);         //��ʼ��
		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;      
	GPIO_Init(GPIOB, &GPIO_InitStructure); 

	 FT_IIC_Stop();
}

//����SDA������Ϊ����
void FT_IICSDA_IN(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //����ģʽ
	GPIO_Init(GPIOB, &GPIO_InitStructure);      
}	

//����SDA������Ϊ���
void FT_IICSDA_OUT()
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���ģʽ
	GPIO_Init(GPIOB, &GPIO_InitStructure);  
}

//IIC��ʼ�ź�
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

//IICֹͣ�ź�
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
* ��    ��: u8 FT_MCU_WaitAck(void)
* ��    �ܣ��ȴ�Ӧ���ź�
* ��ڲ�������
* ���ز���: 1��Ӧ��ʧ��
            0��Ӧ��ɹ�  
* ˵    ����       
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

//����ACKӦ��
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

//������ACKӦ��		    
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
* ��    ��: void FT_IIC_write_OneByte(u8 send_date)
* ��    �ܣ�IICдһ���ֽ�
* ��ڲ�����send_date�����͵�����
* ���ز���: 1����Ӧ�� 
            0����Ӧ��  
* ˵    ����       
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
* ��    ��: void FT_IIC_write_OneByte(u8 send_date)
* ��    �ܣ�IIC��һ���ֽ�
* ��ڲ�����ack��1������ACK
                 0������nACK 
* ���ز���: ���������� 
* ˵    ����       
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
/*******************************IO��ģ��IIC*************************************
*******************************************************************************/


/****************************************************************************
* ��    ��: void FT5426_WriteReg(u16 FT_reg,u8 date)
* ��    �ܣ���FT5426��Ӧ�Ĵ���д������
* ��ڲ�����FT_reg���Ĵ���
            date  ����д����
* ���ز���: ��
* ˵    ����       
****************************************************************************/
void FT5426_WriteReg(u16 FT_reg,u8 date)
{
	FT_IIC_Start();	 
	FT_IIC_write_OneByte(FT_WR_CMD);	 //д���� 	 
	FT_MCU_WaitAck(); 	 										  		   
	FT_IIC_write_OneByte(FT_reg&0XFF); //��8λ��ַ
	FT_MCU_WaitAck();     
  FT_IIC_write_OneByte(date);        //������
	FT_MCU_WaitAck();
  FT_IIC_Stop();					  
}

/****************************************************************************
* ��    ��: u8 FT5426_ReadReg(u16 FT_reg)
* ��    �ܣ�����FT5426��Ӧ�Ĵ����е�����
* ��ڲ�����FT_reg���Ĵ���
* ���ز���: ����������
* ˵    ����       
****************************************************************************/		  
u8 FT5426_ReadReg(u16 FT_reg)
{
	u8 date; 
	
 	FT_IIC_Start();	
 	FT_IIC_write_OneByte(FT_WR_CMD);   	 //д���� 	 
	FT_MCU_WaitAck(); 	 										  		   
 	FT_IIC_write_OneByte(FT_reg&0XFF);   //��8λ��ַ
	FT_MCU_WaitAck();  
 	FT_IIC_Start();  	 	   
	FT_IIC_write_OneByte(FT_RD_CMD);   	 //������		   
	FT_MCU_WaitAck();	      
  date=FT_IIC_Read_OneByte(0);         //������	  
  FT_IIC_Stop();  
	return date;
} 

/****************************************************************************
* ��    ��: void Read_Touch_Point(u16 addr,u8 *pBuffer)
* ��    �ܣ������������4���Ĵ�������
* ��ڲ�����addr���������׸��Ĵ��� 
            pBuffer����Ŷ��������ĸ��Ĵ�������
* ���ز���: ��
* ˵    ����       
****************************************************************************/	
void Read_Touch_Point(u16 addr,u8 *pBuffer)
{
 u8 i;
	for(i=0;i<4;i++)
	{
	  *pBuffer++=FT5426_ReadReg(addr+i);
	}
}

//��ʼ��FT5426
void FT5426_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOF, ENABLE); 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;     //����ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   //���� 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;     //����
  GPIO_Init(GPIOF, &GPIO_InitStructure); 
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;    //��� 
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	FT_IIC_Init();   
	
	FT_RST=0;				 //��λ	
	delay_ms(25);
 	FT_RST=1;				    
	delay_ms(50);  	

  FT5426_WriteReg(0x00,0);	  //����״̬���� 0:������������ģʽ
	FT5426_WriteReg(0xA4,1);		//1:�ж�ģʽ 	 0:��ѯģʽ					 
	FT5426_WriteReg(0x80,18);	  //������Чֵ����Ϊ18	ԽСԽ����						
	FT5426_WriteReg(0x88,12);   //�������� ����С��12,���14
}

u8 Touch_num;  //��������Ч����  ���5��
/****************************************************************************
* ��    ��: void FT5426_Scan(void)
* ��    �ܣ�FT5426����ɨ�躯��
* ��ڲ�������
* ���ز���: ��
* ˵    ����       
****************************************************************************/ 
void FT5426_Scan(void)
{
	u8 touch_buf[4];
	u8 i=0;
	u8 j=0;
  u8 num;
	
	if(FT_INT==0)  //�д�������
	{
		Touch_num=FT5426_ReadReg(FT_Read_TouchNum_REG); //��ȡ��Ч������ĸ���
    num=Touch_num;
		
		for(i=0;i<num;i++)
			{				
				Read_Touch_Point(FT5426_Touch_Addr[i],touch_buf);

				if(dir_flag==1)//����
					{
						x[i]=((u16)(touch_buf[0]&0X0F)<<8)+touch_buf[1];
						y[i]=((u16)(touch_buf[2]&0X0F)<<8)+touch_buf[3];						
					}else        //����
					{
						x[i]=480-(((u16)(touch_buf[2]&0X0F)<<8)+touch_buf[3]);
						y[i]=((u16)(touch_buf[0]&0X0F)<<8)+touch_buf[1];
					} 
        	Draw_Point(x[i],y[i],Touch_Point_COLOR[i]);		//����	
			}		
	}
	else  //����̧�������Ļ����  �������Ҫ����̧������Чֵ �ɲο�xpt2046��������
	{                    //Ҳ�ɸ��ݶ�����������׸��Ĵ����ĸ���λ�ж�,����ο�FT�ֲ�
   for(j=0;j<num;j++)
		{
			x[j]=0xffff;
			y[j]=0xffff;
		}
	}	 
}






 



























