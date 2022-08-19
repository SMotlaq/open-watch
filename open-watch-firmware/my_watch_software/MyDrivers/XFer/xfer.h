#ifndef __XFER__
#define __XFER__

#include "usart.h"
#include "stdio.h"

#define DEBUG_UART			huart1 
#define BLUETOOTH_UART	huart2

#define DEBUG_ENABLE		0
#define BLE_DEBUG 			1

#if DEBUG_ENABLE
	#include "stdarg.h"
	#include "string.h"
	#include "stdlib.h"
#endif

void rawXFer(uint8_t* data, uint16_t length);
void DEBUG(const char* _str, ...);

#endif
