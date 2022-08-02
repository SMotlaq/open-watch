#include "usart.h"
#include "stdio.h"

#define DEBUG_UART			huart1 
#define BLUETOOTH_UART	huart2

#define DEBUG_ENABLE		1
#define BLE_DEBUG 			1

#if DEBUG_ENABLE
	#include "stdarg.h"
	#include "string.h"
	#include "stdlib.h"
#endif

void DEBUG(const char* _str, ...);
