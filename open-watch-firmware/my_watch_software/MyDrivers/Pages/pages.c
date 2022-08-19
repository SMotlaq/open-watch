#include "pages.h"

void render_ack_waiting(System* sys){
	
	ssd1306_Fill(Black);

	ssd1306_DrawShape(sys->display_collibration_x+60, sys->display_collibration_y+16, Shape_UP);
	
	ssd1306_SetCursor(sys->display_collibration_x + 42, sys->display_collibration_y + 22);
	ssd1306_WriteString("ACCEPT", Font_7x10, White);
	
//	for(int i=0; i<102; i++)
//		ssd1306_DrawPixel(sys->display_collibration_x + i + 14, sys->display_collibration_y + 36, White);
	
	ssd1306_SetCursor(sys->display_collibration_x + 0, sys->display_collibration_y + 37);
	ssd1306_WriteString("Connection request", Font_7x10, White);
	
	ssd1306_SetCursor(sys->display_collibration_x + 42, sys->display_collibration_y + 52);
	ssd1306_WriteString("REJECT", Font_7x10, White);
	
	ssd1306_DrawShape(sys->display_collibration_x+60, sys->display_collibration_y+63, Shape_DOWN);
	
	ssd1306_UpdateScreen(&hi2c1);
}

void render_home(System* sys){
					
	char buf[15];

	ssd1306_Fill(Black);

	ssd1306_SetCursor(sys->display_collibration_x+24, sys->display_collibration_y+16);
	sprintf(buf, "%02d:%02d", sys->Time.Hours, sys->Time.Minutes);
	ssd1306_WriteString(buf,Font_16x26,White);

	ssd1306_SetCursor(sys->display_collibration_x+104, sys->display_collibration_y+29);
	sprintf(buf, ":%02d", sys->Time.Seconds);
	ssd1306_WriteString(buf,Font_7x10,White);

	ssd1306_SetCursor(sys->display_collibration_x+30, sys->display_collibration_y+42);
	sprintf(buf, "20%02d-%02d-%02d", sys->Date.Year, sys->Date.Month, sys->Date.Date);
	ssd1306_WriteString(buf,Font_7x10,White);
	
	
	if(sys->battery/12 == 0){
		ssd1306_DrawShape(sys->display_collibration_x+0, sys->display_collibration_y+54, Shape_battery_allert);
	}
	else{
		ssd1306_DrawShape(sys->display_collibration_x+0, sys->display_collibration_y+54, Shape_battery_empty);
		for(uint8_t i=0; i<sys->battery/12; i++)
			ssd1306_DrawShape(sys->display_collibration_x+2+i, sys->display_collibration_y+56, Shape_battery_bar);
	}
	ssd1306_SetCursor(sys->display_collibration_x+15, sys->display_collibration_y+55);
	sprintf(buf, "%d%%", (uint8_t)((sys->battery>=100) ? 100 : sys->battery));
	ssd1306_WriteString(buf,Font_7x10,White);

	ssd1306_DrawShape(sys->display_collibration_x+121, sys->display_collibration_y+54, Shape_BT);
	
	if(getConnected(sys))
		ssd1306_DrawShape(sys->display_collibration_x+110, sys->display_collibration_y+55, Shape_Connected);
	else
		ssd1306_DrawShape(sys->display_collibration_x+110, sys->display_collibration_y+54, Shape_NC);

	ssd1306_UpdateScreen(&hi2c1);
}

void render_connected(System* sys){
	ssd1306_Fill(Black);

	ssd1306_SetCursor(sys->display_collibration_x + 14, sys->display_collibration_y + 22);
	ssd1306_WriteString("Greetings", Font_11x18, White);
	
	ssd1306_SetCursor(sys->display_collibration_x + sys->name_xpos, sys->display_collibration_y + 43);
	ssd1306_WriteString(sys->user_name,Font_7x10,White);
	
	ssd1306_UpdateScreen(&hi2c1);
}

