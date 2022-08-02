#ifndef __SYSTEM__
#define __SYSTEM__

#include "main.h"
#include "xfer.h"
#include "pitches.h"

#define RX_BUFFER_SIZE 	6
#define TX_BUFFER_SIZE 	150

#define NOTIF_DURATION 5

#define ISCONNECTED_MASK		7
#define ISPSAVING_MASK			6
#define ISPPGENABLED_MASK		5
#define SCREENFLAG_MASK			4
#define SCREENENABLED_MASK	3
#define PEDFLAG_MASK				2
#define VIBENENABLED_MASK		1
#define SOUNDENABLED_MASK		0

enum system_states {ack_waiting, hello, home, bloody_hell, about};

typedef struct System_t{
	
	uint32_t pedometer;
	float pace_size;
	
	uint16_t voltage;
	
	uint8_t heart_beat;
	float SpO2;
	
	RTC_DateTypeDef Date;
	RTC_TimeTypeDef Time;

	// Flags:
	// ------------------------------------------------------------------------------------------------------------------------- //
	// |       7      |       6      |       5      |       4      |       3      |       2      |       1      |       0      | //
	// ------------------------------------------------------------------------------------------------------------------------- //
	// |  isConnected |   isPSaving  | isPPGEnabled |  screenFlag  | screenEnable |    pedFlag   |   vibEnable  |  soundEnable | //
	// ------------------------------------------------------------------------------------------------------------------------- //
	
	uint8_t TxBuffer[TX_BUFFER_SIZE];
	
	uint8_t flags;
	
	int8_t display_collibration_x;
	int8_t display_collibration_y;
	
	const uint8_t notif_melody[NOTIF_DURATION];
	const uint8_t notif_vibs[NOTIF_DURATION];
	
	enum system_states state;
	
	char user_name[18];
	uint8_t name_xpos;
	
} System;


// Constructor
System sys_init(void);

// Get Flags
uint8_t getConnected(System* sys);
uint8_t getPSaving(System* sys);
uint8_t getPPGEnable(System* sys);
uint8_t getScreenFlag(System* sys);
uint8_t getScreenEable(System* sys);
uint8_t getPedFlag(System* sys);
uint8_t getVibEnable(System* sys);
uint8_t getSoundEnable(System* sys);

// SetFlags
void setConnected(System* sys, uint8_t state);
void setPSaving(System* sys, uint8_t state);
void setPPGEnable(System* sys, uint8_t state);
void setScreenFlag(System* sys, uint8_t state);
void setScreenEable(System* sys, uint8_t state);
void setPedFlag(System* sys, uint8_t state);
void setVibEnable(System* sys, uint8_t state);
void setSoundEnable(System* sys, uint8_t state);


// Main functions
void RxParser(System* sys, uint8_t* RxBuffer);

#endif
