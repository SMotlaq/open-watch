#include "system.h"

// Packet keys
const static uint8_t connection_key[19]				= {0xb6, 0x91, 0xdf, 0x92 ,0x9a ,0x92 ,0x90 ,0x8d ,0x86 ,0xdf ,0x90 ,0x99 ,0xdf ,0x92 ,0x86 ,0xdf ,0x9e ,0x88 ,0x8d};
const static uint8_t system_heartbeat_key[19]	= {0x96, 0x87, 0x9e ,0xd1 ,0xab ,0x97 ,0x9a ,0xdf ,0x93 ,0x90 ,0x8c ,0x8b ,0xdf ,0x97 ,0x9a ,0x9e ,0x8d ,0x8b ,0xd1};

uint32_t __masks = RTC_ALARMMASK_NONE;
	
System sys_init(){
	System sys;

	sys.pedometer = 0;
	sys.pace_size = 0.00006;
	
	sys.voltage = 0;
	sys.battery = 0;
	
	sys.ppg_data_address_pointer = 0;
	sys.heart_beat = 0;
	sys.SpO2 = 0;
	
	sys.isAlarmEnabled = 0;
	
	sys.flags = 0;
	sys.flags = (0 << ISCONNECTED_MASK) 	|
							(0 << ISRINING_MASK)			| 
							(0 << ISSHRREACHED_MASK)	| 
							(0 << SCREENFLAG_MASK)		| 
							(1 << SCREENENABLED_MASK)	|
							(0 << PEDFLAG_MASK)				| 
							(0 << VIBENENABLED_MASK)	| 
							(1 << SOUNDENABLED_MASK);
	
	sys.display_collibration_x = 2;
	sys.display_collibration_y = -2;
	
	sys.state = hello;
	
	sprintf(sys.user_name, "Salman");
	sys.name_xpos = 40;
	
	sys.ack = 0;
	
	return sys;
}

// Get Flags
uint8_t getConnected(System* sys){
	return (sys->flags >> ISCONNECTED_MASK) & 0x01;
}

uint8_t getRinging(System* sys){
	return (sys->flags >> ISRINING_MASK) & 0x01;
}

uint8_t getSHRreached(System* sys){
	return (sys->flags >> ISSHRREACHED_MASK) & 0x01;
}

uint8_t getScreenFlag(System* sys){
	return (sys->flags >> SCREENFLAG_MASK) & 0x01;
}

uint8_t getScreenEable(System* sys){
	return (sys->flags >> SCREENENABLED_MASK) & 0x01;
}

uint8_t getPedFlag(System* sys){
	return (sys->flags >> PEDFLAG_MASK) & 0x01;
}

uint8_t getVibEnable(System* sys){
	return (sys->flags >> VIBENENABLED_MASK) & 0x01;
}

uint8_t getSoundEnable(System* sys){
	return (sys->flags >> SOUNDENABLED_MASK) & 0x01;
} 

// SetFlags
void setConnected(System* sys, uint8_t state){
	sys->flags = state ? (sys->flags | (1 << ISCONNECTED_MASK)) : (sys->flags & ~(1<<ISCONNECTED_MASK));
}

void setRinging(System* sys, uint8_t state){
	sys->flags = state ? (sys->flags | (1 << ISRINING_MASK)) : (sys->flags & ~(1<<ISRINING_MASK));
}

void setSHRreached(System* sys, uint8_t state){
	sys->flags = state ? (sys->flags | (1 << ISSHRREACHED_MASK)) : (sys->flags & ~(1<<ISSHRREACHED_MASK));
}

void setScreenFlag(System* sys, uint8_t state){
	sys->flags = state ? (sys->flags | (1 << SCREENFLAG_MASK)) : (sys->flags & ~(1<<SCREENFLAG_MASK));
}

void setScreenEable(System* sys, uint8_t state){
	sys->flags = state ? (sys->flags | (1 << SCREENENABLED_MASK)) : (sys->flags & ~(1<<SCREENENABLED_MASK));
}

void setPedFlag(System* sys, uint8_t state){
	sys->flags = state ? (sys->flags | (1 << PEDFLAG_MASK)) : (sys->flags & ~(1<<PEDFLAG_MASK));
}

void setVibEnable(System* sys, uint8_t state){
	sys->flags = state ? (sys->flags | (1 << VIBENENABLED_MASK)) : (sys->flags & ~(1<<VIBENENABLED_MASK));
}

void setSoundEnable(System* sys, uint8_t state){
	sys->flags = state ? (sys->flags | (1 << SOUNDENABLED_MASK)) : (sys->flags & ~(1<<SOUNDENABLED_MASK));
}

// Main functions --------------------------------------------------------- 

