#include "system.h"

System sys_init(){
	System sys;
	sys.display_collibration_x = 2;
	sys.display_collibration_y = -2;
	sys.pedometer = 3;
	sys.SpO2 = 98.4;
	sys.heart_beat = 112;
	sys.flags = 0x88;
	sys.voltage = 0;
	sys.state = hello;
	sprintf(sys.user_name, "Salman");
	sys.name_xpos = 40;
	sys.pace_size = 0.0008;
	
	return sys;
}

// Get Flags
uint8_t getConnected(System* sys){
	return (sys->flags >> ISCONNECTED_MASK) & 0x01;
}

uint8_t getRinging(System* sys){
	return (sys->flags >> ISRINING_MASK) & 0x01;
}

uint8_t getPPGEnable(System* sys){
	return (sys->flags >> ISPPGENABLED_MASK) & 0x01;
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

void setPPGEnable(System* sys, uint8_t state){
	sys->flags = state ? (sys->flags | (1 << ISPPGENABLED_MASK)) : (sys->flags & ~(1<<ISPPGENABLED_MASK));
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

// Main functions
void RxParser(System* sys, uint8_t* RxBuffer){
	// set name
	// set name xpos
	// set date and time
	// set pac size
	setScreenEable(sys, 1);
	//DEBUG("%s", (char*)RxBuffer);
}

void AddPPData(SAMPLE* samples, uint8_t number(
