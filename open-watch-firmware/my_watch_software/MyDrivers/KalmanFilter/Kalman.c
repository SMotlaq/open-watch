#include "Kalman.h"
#include "math.h"

void kalman_scaler_init(KalmanScaler* hkalman, float Q, float R){
	#if ALLXYZ_MODE	
		hkalman->y_hat = 0;
		hkalman->z_hat = 0;
		
		hkalman->z_y = 0;
		hkalman->z_z = 0;
		
		hkalman->pre_y_hat = 0;
		hkalman->pre_z_hat = 0;
	#endif

	hkalman->x_hat = 0;
	hkalman->z_x = 0;
	hkalman->pre_x_hat = 0;
	
	hkalman->Pn = 0;
	hkalman->pre_Pn1= 0;
	
	hkalman->Q = Q;
	hkalman->R = R;
	
}

void kalman_2d1r_init(Kalman2D1R* hkalman, float Q, float R){
	#if ALLXYZ_MODE
		hkalman->wx_hat = 0;
		hkalman->dwx_hat = 0;

		hkalman->wz_hat = 0;
		hkalman->dwz_hat = 0;

		hkalman->z_x = 0;
		hkalman->z_z = 0;
		
		hkalman->pre_wx_hat = 0;
		hkalman->pre_dwx_hat = 0;
		hkalman->pre_wz_hat = 0;
		hkalman->pre_dwz_hat = 0;
	#endif
	
	hkalman->wy_hat = 0;
	hkalman->dwy_hat = 0;	
	
	hkalman->z_y = 0;
	
	hkalman->pre_wy_hat = 0;
	hkalman->pre_dwy_hat = 0;
	
	#if LOWMEM_MODE==0
		hkalman->Pnn1[0][0] = 0;
		hkalman->Pnn1[1][0] = 0;
		hkalman->Pnn1[0][1] = 0;
		hkalman->Pnn1[1][1] = 0;
		
		hkalman->Pn[0][0] = 0;
		hkalman->Pn[1][0] = 0;
		hkalman->Pn[0][1] = 0;
		hkalman->Pn[1][1] = 0;
		
		hkalman->pre_Pn1[0][0] = 0;
		hkalman->pre_Pn1[1][0] = 0;
		hkalman->pre_Pn1[0][1] = 0;
		hkalman->pre_Pn1[1][1] = 0;

		hkalman->R = R;
		hkalman->Q = Q;
		
		hkalman->K[0] = 0;
		hkalman->K[1] = 0;
	#endif

}

void kalman_scaler_update(KalmanScaler* hkalman){
	hkalman->x_hat = hkalman->R/((float)(hkalman->pre_Pn1+hkalman->Q+hkalman->R))*hkalman->pre_x_hat + (hkalman->pre_Pn1+hkalman->Q)/((float)(hkalman->pre_Pn1+hkalman->Q+hkalman->R))*hkalman->z_x;
	hkalman->pre_x_hat = hkalman->x_hat;

	#if ALLXYZ_MODE
		hkalman->y_hat = hkalman->R/((float)(hkalman->pre_Pn1+hkalman->Q+hkalman->R))*hkalman->pre_y_hat + (hkalman->pre_Pn1+hkalman->Q)/((float)(hkalman->pre_Pn1+hkalman->Q+hkalman->R))*hkalman->z_y;
		hkalman->pre_y_hat = hkalman->y_hat;
		
		hkalman->z_hat = hkalman->R/((float)(hkalman->pre_Pn1+hkalman->Q+hkalman->R))*hkalman->pre_z_hat + (hkalman->pre_Pn1+hkalman->Q)/((float)(hkalman->pre_Pn1+hkalman->Q+hkalman->R))*hkalman->z_z;
		hkalman->pre_z_hat = hkalman->z_hat;
	#endif
	
	#if LOWMEM_MODE	
		hkalman->Pn = 26.4357548;
		hkalman->pre_Pn1 = 26.4357548;
	#else
		hkalman->Pn = hkalman->R*(hkalman->pre_Pn1+hkalman->Q)/((float)(hkalman->pre_Pn1+hkalman->Q+hkalman->R));	
		hkalman->pre_Pn1 = hkalman->Pn;
	#endif
	
}

