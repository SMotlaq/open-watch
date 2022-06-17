#ifndef __MAIN_H
	#include "main.h"
	#define __MAIN_H
#endif
#include "tim.h"

#define ALLXYZ_MODE 0
#define LOWMEM_MODE 1  

typedef struct{
	int16_t x_hat;
	int16_t z_x;
	int16_t pre_x_hat;
	
	#if ALLXYZ_MODE	
		int16_t y_hat;
		int16_t z_y;
		int16_t pre_y_hat;
	
		int16_t z_hat;	
		int16_t z_z;
		int16_t pre_z_hat;
	#endif
	
	float Pn;
	float pre_Pn1;
	
	float R;
	float Q;
	
} KalmanScaler;

typedef struct{
	
	int16_t wy_hat;
	int16_t dwy_hat;
	int16_t z_y;
	int16_t pre_wy_hat;
	int16_t pre_dwy_hat;
	
	#if ALLXYZ_MODE
		int16_t wx_hat;
		int16_t dwx_hat;
		int16_t z_x;
		int16_t pre_wx_hat;
		int16_t pre_dwx_hat;
		
		int16_t wz_hat;
		int16_t dwz_hat;
		int16_t z_z;	
		int16_t pre_wz_hat;
		int16_t pre_dwz_hat;	
	#endif
	
	#if LOWMEM_MODE==0
		float Pnn1[2][2];
		float Pn[2][2];
		float pre_Pn1[2][2];
		
		float R;
		float Q;
	#endif
	
	float K[2];
	
} Kalman2D1R;

void kalman_scaler_init(KalmanScaler* hkalman, float Q, float R);
void kalman_2d1r_init(Kalman2D1R* hkalman, float Q, float R);

void kalman_scaler_update(KalmanScaler* hkalman);
void kalman_2d1r_update(Kalman2D1R* hkalman);
