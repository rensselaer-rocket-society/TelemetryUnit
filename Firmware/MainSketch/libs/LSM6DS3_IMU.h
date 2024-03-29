#ifndef LSM6DS3_H
#define LSM6DS3_H

#include <I2C.h>

namespace LSM{

	const float ACCEL_TO_MPSPS = 0.004788;
	const float GYRO_TO_DPS = 0.0175;

	const uint8_t I2C_ADDR = 0x6B; //APPARENTLY I2C Library doesn't want shifted address

	const uint8_t REG_STATUS = 0x1E;
	const uint8_t REG_OUT_G = 0x22;
	const uint8_t REG_OUT_XL = 0x28;
	const uint8_t REG_CTRL1_XL = 0x10;
	const uint8_t REG_CTRL2_G = 0x11;

	struct Vec3 {
		int16_t x;
		int16_t y;
		int16_t z; 
	};

	struct AccelGyroData {
		Vec3 gyro; // 17.5 mDeg/s per LSB (1.05 arcminutes/s per LSB)
		Vec3 accel; // 4.788 mm/(s^2) (0.488 mg) per LSB
	};

	bool IsDataReady() {
		uint8_t status;
		I2c.read(I2C_ADDR, REG_STATUS, 1, &status);
		return status & 0x03;//If either data is new
	}

	AccelGyroData ReadData() {
		AccelGyroData ret;
		//Byte orders are consistent for this device, can populate struct directly
		I2c.read(I2C_ADDR, REG_OUT_G, 12, (uint8_t*)&ret);
		return ret;
	}

	AccelGyroData CheckAndRead() {
		if(IsDataReady()){
			return ReadData();
		} else {
			AccelGyroData ret;
			memset(&ret, 0xFF, sizeof(ret));
			return ret;
		}
	}

	AccelGyroData BlockingRead() {
		bool ready;
		do{
			ready = IsDataReady();
		} while(!ready);
		return ReadData();
	}

	bool CheckDevicePresent() {
		uint8_t whoami;
		if(!I2c.read(I2C_ADDR, 0x0F, 1, &whoami)){ 
			return whoami == 0x69;
		} else return false;
	}

	void Init() {
		I2c.write(I2C_ADDR, REG_CTRL1_XL, (uint8_t)0x44); // 104Hz, +/-16g
		I2c.write(I2C_ADDR, REG_CTRL2_G, (uint8_t)0x44);  // 104Hz, Full scale 500dps (maybe lower?)
	}
}

#endif