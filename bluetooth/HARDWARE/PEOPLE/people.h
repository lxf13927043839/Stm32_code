#ifndef __PEOPLE_H

#define __PEOPLE_H


#include "stm32f4xx.h"
#include "sys.h"
#include "systick.h"

#define	PEOPLE PCin(12)  //���ݶ˿�


void People_init(void);
int People_scan(void);

#endif
