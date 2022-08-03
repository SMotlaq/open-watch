#ifndef __PAGES__
#define __PAGES__

#include "system.h"
#include "ssd1306.h"
#include "stdio.h"
#include "i2c.h"

float map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max);

void render_ack_waiting(System* sys);
void render_home(System* sys);
void render_connected(System* sys);
void render_bloody_hell(System* sys);
void render_pedomedo(System* sys);
void render_about(System* sys);
void render_ringing(System* sys);

#endif
