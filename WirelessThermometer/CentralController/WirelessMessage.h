#pragma once

#include <Arduino.h>

//
// Remote -> central controller messages.
//

#define MSG_R2C_THERMOMETER ((uint8_t)0x81)
#define MSG_R2C_LIGHT ((uint8_t)0x82)

//
// Central controller -> remote message.
//

#define MSG_C2R_LIGHT_POWER ((uint8_t)0x01)
#define MSG_C2R_LIGHT_LEVEL ((uint8_t)0x02)
#define MSG_C2R_RTC ((uint8_t)0x03)

//
// Light
//

#define WLMC_LIGHT_POWER_ON ((uint8_t)0x80)
#define WLMC_LIGHT_POWER_OFF ((uint8_t)0x00)

#define WLM_LIGHT_IS_POWER_ON(status) (((status) & WLMC_LIGHT_POWER_ON) == WLMC_LIGHT_POWER_ON)
#define WLM_LIGHT_LEVEL(status) (((uint8_t)(status)) & ~WLMC_LIGHT_POWER_ON)
#define WLM_LIGHT_SET_STATUS(isPowerOn, level) (((isPowerOn) ? WLMC_LIGHT_POWER_ON : WLMC_LIGHT_POWER_OFF) | (level))

//
// RTC
//

#define WLMC_RTC_HOUR_MASK ((uint8_t)0x80)  // 0:24hour, 1:12hour
#define WLMC_RTC_AMPM_MASK ((uint8_t)0x40)  // 0:AM, 1:PM

#define WLM_RTC_SET_HOUR_SETTING(is12H, isPM) ((uint8_t)(((is12H) ? WLMC_RTC_HOUR_MASK : 0)) | ((isPM) ? WLMC_RTC_AMPM_MASK : 0))
#define WLM_RTC_IS_12HOUR(hourSettings) (((hourSettings) & WLMC_RTC_HOUR_MASK) != 0)
#define WLM_RTC_IS_PM(hourSettings) (((hourSettings) & WLMC_RTC_AMPM_MASK) != 0)

struct WLM {
	uint8_t size;  // Message size in bytes.
	uint8_t type;  // One of MSG_* constants.

	WLM(uint8_t size, uint8_t type) : size(size), type(type) {
	}
};

struct WLM_R2C_THERMOMETER : WLM {
	uint8_t sensorStatus;
	int16_t temperature10Int;  // 10 times of temperature as integer, for example, 103=10.3'C; -123=-12.3'C
	int16_t humidity10Int;  // 10 times of humidity as integer, for example, 123=12.3%

	WLM_R2C_THERMOMETER(uint8_t sensorStatus, float temperature, float humidity)
		: WLM(sizeof(WLM_R2C_THERMOMETER), MSG_R2C_THERMOMETER)
		, sensorStatus(sensorStatus)
		, temperature10Int((int16_t)(10 * temperature))
		, humidity10Int((int16_t)(10 * humidity)) {
	}
};

struct WLM_R2C_LIGHT : WLM {
	uint8_t status;  // Bit 7: 1 for power ON, 0 for OFF, bit 6-0: level, depends on remote module.
	uint16_t percentage10Int;  // (0-1000) 10 times of lightness percentage, for example, 1000=100.0%, 123=12.3%

	WLM_R2C_LIGHT(bool isPowerOn, uint8_t level, float percentage)
		: WLM(sizeof(WLM_R2C_LIGHT), MSG_R2C_LIGHT)
		, status(WLM_LIGHT_SET_STATUS(isPowerOn, level))
		, percentage10Int((int16_t)(10 * percentage)) {
	}
};

struct WLM_C2R_LIGHT_POWER : WLM {
	uint8_t isPowerOn;  // 1 for ON, 0 for OFF.

	WLM_C2R_LIGHT_POWER(bool isPowerOn)
		: WLM(sizeof(WLM_C2R_LIGHT_POWER), MSG_C2R_LIGHT_POWER)
		, isPowerOn(isPowerOn ? 1 : 0) {
	}
};

struct WLM_C2R_LIGHT_LEVEL : WLM {
	uint8_t level;

	WLM_C2R_LIGHT_LEVEL(uint8_t level)
		: WLM(sizeof(WLM_C2R_LIGHT_LEVEL), MSG_C2R_LIGHT_LEVEL)
		, level(level) {
	}
};

struct WLM_C2R_RTC : WLM {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t weekday;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t hourSettings;

	WLM_C2R_RTC(
		uint16_t year, uint8_t month, uint8_t day, uint8_t weekday,
		uint8_t hour, uint8_t minute, uint8_t second,
		bool is12H, bool isPM)
		: WLM(sizeof(WLM_C2R_RTC), MSG_C2R_RTC)
		, year(year), month(month), day(day), weekday(weekday)
		, hour(hour), minute(minute), second(second)
		, hourSettings(WLM_RTC_SET_HOUR_SETTING(is12H, isPM)) {
	}
};
