#ifndef USART_H
#define USART_H
#include "string.h"
#include "stdarg.h"
#include "stdio.h"
#define USART3_MAX_RECV_LEN		400

void UART1_init(uint32_t bound);
void UART3_init(uint32_t bound);
void Put_Char(USART_TypeDef* USARTx, uint16_t Data);	
uint16_t Get_Char(USART_TypeDef* USARTx);


void u3_printf(char* fmt,...);  

#endif  
