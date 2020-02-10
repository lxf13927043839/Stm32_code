#ifndef __KEY_H

#define __KEY_H

#include "stm32f4xx.h"
#include "systick.h"	
#include "sys.h" 

void KEY_init(void);

int key_scanf(int mode);
//add
void key_scan_advanced(u8 mode);


#define key0 GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_9)
#define key1 GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_8)
#define key2 GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_7)
#define key3 GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_6)
//add

//��������
extern u8   keydown_data;    //�������º�ͷ��ص�ֵ
extern u8   keyup_data;      //����̧�𷵻�ֵ
extern u16  key_time;
extern u8   key_tem; 

//����ֵ����
#define KEY0_DATA	  1
#define KEY1_DATA	  2
#define KEY2_DATA	  3
#define KEY3_DATA   4

#endif
