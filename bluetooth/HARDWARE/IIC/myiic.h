#ifndef __MYIIC_H

#define __MYIIC_H
#include "stm32f4xx.h"
#include "sys.h"
#include "systick.h"

//ÅäÖÃÊäÈë
#define SDA_IN() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}
//ÅäÖÃÊä³ö
#define SDA_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;}

#define IIC_SCL PBout(8) //SCL
#define IIC_SDA PBout(9) //SDA
#define READ_SDA PBin(9) //ÊäÈëSDA

void IIC_init(void);
void IIC_start(void);
void IIC_stop(void);
u8 IIC_wait_ack(void);
void IIC_ack(void);
void IIC_nack(void);
void IIC_Send_Byte(u8 txd);
u8 IIC_Read_Byte(unsigned char ack);

#endif



