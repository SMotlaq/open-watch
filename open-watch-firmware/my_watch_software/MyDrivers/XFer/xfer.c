#include "xfer.h"

void DEBUG(const char* _str, ...){
	#if DEBUG_ENABLE
		va_list args;
		va_start(args, _str);
		char buffer[50];
		memset(buffer, 0, 50);
		int buffer_size = vsprintf(buffer, _str, args);
		#if BLE_DEBUG
			HAL_UART_Transmit(&BLUETOOTH_UART, (uint8_t*)buffer, buffer_size, 1000);
		#else
			HAL_UART_Transmit(&DEBUG_UART, (uint8_t*)buffer, buffer_size, 5000);
		#endif
	#endif
}