void kalman_2d1r_update(Kalman2D1R* hkalman){
	float T = SAMPLING_TIME;
	
	#if LOWMEM_MODE==0
		hkalman->Pnn1[0][0] = hkalman->pre_Pn1[0][0] + 2*T*hkalman->pre_Pn1[0][1] + T*T*hkalman->pre_Pn1[1][1];
		hkalman->Pnn1[1][0] = hkalman->pre_Pn1[0][1] + T*hkalman->pre_Pn1[1][1];
		hkalman->Pnn1[0][1] = hkalman->Pnn1[1][0];
		hkalman->Pnn1[1][1] = hkalman->pre_Pn1[1][1] + hkalman->Q;

		hkalman->K[0] = hkalman->Pnn1[0][0]/((float)(hkalman->Pnn1[0][0] + hkalman->R));
		hkalman->K[1] = hkalman->Pnn1[1][0]/((float)(hkalman->Pnn1[0][0] + hkalman->R));
	#else
		hkalman->K[0] = 0.024835; 
		hkalman->K[1] = 0.312276; 
	#endif

	#if ALLXYZ_MODE
		hkalman->wx_hat = -hkalman->K[0]*hkalman->wx_hat + hkalman->wx_hat -hkalman->K[0]*T*hkalman->dwx_hat + T*hkalman->dwx_hat + hkalman->K[0]*hkalman->z_x;
		hkalman->dwx_hat = -hkalman->K[1]*hkalman->wx_hat - hkalman->K[1]*T*hkalman->dwx_hat + hkalman->dwx_hat + hkalman->K[1]*hkalman->z_x;
		hkalman->pre_wx_hat = hkalman->wx_hat;
		hkalman->pre_dwx_hat = hkalman->dwx_hat;
		
		hkalman->wz_hat = -hkalman->K[0]*hkalman->wz_hat + hkalman->wz_hat -hkalman->K[0]*T*hkalman->dwz_hat + T*hkalman->dwz_hat + hkalman->K[0]*hkalman->z_z;
		hkalman->dwz_hat = -hkalman->K[1]*hkalman->wz_hat - hkalman->K[1]*T*hkalman->dwz_hat + hkalman->dwz_hat + hkalman->K[1]*hkalman->z_z;
		hkalman->pre_wz_hat = hkalman->wz_hat;
		hkalman->pre_dwz_hat = hkalman->dwz_hat;
	#endif

	hkalman->wy_hat = -hkalman->K[0]*hkalman->wy_hat + hkalman->wy_hat -hkalman->K[0]*T*hkalman->dwy_hat + T*hkalman->dwy_hat + hkalman->K[0]*hkalman->z_y;
	hkalman->dwy_hat = -hkalman->K[1]*hkalman->wy_hat - hkalman->K[1]*T*hkalman->dwy_hat + hkalman->dwy_hat + hkalman->K[1]*hkalman->z_y;
	hkalman->pre_wy_hat = hkalman->wy_hat;
	hkalman->pre_dwy_hat = hkalman->dwy_hat;	

	#if LOWMEM_MODE==0
		hkalman->Pn[0][0] = pow(hkalman->K[0],2)*hkalman->Pnn1[0][0] + pow(hkalman->K[0],2)*hkalman->R - 2*hkalman->K[0]*hkalman->Pnn1[0][0] + hkalman->Pnn1[0][0];
		hkalman->Pn[1][0] = hkalman->K[0]*hkalman->K[1]*hkalman->Pnn1[0][0] + hkalman->K[0]*hkalman->K[1]*hkalman->R - hkalman->K[0]*hkalman->Pnn1[0][1] - hkalman->K[1]*hkalman->Pnn1[0][0] + hkalman->Pnn1[0][1];
		hkalman->Pn[0][1] = hkalman->Pn[1][0];
		hkalman->Pn[1][1] = pow(hkalman->K[1],2)*hkalman->Pnn1[0][0] + pow(hkalman->K[1],2)*hkalman->R - 2*hkalman->K[1]*hkalman->Pnn1[0][1] + hkalman->Pnn1[1][1];
		
		hkalman->pre_Pn1[0][0] = hkalman->Pn[0][0];
		hkalman->pre_Pn1[1][0] = hkalman->Pn[1][0];
		hkalman->pre_Pn1[0][1] = hkalman->Pn[0][1];
		hkalman->pre_Pn1[1][1] = hkalman->Pn[1][1];
	#endif
}
