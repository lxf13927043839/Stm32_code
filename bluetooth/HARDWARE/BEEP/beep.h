#ifndef __BEEP_H

#define __BEEP_H
#include "stm32f4xx.h"   
#include "sys.h"


void BEEP_init(void);

#define BEEP PGout(7)	// BEEP

#endif
