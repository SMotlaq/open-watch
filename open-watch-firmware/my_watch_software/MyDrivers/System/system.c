#include "system.h"

System sys_init(){
	System sys;
	
	sys.battery = 0;
	
	return sys;
}

// Get Flags
uint8_t getCharging(System* sys){
	return (sys->flags >> ISCHARGING_MASK) & 0x01;
}

uint8_t getPSaving(System* sys){
	return (sys->flags >> ISPSAVING_MASK) & 0x01;
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
void setCharging(System* sys, uint8_t state){
	sys->flags = state ? (sys->flags | (1 << ISCHARGING_MASK)) : (sys->flags | (1<<ISCHARGING_MASK));
}

void setPSaving(System* sys, uint8_t state){
	sys->flags = state ? (sys->flags | (1 << ISPSAVING_MASK)) : (sys->flags | (1<<ISPSAVING_MASK));
}

void setPPGEnable(System* sys, uint8_t state){
	sys->flags = state ? (sys->flags | (1 << ISPPGENABLED_MASK)) : (sys->flags | (1<<ISPPGENABLED_MASK));
}

void setScreenFlag(System* sys, uint8_t state){
	sys->flags = state ? (sys->flags | (1 << SCREENFLAG_MASK)) : (sys->flags | (1<<SCREENFLAG_MASK));
}

void setScreenEable(System* sys, uint8_t state){
	sys->flags = state ? (sys->flags | (1 << SCREENENABLED_MASK)) : (sys->flags | (1<<SCREENENABLED_MASK));
}

void setPedFlag(System* sys, uint8_t state){
	sys->flags = state ? (sys->flags | (1 << PEDFLAG_MASK)) : (sys->flags | (1<<PEDFLAG_MASK));
}

void setVibEnable(System* sys, uint8_t state){
	sys->flags = state ? (sys->flags | (1 << VIBENENABLED_MASK)) : (sys->flags | (1<<VIBENENABLED_MASK));
}

void setSoundEnable(System* sys, uint8_t state){
	sys->flags = state ? (sys->flags | (1 << SOUNDENABLED_MASK)) : (sys->flags | (1<<SOUNDENABLED_MASK));
}

// Main functions
void RxParser(uint8_t* RxBuffer){
	
}