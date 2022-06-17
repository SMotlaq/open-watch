#ifndef __MAIN_H
	#include "main.h"
	#define __MAIN_H
#endif

typedef struct{
	float battery;
	uint32_t saving_mode;
	
	RTC_DateTypeDef Date;
	RTC_TimeTypeDef Time;
}System;