void RxParser(System* sys){
	
	// Copy buffer to prevent it from changes while parsing
	uint8_t RxBuffer[RX_BUFFER_SIZE];
	memcpy(RxBuffer, sys->RxBuffer, RX_BUFFER_SIZE);
	
	// Extracting packet type
	uint8_t packet_type = RxBuffer[0];
	
	// Validation
	if(packet_type == RxBuffer[20]){
		
		// Parsing
		switch (packet_type)
    {
    	case 0xA0:
					for(int i=0; i<19; i++){
						if(RxBuffer[i+1] != connection_key[i])
							break;
					}
					sys->state = ack_waiting;
					setScreenEable(sys, 1);
    		break;
    	case 0xB1:
					for(int i=0; i<18; i++){
						sys->user_name[i] = RxBuffer[i+1];
					}
					sys->name_xpos = RxBuffer[19];
					sys->state = hello;
					setScreenEable(sys, 1);
    		break;
			case 0xC2:
					sys->Date.Year	= RxBuffer[1];
					sys->Date.Month	= RxBuffer[2];
					sys->Date.Date	= RxBuffer[3];
					HAL_RTC_SetDate(&hrtc, &sys->Date, RTC_FORMAT_BIN);
					
					sys->Time.Hours		= RxBuffer[4];
					sys->Time.Minutes	= RxBuffer[5];
					sys->Time.Seconds	= RxBuffer[6];
					HAL_RTC_SetTime(&hrtc, &sys->Time, RTC_FORMAT_BIN);
			
					sys->pace_size	= RxBuffer[9]/(double)100000.0;
    		break;
			case 0xD3:
					if(RxBuffer[1] == 250){
						// Cancel alarm
						sys->isAlarmEnabled = 0;
					}
					else{
						// Set alarm
						sys->isAlarmEnabled = 1;
						__masks = RTC_ALARMMASK_NONE;
				
						if(RxBuffer[8])
							__masks |= RTC_ALARMMASK_DATEWEEKDAY;
						if(RxBuffer[9])
							__masks |= RTC_ALARMMASK_HOURS;
						if(RxBuffer[10])
							__masks |= RTC_ALARMMASK_MINUTES;
						if(RxBuffer[11])
							__masks |= RTC_ALARMMASK_SECONDS;
						
						setAlarm(RxBuffer[4], RxBuffer[5], RxBuffer[6], RxBuffer[7], __masks);
					}
    		break;
			case 0xE4:
					sys->SpO2 = RxBuffer[1] + RxBuffer[2]/10.0;
					sys->heart_beat = RxBuffer[3];
    		break;
			case 0xF5:
					for(int i=0; i<19; i++){
						if(RxBuffer[i+1] != system_heartbeat_key[i])
							setSHRreached(sys, 0);
							break;
					}
					setSHRreached(sys, 1);
    		break;
    }
	}
}

void addAck2buffer(System* sys){
	sys->TxBuffer[4] = sys->ack;
}

void addPedometer2buffer(System* sys){
	sys->TxBuffer[2] = (uint8_t)((sys->pedometer & 0xFF00) >> 8);
	sys->TxBuffer[3] = (uint8_t)((sys->pedometer & 0x00FF) >> 0);
}

void addBattery2buffer(System* sys){
	sys->TxBuffer[1] = sys->battery;
}

void clear_PPG_buffer(System* sys){
	sys->ppg_data_address_pointer=0;
}

void append_PPG_buffer(System* sys, uint32_t ir, uint32_t red){
	uint8_t irLSB, irMSB, redLSB, redMSB;
	
	irLSB = (uint8_t)((ir & 0x000000FF) >> 0);
	irMSB = (uint8_t)((ir & 0x0000FF00) >> 8);
	
	redLSB = (uint8_t)((red & 0x000000FF) >> 0);
	redMSB = (uint8_t)((red & 0x0000FF00) >> 8);
	
	sys->TxBuffer[ 10 + sys->ppg_data_address_pointer*4 + 0 ] = irLSB;
	sys->TxBuffer[ 10 + sys->ppg_data_address_pointer*4 + 1 ] = irMSB;
	sys->TxBuffer[ 10 + sys->ppg_data_address_pointer*4 + 2 ] = redLSB;
	sys->TxBuffer[ 10 + sys->ppg_data_address_pointer*4 + 3 ] = redMSB;
	
	if(sys->ppg_data_address_pointer != 199){
		sys->ppg_data_address_pointer++;
	}
	else{
		sys->ppg_data_address_pointer=0;
		TxGenerator(sys);
	}
}

void TxGenerator(System* sys){
	sys->TxBuffer[0] = 0xA0;
	
	addAck2buffer(sys);
	addBattery2buffer(sys);
	addPedometer2buffer(sys);
	
	sys->TxBuffer[TX_BUFFER_SIZE-1] = 0xA0;
	
	rawXFer(sys->TxBuffer, TX_BUFFER_SIZE);
}

void setAlarm(uint8_t Hours, uint8_t Minutes, uint8_t Seconds, uint8_t weekday, uint32_t masks){
	RTC_AlarmTypeDef sAlarm = {0};
	sAlarm.AlarmTime.Hours = Hours;
	sAlarm.AlarmTime.Minutes = Minutes;
	sAlarm.AlarmTime.Seconds = Seconds;
	sAlarm.AlarmTime.SubSeconds = 0;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmMask = masks;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
	sAlarm.AlarmDateWeekDay = weekday;
	sAlarm.Alarm = RTC_ALARM_A;
	HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);
}

float map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max){
	return (x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}