void render_bloody_hell(System* sys){
	
	char buf[15];
	
	ssd1306_Fill(Black);

	// Heart Rate
	ssd1306_DrawShape(sys->display_collibration_x + 9, sys->display_collibration_y + 22, Shape_heart);
	if(getConnected(sys)){
		ssd1306_SetCursor(sys->display_collibration_x + ((sys->heart_beat>=100) ? 29 : 32), sys->display_collibration_y + 20);
		sprintf(buf, "%d", sys->heart_beat);
		ssd1306_WriteString(buf,Font_7x10,White);
	}
	else{
		ssd1306_SetCursor(sys->display_collibration_x + 29, sys->display_collibration_y + 20);
		ssd1306_WriteString("---",Font_7x10,White);
	}
	
	ssd1306_SetCursor(sys->display_collibration_x + 32, sys->display_collibration_y + 32);
	ssd1306_WriteString("HR",Font_7x10,White);
	
	// SpO2
	ssd1306_DrawShape(sys->display_collibration_x + 67 , sys->display_collibration_y + 22, Shape_SpO2);
	ssd1306_SetCursor(sys->display_collibration_x + 85, sys->display_collibration_y + 20);
	if(getConnected(sys)){
		sprintf(buf, "%.1f%%", sys->SpO2);
		ssd1306_WriteString(buf,Font_7x10,White);
	}
	else{
		ssd1306_WriteString(" ---",Font_7x10,White);
	}
	
	ssd1306_SetCursor(sys->display_collibration_x + 88, sys->display_collibration_y + 32);
	ssd1306_WriteString("SpO2",Font_7x10,White);
	
	// Text
	ssd1306_SetCursor(sys->display_collibration_x + 26, sys->display_collibration_y + 55);
	ssd1306_WriteString("HEALTH CARE",Font_7x10,White);
	
	ssd1306_UpdateScreen(&hi2c1);
}

void render_pedomedo(System* sys){
	
	char buf[15];
	
	ssd1306_Fill(Black);

	// Pesometer
	ssd1306_DrawShape(sys->display_collibration_x + 66 , sys->display_collibration_y + 22, Shape_kafsh);
	
	uint8_t temp_x = 0;
	if(sys->pedometer <10)
		temp_x = 99;
	else if(sys->pedometer < 100)
		temp_x = 95;
	else if(sys->pedometer < 1000)
		temp_x = 92;
	else if(sys->pedometer < 10000)
		temp_x = 88;
	else
		temp_x = 85;
	
	ssd1306_SetCursor(sys->display_collibration_x + temp_x, sys->display_collibration_y + 20);
	sprintf(buf, "%d", sys->pedometer);
	ssd1306_WriteString(buf,Font_7x10,White);
	
	ssd1306_SetCursor(sys->display_collibration_x + 85, sys->display_collibration_y + 32);
	ssd1306_WriteString("Steps",Font_7x10,White);
	
	// Distance
	ssd1306_DrawShape(sys->display_collibration_x + 14 , sys->display_collibration_y + 22, Shape_Location);
	double distance = sys->pedometer * sys->pace_size;
	ssd1306_SetCursor(sys->display_collibration_x + ((distance>=10) ? 27 : 30), sys->display_collibration_y + 20);
	sprintf(buf, "%.1f", distance);
	ssd1306_WriteString(buf,Font_7x10,White);
	
	ssd1306_SetCursor(sys->display_collibration_x + 34, sys->display_collibration_y + 32);
	ssd1306_WriteString("km",Font_7x10,White);
	
	// Text
	ssd1306_SetCursor(sys->display_collibration_x + 33, sys->display_collibration_y + 55);
	ssd1306_WriteString("PEDOMETER",Font_7x10,White);
	
	ssd1306_UpdateScreen(&hi2c1);
}

void render_about(System* sys){
	
	ssd1306_Fill(Black);

	ssd1306_SetCursor(sys->display_collibration_x + 14, sys->display_collibration_y + 15);
	ssd1306_WriteString("OpenWatch", Font_11x18, White);
	
	ssd1306_SetCursor(sys->display_collibration_x + 8, sys->display_collibration_y + 39);
	ssd1306_WriteString("S.Motlaq - SMSMT",Font_7x10,White);
	
	ssd1306_SetCursor(sys->display_collibration_x + 53, sys->display_collibration_y + 54);
	ssd1306_WriteString("AUT",Font_7x10,White);
	
	ssd1306_UpdateScreen(&hi2c1);
}

void render_ringing(System* sys){
	
	ssd1306_Fill(Black);
	
	// Clock icon
	ssd1306_DrawShape(sys->display_collibration_x + 48 , sys->display_collibration_y + 16, Shape_clock_LH);
	ssd1306_DrawShape(sys->display_collibration_x + 64 , sys->display_collibration_y + 16, Shape_clock_RH);
	
	ssd1306_SetCursor(sys->display_collibration_x + 11, sys->display_collibration_y + 55);
	ssd1306_WriteString("ALARM ACTIVATED",Font_7x10,White);
	
	ssd1306_UpdateScreen(&hi2c1);
}
