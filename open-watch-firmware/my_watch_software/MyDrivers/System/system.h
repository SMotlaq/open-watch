#ifndef __MAIN_H
	#include "main.h"
	#define __MAIN_H
#endif

#include "pitches.h"

#define RX_BUFFER_SIZE 	40
#define TX_BUFFER_SIZE 	50

#define NOTIF_DURATION 5

typedef struct{
	
	uint32_t pedometer;
	
	float battery;
	float SpO2;
	
	RTC_DateTypeDef Date;
	RTC_TimeTypeDef Time;
	
	uint8_t flags;

	// Flags:
	// ------------------------------------------------------------------------------------------------------------------------- //
	// |       7      |       6      |       5      |       4      |       3      |       2      |       1      |       0      | //
	// ------------------------------------------------------------------------------------------------------------------------- //
	// |  isCharging  |   isPSaving  | isPPGEnabled |  screenFlag  | screenEnable |    pedFlag   |   vibEnable  |  soundEnable | //
	// ------------------------------------------------------------------------------------------------------------------------- //
	
	
	uint8_t _TxBuffer[TX_BUFFER_SIZE];
	uint8_t _RxBuffer[RX_BUFFER_SIZE];
	
	uint8_t heart_beat;
	
	const uint8_t notif_melody[NOTIF_DURATION];
	const uint8_t notif_vibs[NOTIF_DURATION];
	
}System;


// Constructor
System init(void);

// Get Flags
uint8_t getCharging(System* sys);
uint8_t getPSaving(System* sys);
uint8_t getPPGEnable(System* sys);
uint8_t getScreenFlag(System* sys);
uint8_t getScreenEable(System* sys);
uint8_t getPedFlag(System* sys);
uint8_t getVibEnable(System* sys);
uint8_t getSoundEnable(System* sys);

// SetFlags
void setCharging(System* sys, uint8_t state);
void setPSaving(System* sys, uint8_t state);
void setPPGEnable(System* sys, uint8_t state);
void setScreenFlag(System* sys, uint8_t state);
void setScreenEable(System* sys, uint8_t state);
void setPedFlag(System* sys, uint8_t state);
void setVibEnable(System* sys, uint8_t state);
void setSoundEnable(System* sys, uint8_t state);
