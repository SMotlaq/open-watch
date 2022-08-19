#ifndef __SYSTEM__
#define __SYSTEM__

#include "main.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "rtc.h"
#include "xfer.h"

#define RX_BUFFER_SIZE 	21
#define TX_BUFFER_SIZE 	(200*4+11)

#define NOTIF_DURATION 5

#define ISCONNECTED_MASK		7
#define ISRINING_MASK				6
#define ISSHRREACHED_MASK		5
#define SCREENFLAG_MASK			4
#define SCREENENABLED_MASK	3
#define PEDFLAG_MASK				2
#define VIBENENABLED_MASK		1
#define SOUNDENABLED_MASK		0

enum system_states {ack_waiting, hello, home, bloody_hell, pedomedo, about, ringing};

typedef struct System_t{
	
	uint32_t pedometer;
	double pace_size;
	
	uint16_t voltage;
	uint8_t battery;
	
	uint16_t ppg_data_address_pointer;
	uint8_t heart_beat;
	float SpO2;
	
	RTC_DateTypeDef Date;
	RTC_TimeTypeDef Time;
	uint8_t isAlarmEnabled;
	
	uint8_t RxBuffer[RX_BUFFER_SIZE];
	uint8_t TxBuffer[TX_BUFFER_SIZE];
	
	uint8_t flags;
	// Flags:
	// ------------------------------------------------------------------------------------------------------------------------- //
	// |       7      |       6      |       5      |       4      |       3      |       2      |       1      |       0      | //
	// ------------------------------------------------------------------------------------------------------------------------- //
	// |  isConnected |   isRining   | isSHRreached |  screenFlag  | screenEnable |    pedFlag   |   vibEnable  |  soundEnable | //
	// ------------------------------------------------------------------------------------------------------------------------- //
	
	
	int8_t display_collibration_x;
	int8_t display_collibration_y;
	
	enum system_states state;
	
	char user_name[18];
	uint8_t name_xpos;
	
	uint8_t ack;
	
} System;


// Constructor
System sys_init(void);

// Get Flags
uint8_t getConnected(System* sys);
uint8_t getRinging(System* sys);
uint8_t getSHRreached(System* sys);
uint8_t getScreenFlag(System* sys);
uint8_t getScreenEable(System* sys);
uint8_t getPedFlag(System* sys);
uint8_t getVibEnable(System* sys);
uint8_t getSoundEnable(System* sys);

// SetFlags
void setConnected(System* sys, uint8_t state);
void setRinging(System* sys, uint8_t state);
void setSHRreached(System* sys, uint8_t state);
void setScreenFlag(System* sys, uint8_t state);
void setScreenEable(System* sys, uint8_t state);
void setPedFlag(System* sys, uint8_t state);
void setVibEnable(System* sys, uint8_t state);
void setSoundEnable(System* sys, uint8_t state);


// Main functions
void RxParser(System* sys);
void TxGenerator(System* sys);
void addAck2buffer(System* sys);
void addPedometer2buffer(System* sys);
void addBattery2buffer(System* sys);
void clear_PPG_buffer(System* sys);
void append_PPG_buffer(System* sys, uint32_t ir, uint32_t red);
void setAlarm(uint8_t Hours, uint8_t Minutes, uint8_t Seconds, uint8_t weekday, uint32_t masks);
float map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max);

#endif
