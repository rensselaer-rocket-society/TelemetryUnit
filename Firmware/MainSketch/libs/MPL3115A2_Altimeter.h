#ifndef MPL3115A2_H
#define MPL3115A2_H

#include <I2C.h>

namespace MPL{
	const float ALT_TO_M = 1.0/16.0;
	const float TEMP_TO_C = 1.0/16.0;

	const uint8_t I2C_ADDR = 0x60; //APPARENTLY I2C Library doesn't want shifted address

	const uint8_t REG_STATUS = 0x00;
	const uint8_t REG_OUT_P = 0x01;
	const uint8_t REG_OUT_T = 0x04;
	const uint8_t REG_SYSMOD = 0x11;
	const uint8_t REG_PT_DATA_CFG = 0x13;
	const uint8_t REG_CTRL_REG1 = 0x26;
	const uint8_t REG_OFF_H = 0x2D;

	struct AltTempData {
		int32_t alt; //In 16ths of a meter
		int16_t temp; //In 16ths of a degree Celsius
	};

	const uint8_t BASE_CTRL = 0xA8; //Altimeter mode, 32x oversample (130ms/samp)
	const uint8_t ACQ_REQUEST = 0x02; //OR mask to add immediate measurement bit

	AltTempData decode(uint8_t* raw){
		AltTempData ret;
		ret.alt = ((uint32_t)raw[2]<<8)|((uint32_t)raw[1]<<16)|((uint32_t)raw[0]<<24);
		ret.alt = ret.alt >> 12; //Shift back with sign extend
		ret.temp = raw[4]|((uint16_t)raw[3]<<8);
		ret.temp = ret.temp >> 4; //Shift back with sign extend
		return ret;
	}


	AltTempData ReadData() {
		uint8_t raw[5];
		I2c.read(I2C_ADDR, REG_OUT_P, 5, raw);
		return decode(raw);
	}

	AltTempData CheckAndRead() {
		uint8_t data[6];
		I2c.read(I2C_ADDR, REG_STATUS, 6, data); //Status and data (maybe)
		if(data[0] & 0x08){
			return decode(data+1);
		} else {
			AltTempData ret;
			memset(&ret, 0xFF, sizeof(ret));
			return ret;
		}
	}

	AltTempData BlockingRead() {
		uint8_t status;
		do{
			I2c.read(I2C_ADDR, REG_STATUS, 1, &status);
		} while(!(status & 0x08));
		return ReadData();
	}

	bool IsDataReady() {
		uint8_t status;
		I2c.read(I2C_ADDR, REG_STATUS, 1, &status);
		return status & 0x08;
	}

	void SetStandby(bool stby) {
		I2c.write(I2C_ADDR, REG_SYSMOD, (uint8_t)(stby ? 0x00 : 0x01));
	}

	void RequestData() {
		I2c.write(I2C_ADDR, REG_CTRL_REG1, (uint8_t)(BASE_CTRL | ACQ_REQUEST));
	}

	bool CheckDevicePresent() { //Check WHO_AM_I register
		uint8_t whoami;
		if(!I2c.read(I2C_ADDR, 0x0C, 1, &whoami)){ 
			return whoami == 0xC4;
		} else return false;
	}

	void Init() {
		I2c.write(I2C_ADDR, REG_PT_DATA_CFG, (uint8_t)0x07); //Enable data ready flags
		I2c.write(I2C_ADDR, REG_CTRL_REG1, BASE_CTRL); //Base config
	}
}

#endif