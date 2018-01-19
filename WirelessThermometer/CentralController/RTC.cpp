#include "RTC.h"

#define DS3231_REG_SECONDS       0x00
#define DS3231_REG_MINUTES       0x01
#define DS3231_REG_HOURS         0x02
#define DS3231_REG_DAY           0x03
#define DS3231_REG_DATE          0x04
#define DS3231_REG_MONTH_CENTURY 0x05
#define DS3231_REG_YEAR          0x06
#define DS3231_REG_A1M1_SECONDS  0x07
#define DS3231_REG_A1M2_MINUTES  0x08
#define DS3231_REG_A1M3_HOURS    0x09
#define DS3231_REG_A1M4_DAY_DATE 0x0A
#define DS3231_REG_A2M2_MINUTES  0x0B
#define DS3231_REG_A2M3_HOURS    0x0C
#define DS3231_REG_A2M4_DAY_DATE 0x0D
#define DS3231_REG_CONTROL       0x0E
#define DS3231_REG_STATUS        0x0F
#define DS3231_REG_AGING_OFFSET  0x10
#define DS3231_REG_TEMP_MSB      0x11
#define DS3231_REG_TEMP_LSB      0x12

static void i2cReadReg(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t size) {
	Wire.beginTransmission(addr);
	Wire.write(reg);
	Wire.endTransmission();

	Wire.requestFrom(addr, size);
	Wire.readBytes(buf, size);
}

static void i2cWriteReg(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t size) {
	Wire.beginTransmission(addr);
	Wire.write(reg);
	for (uint8_t i = 0; i < size; i++) {
		Wire.write(buf[i]);
	}
	Wire.endTransmission();
}

void RTC::setup() {
	// Set alarm1 every second.
	uint8_t data[4] = { 0x80,0x80,0x80,0x80 };  // A1M1=A1M2=A1M3=A1M4 = 1111, alarm once per second.
	i2cWriteReg(addr, DS3231_REG_A1M1_SECONDS, data, 4);

	// Enable alarm1.
	uint8_t ctrl = 0x01;
	i2cWriteReg(addr, DS3231_REG_CONTROL, &ctrl, 1);

	//uint8_t initd[7] = {
	//	0x00, 0x37, 0x00, 0x06, 0x13, 0x81, 0x18
	//};
	//i2cWriteReg(addr, 0, initd, 7);
}

void RTC::loop() {
	loopSet();
	loopRead();
}

void RTC::loopSet() {
	if (ds.f.rtcYearEditReadyToSet) {
		ds.f.rtcYearEditReadyToSet = 0;
		isValueSet = true;

		uint8_t century = ds.rtcYearEdit >= 2000 ? 0x80 : 0x00;
		uint8_t year = ds.rtcYearEdit % 100;
		uint8_t regData;
		i2cReadReg(addr, DS3231_REG_MONTH_CENTURY, &regData, 1);
		regData = (regData & 0x7F) | century;
		i2cWriteReg(addr, DS3231_REG_MONTH_CENTURY, &regData, 1);
		regData = ((year / 10) << 4) | (year % 10);
		i2cWriteReg(addr, DS3231_REG_YEAR, &regData, 1);
	}

	if (ds.f.rtcMonthEditReadyToSet) {
		ds.f.rtcMonthEditReadyToSet = 0;
		isValueSet = true;

		uint8_t regData;
		i2cReadReg(addr, DS3231_REG_MONTH_CENTURY, &regData, 1);
		regData = (regData & 0x80)
			| ((ds.rtcMonthEdit / 10) << 4) | (ds.rtcMonthEdit % 10);
		i2cWriteReg(addr, DS3231_REG_MONTH_CENTURY, &regData, 1);
	}

	if (ds.f.rtcDayEditReadyToSet) {
		ds.f.rtcDayEditReadyToSet = 0;
		isValueSet = true;

		uint8_t regData = ((ds.rtcDayEdit / 10) << 4) | (ds.rtcDayEdit % 10);
		i2cWriteReg(addr, DS3231_REG_DATE, &regData, 1);
	}

	if (ds.f.rtcWeekdayEditReadyToSet) {
		ds.f.rtcWeekdayEditReadyToSet = 0;
		isValueSet = true;

		uint8_t regData = ds.rtcWeekdayEdit;
		i2cWriteReg(addr, DS3231_REG_DAY, &regData, 1);
	}

	if (ds.f.rtcHourEditReadyToSet) {
		ds.f.rtcHourEditReadyToSet = 0;
		isValueSet = true;

		uint8_t regData = ((ds.rtcHourEdit / 10) << 4) | (ds.rtcHourEdit % 10);
		i2cWriteReg(addr, DS3231_REG_HOURS, &regData, 1);
	}

	if (ds.f.rtcMinuteEditReadyToSet) {
		ds.f.rtcMinuteEditReadyToSet = 0;
		isValueSet = true;

		uint8_t regData = ((ds.rtcMinuteEdit / 10) << 4) | (ds.rtcMinuteEdit % 10);
		i2cWriteReg(addr, DS3231_REG_MINUTES, &regData, 1);
	}
}

void RTC::loopRead() {
	if (!isValueSet) {
		// Check A1F in STATUS
		uint8_t status;
		i2cReadReg(addr, DS3231_REG_STATUS, &status, 1);
		if (!(status & 0x01)) {
			return;
		}

		status = status & 0xFE;
		i2cWriteReg(addr, DS3231_REG_STATUS, &status, 1);
	}

	isValueSet = false;

	// Read all data.
	uint8_t data[7];
	i2cReadReg(addr, DS3231_REG_SECONDS, data, 7);

	uint16_t year = ((data[6] & 0xF0) >> 4) * 10 + (data[6] & 0x0F);
	if (0x80 & data[5]) {
		year += 2000;
	}
	else {
		year += 1900;
	}

	uint8_t month = ((data[5] & 0x10) >> 4) * 10 + (data[5] & 0x0F);
	uint8_t day = ((data[4] & 0x30) >> 4) * 10 + (data[4] & 0x0F);
	uint8_t weekday = (data[3] & 0x07);
	uint8_t hour = ((data[2] & 0x30) >> 4) * 10 + (data[2] & 0x0F);
	uint8_t minute = ((data[1] & 0x70) >> 4) * 10 + (data[1] & 0x0F);
	uint8_t second = ((data[0] & 0x70) >> 4) * 10 + (data[0] & 0x0F);

	ds.setRtcData(year, month, day, weekday, hour, minute, second);
}
