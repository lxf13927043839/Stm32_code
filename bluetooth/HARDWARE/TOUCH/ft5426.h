#ifndef __FT5206_H
#define __FT5206_H	
#include "common.h"	

//////////////////////////////////////////////////////////////////////////////////	 

 
extern u16 x[5]; 	//��������Ļ����  ����FT5426��5�㴥�صĵ����� ������߶��������ԱΪ5��
extern u16 y[5];
extern u8 Touch_num; //��������Ч����  ���5��


//ģ��IIC��ص�IO�ڶ��� 
#define FT_IIC_SCL      PAout(5) 	 //SCL
#define FT_IIC_SDAOUT   PBout(1)   //SDA���
#define FT_IIC_SDAIN    GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)  //SDA���� 

//����FT5426���IO�ڶ���
#define FT_RST    	    PBout(0)	 //FT5426��λ����
#define FT_INT    		  PFin(11) 	 //FT5426�ж�����	

//FT5426��д����	
#define FT_WR_CMD 			0X70       //д����
#define FT_RD_CMD 			0X71	     //������
  
//FT5426��ȡ������Ч��Ĵ�������
#define FT_Read_TouchNum_REG      0x02		//����״̬�Ĵ���

//ģ��IIC��غ���
void FT_IIC_Init(void);         //��ʼ��IIC��IO��				 
void FT_IIC_Start(void);				// IIC��ʼ�ź�
void FT_IIC_Stop(void);	  			// IICֹͣ�ź�
void FT_IIC_write_OneByte(u8 send_date);	//IICдһ���ֽ�
u8 FT_IIC_Read_OneByte(u8 ack);	    //IIC��һ���ֽ�
u8 FT_MCU_WaitAck(void); 				    //�ȴ�ACK�ź�
void FT_MCU_Send_Ack(void);					//����ACK�ź�
void FT_MCU_NOAck(void);					  //������ACK�ź�
 
//FT5426��غ���
void FT5426_WriteReg(u16 FT_reg,u8 date);
u8 FT5426_ReadReg(u16 FT_reg);
void FT5426_Init(void);
void FT5426_Scan(void);   //FT5426����ɨ�躯��

#endif



