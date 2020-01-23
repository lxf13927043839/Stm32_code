#include "led.h" 

//初始化 PE3 4 ，PG9 为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG, ENABLE);			//使能GPIOE，GPIOF时钟

  //GPIOE3,E4 初始化设置 
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3 | GPIO_Pin_4;		//LED0和LED1对应IO口
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;			    //普通输出模式，
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//推挽输出，驱动LED需要电流驱动
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		    //100MHz
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				    //上拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);						//初始化GPIOE，把配置的数据写入寄存器
  
  //GPIOG9   初始化设置 
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;	//LED2和LED3对应IO口
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;				//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//100MHz
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;					//上拉
  GPIO_Init(GPIOG, &GPIO_InitStructure);						//初始化GPIOG，把配置的数据写入寄存器
	
  GPIO_SetBits(GPIOE,GPIO_Pin_3  | GPIO_Pin_4);			  //GPIOE3,E4   设置高，灯灭
  GPIO_SetBits(GPIOG,GPIO_Pin_9);				//GPIOG9      设置高，灯灭
}






