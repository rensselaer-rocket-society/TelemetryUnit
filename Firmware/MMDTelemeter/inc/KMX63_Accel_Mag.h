#ifndef KMZ63_ACCEL_MAG_H
#define KMZ63_ACCEL_MAG_H

#include <I2C.h>

namespace KMX{

	const float ACCEL_TO_MPSPS = 0.00195;
	const float MAG_TO_UTSLA = 0.03662;

	const uint8_t I2C_ADDR = 0x0E; //APPARENTLY I2C Library doesn't want shifted address

	const uint8_t REG_INS1 = 0x01;
	const uint8_t REG_INC1 = 0x2A;
	const uint8_t REG_INC3 = 0x2C;
	const uint8_t REG_OUT_MAG = 0x10;
	const uint8_t REG_OUT_XL = 0x0A;
	const uint8_t REG_OUT_TEMP = 0x16;
	const uint8_t REG_ODCNTL = 0x38;
	const uint8_t REG_CNTL2 = 0x3A;

	struct Vec3 {
		int16_t x;
		int16_t y;
		int16_t z; 
	};

	struct AccelMagData {
		Vec3 accel; // 4.788 mm/(s^2) (0.488 mg) per LSB
		Vec3 magneto; // 17.5 mDeg/s per LSB (1.05 arcminutes/s per LSB)
		int16_t temp;
	};
	
	bool IsDataReady() {
		uint8_t status;
		I2c.read(I2C_ADDR, REG_INS1, 1, &status);
		return (status & 0x18) == 0x18; //If both channels are available
	}

	AccelMagData ReadData() {
		AccelMagData ret;
		//Byte orders are consistent for this device, can populate struct directly
		I2c.read(I2C_ADDR, REG_OUT_XL, 14, (uint8_t*)&ret);
		return ret;
	}

	bool CheckDevicePresent() {
		uint8_t whoami;
		if(!I2c.read(I2C_ADDR, 0x00, 1, &whoami)){ 
			return whoami == 0x2D;
		} else return false;
	}

	void Init() {

		I2c.write(I2C_ADDR, REG_INC1, (uint8_t)0x18); // Accel/Mag data ready interrupt GPIO1
		I2c.write(I2C_ADDR, REG_INC3, (uint8_t)0x81); // GPIO1 Push-Pull Pulsed Active Low
		I2c.write(I2C_ADDR, REG_ODCNTL, (uint8_t)0x33); // 100 Hz mode for both accel, mag, and temp
		I2c.write(I2C_ADDR, REG_CNTL2, (uint8_t)0x7D); // Turn on accel/mag/gyro +/-64G range, maximum resolution
	}
}

#endif