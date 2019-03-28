#ifndef MPL3115A2_H
#define MPL3115A2_H

#include <I2C.h>

namespace MPL{
	const float ALT_TO_M = 1.0/16.0;
	const float TEMP_TO_C = 1.0/16.0;

	const uint8_t I2C_ADDR = 0x60; //APPARENTLY I2C Library doesn't want shifted address

	const uint8_t REG_STATUS = 0x06;
	const uint8_t REG_OUT_P = 0x01;
	const uint8_t REG_OUT_T = 0x04;
	const uint8_t REG_SYSMOD = 0x11;
	const uint8_t REG_PT_DATA_CFG = 0x13;
	const uint8_t REG_CTRL_REG1 = 0x26;
	const uint8_t REG_OFF_H = 0x2D;

	struct AltTempData {
		uint32_t alt; //In 16ths of a meter
		uint16_t temp; //In 16ths of a degree Celsius
	};

	const uint8_t BASE_CTRL = 0xA8; //Altimeter mode, 32x oversample (130ms/samp)
	const uint8_t ACQ_REQUEST = 0x02; //OR mask to add immediate measurement bit

	AltTempData decode(uint8_t* raw){
		AltTempData ret;
		ret.alt = ((uint32_t)raw[2]>>4)|((uint32_t)raw[1]<<4)|((uint32_t)raw[0]<<12);
		ret.temp = ((uint32_t)raw[4]>>4)|((uint32_t)raw[3]<<4);
		return ret;
	}


	AltTempData PresetRead() {
		uint8_t raw[5];
		//ASSUMES reg pointer already set
		I2c.read(I2C_ADDR, 5, raw);
		return decode(raw);
	}

	AltTempData ReadData() {
		I2c.write(I2C_ADDR, REG_OUT_P); //Set reg pointer
		return PresetRead();
	}

	AltTempData CheckAndRead() {
		uint8_t status;
		I2c.read(I2C_ADDR, REG_STATUS, 1, &status);
		if(status & 0x08){
			return PresetRead();
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
		return PresetRead();
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