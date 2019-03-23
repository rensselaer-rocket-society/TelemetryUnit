#ifndef MPL3115A2_H
#define MPL3115A2_H

#include <I2C.h>

namespace MPL{

	const uint8_t I2C_ADDR = 0xC0;

	const uint8_t REG_STATUS = 0x00;
	const uint8_t REG_OUT_P = 0x01;
	const uint8_t REG_OUT_T = 0x04;
	const uint8_t REG_SYSMOD = 0x11;
	const uint8_t REG_PT_DATA_CFG = 0x13;
	const uint8_t REG_CTRL_REG1 = 0x26;
	const uint8_t REG_OFF_H = 0x2D;

	struct AltTempData {
		unsigned long alt; //In 16ths of a meter
		unsigned int temp; //In 16ths of a degree Celsius
	};

	const uint8_t BASE_CTRL = 0xA8; //Altimeter mode, 32x oversample (130ms/samp)
	const uint8_t ACQ_REQUEST = 0x02; //OR mask to add immediate measurement bit

	AltTempData decode(uint8_t* raw){
		AltTempData ret;
		ret.alt = (raw[2]>>4)|(raw[1]<<4)|(raw[0]<<12);
		ret.temp = (raw[4]>>4)|(raw[3]<<4);
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
		bool ready = I2c.read(I2C_ADDR, REG_STATUS) & 0x08;
		if(ready){
			return PresetRead();
		} else {
			AltTempData ret;
			memset(&ret, 0xFF, sizeof(&ret));
			return ret;
		}
	}

	AltTempData BlockingRead() {
		bool ready;
		do{
			ready = I2c.read(I2C_ADDR, REG_STATUS) & 0x08;
		} while(!ready);
		return PresetRead();
	}

	bool IsDataReady() {
		return I2c.read(I2C_ADDR, REG_STATUS) & 0x08;
	}

	void SetStandby(bool stby) {
		I2c.write(I2C_ADDR, REG_SYSMOD, (uint8_t)(stby ? 0x00 : 0x01));
	}

	void RequestData() {
		I2c.write(I2C_ADDR, REG_CTRL_REG1, (uint8_t)(BASE_CTRL | ACQ_REQUEST));
	}

	void Init() {
		I2c.write(I2C_ADDR, REG_PT_DATA_CFG, (uint8_t)0x07); //Enable data ready flags
		I2c.write(I2C_ADDR, REG_CTRL_REG1, BASE_CTRL); //Base config
	}
}

#endif