/*
library name: 	MPU6050 6 axis module
written by: 		T.Jaber
Date Written: 	25 Mar 2019
Last Modified: 	20 April 2019 by Mohamed Yaqoob
Description: 		MPU6050 Module Basic Functions Device Driver library that use HAL libraries.
References:			
								- MPU6050 Registers map: https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf
								- Jeff Rowberg MPU6050 library: https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050
								
* Copyright (C) 2019 - T. Jaber
   This is a free software under the GNU license, you can redistribute it and/or modify it under the terms
   of the GNU General Public Licenseversion 3 as published by the Free Software Foundation.
	
   This software library is shared with puplic for educational purposes, without WARRANTY and Author is not liable for any damages caused directly
   or indirectly by this software, read more about this on the GNU General Public License.

*/

//Header files
#include "MPU6050.h"

//Library Variable
//1- I2C Handle 
static I2C_HandleTypeDef i2cHandler;

//Fucntion Definitions
//1- i2c Handler 
void MPU6050_Init(I2C_HandleTypeDef *I2Chnd)
{
	memcpy(&i2cHandler, I2Chnd, sizeof(*I2Chnd));
}

//2- i2c Read
void I2C_Read(uint8_t ADDR, uint8_t *i2cBif, uint8_t NofData)
{
	uint8_t i2cBuf[2];
	uint8_t MPUADDR;
	//Need to Shift address to make it proper to i2c operation
	MPUADDR = (MPU_ADDR<<1);
	i2cBuf[0] = ADDR;
	HAL_I2C_Master_Transmit(&i2cHandler, MPUADDR, i2cBuf, 1, 10);
	HAL_I2C_Master_Receive(&i2cHandler, MPUADDR, i2cBif, NofData, 100);
}

//3- i2c Write
void I2C_Write8(uint8_t ADDR, uint8_t data)
{
	uint8_t i2cData[2];
	i2cData[0] = ADDR;
	i2cData[1] = data;
	uint8_t MPUADDR = (MPU_ADDR<<1);
	HAL_I2C_Master_Transmit(&i2cHandler, MPUADDR, i2cData, 2,100);
}

//4- MPU6050 Initialaztion Configuration 
void MPU6050_Config(MPU_ConfigTypeDef *config)
{
	uint8_t Buffer = 0;
	//Clock Source 
	//Reset Device
	I2C_Write8(PWR_MAGT_1_REG, 0x80);
	HAL_Delay(100);
	Buffer = config ->ClockSource & 0x07; //change the 7th bits of register
	Buffer |= (config ->Sleep_Mode_Bit << 6) &0x40; // change only the 7th bit in the register
	I2C_Write8(PWR_MAGT_1_REG, Buffer);
	HAL_Delay(100); // should wait 10ms after changeing the clock setting.
	
	//Set the Digital Low Pass Filter
	Buffer = 0;
	Buffer = config->CONFIG_DLPF & 0x07;
	I2C_Write8(CONFIG_REG, Buffer);
	
	//Select the Gyroscope Full Scale Range
	Buffer = 0;
	Buffer = (config->Gyro_Full_Scale << 3) & 0x18;
	I2C_Write8(GYRO_CONFIG_REG, Buffer);
	
	//Select the Accelerometer Full Scale Range 
	Buffer = 0; 
	Buffer = (config->Accel_Full_Scale << 3) & 0x18;
	I2C_Write8(ACCEL_CONFIG_REG, Buffer);
	//Set SRD To Default
	MPU6050_Set_SMPRT_DIV(0x04);
}

//5- Get Sample Rate Divider
uint8_t MPU6050_Get_SMPRT_DIV(void)
{
	uint8_t Buffer = 0;
	
	I2C_Read(SMPLRT_DIV_REG, &Buffer, 1);
	return Buffer;
}

//6- Set Sample Rate Divider
void MPU6050_Set_SMPRT_DIV(uint8_t SMPRTvalue)
{
	I2C_Write8(SMPLRT_DIV_REG, SMPRTvalue);
}

//7- Get External Frame Sync.
uint8_t MPU6050_Get_FSYNC(void)
{
	uint8_t Buffer = 0;
	
	I2C_Read(CONFIG_REG, &Buffer, 1);
	Buffer &= 0x38; 
	return (Buffer>>3);
}

//8- Set External Frame Sync. 
void MPU6050_Set_FSYNC(enum EXT_SYNC_SET_ENUM ext_Sync)
{
	uint8_t Buffer = 0;
	I2C_Read(CONFIG_REG, &Buffer,1);
	Buffer &= ~0x38;
	
	Buffer |= (ext_Sync <<3); 
	I2C_Write8(CONFIG_REG, Buffer);
	
}

//9- Get Accel Raw Data
void MPU6050_Get_RawData(RawData_Def *rawDefA, RawData_Def *rawDefG)
{
	uint8_t i2cBuf[2];
	uint8_t AcceArr[6], GyroArr[6];
	
	I2C_Read(INT_STATUS_REG, &i2cBuf[1],1);
	if((i2cBuf[1]&&0x01))
	{
		I2C_Read(ACCEL_XOUT_H_REG, AcceArr,6);
		
		//Accel Raw Data
		rawDefA->x = ((AcceArr[0]<<8) + AcceArr[1]); // x-Axis
		rawDefA->y = ((AcceArr[2]<<8) + AcceArr[3]); // y-Axis
		rawDefA->z = ((AcceArr[4]<<8) + AcceArr[5]); // z-Axis
		//Gyro Raw Data
		I2C_Read(GYRO_XOUT_H_REG, GyroArr,6);
		rawDefG->x = (GyroArr[0]<<8) + GyroArr[1];
		rawDefG->y = (GyroArr[2]<<8) + GyroArr[3];
		rawDefG->z = (GyroArr[4]<<8) + GyroArr[5];
	}
}
