// DataStore.h
#pragma once

#include <Arduino.h>

#define S_S_OK ((uint8_t)0)
#define S_S_ERROR ((uint8_t)1)

#define S_WL_OK ((uint8_t)0)
#define S_WL_NOCHIP ((uint8_t)1)
#define S_WL_NOSIGNAL ((uint8_t)2)

#define S_RTC_OK ((uint8_t)0)
#define S_RTC_TIMEOUT ((uint8_t)1)

#define INVALID_TEMPERATURE ((float)-100.0f)
#define INVALID_HUMIDITY ((float)-1.0f)

#define IS_VALID_TEMPERATURE(t) ((t) >= -40 && (t) <= 80)
#define IS_VALID_HUMIDITY(h) ((h) >= 0 && (h) <= 100)

class DataStore {
public:
	float temperature = INVALID_TEMPERATURE;
	float humidity = INVALID_HUMIDITY;
	uint8_t sensorResult = 0;

	uint8_t /* S_S_xxx */ sensorState = S_S_OK;
	uint8_t /* S_WL_xxx */ wirelessState = S_WL_OK;

	bool lightIsOn = false;
	uint8_t lightLevel = 0;
	float lightPercentage = 0;

	bool lightControlIsOn = false;
	uint8_t lightControlLevel = 0;

	uint8_t /* S_RTC_xxx */ rtcStatus = S_RTC_TIMEOUT;
	uint16_t rtcYear = 0xFFFF;
	uint8_t rtcMonth = 0xFF;
	uint8_t rtcDay = 0xFF;
	uint8_t rtcWeekday = 0xFF;
	uint8_t rtcHour = 0xFF;
	uint8_t rtcMinute = 0xFF;
	uint32_t rtcLastUpdatedMillis = 0;

	struct {
		// For UI:
		uint8_t sensorValueReadyToShow : 1;
		uint8_t sensorStateReadyToShow : 1;
		uint8_t updateAnimReadyToShow : 1;
		uint8_t wirelessStateReadyToShow : 1;

		// For Wireless:
		uint8_t sensorValueReadyToSend : 1;
		uint8_t lightValueReadyToSend : 1;

		// For light:
		uint8_t lightControlIsOnReadyToSet : 1;
		uint8_t lightControlLevelReadyToSet : 1;

		// Fro RTC:
		uint8_t rtcStatusReadyToShow : 1;
		uint8_t rtcDateReadyToShow : 1;
		uint8_t rtcWeekdayReadyToShow : 1;
		uint8_t rtcTimeReadyToShow : 1;
	} f = { 0 };

public:
	void setSensorResult(
		uint8_t r,
		float t = INVALID_TEMPERATURE,
		float h = INVALID_HUMIDITY
	) {
		sensorResult = r;
		temperature = t;
		humidity = h;
		f.sensorValueReadyToSend = 1;
		f.sensorValueReadyToShow = 1;
		f.updateAnimReadyToShow = 1;
	}

	void setSensorState(uint8_t state) {
		if (state != sensorState) {
			sensorState = state;
			f.sensorStateReadyToShow = 1;
		}
	}

	void setWirelessState(uint8_t state) {
		if (state != wirelessState) {
			wirelessState = state;
			f.wirelessStateReadyToShow = 1;
		}

		if (state != S_WL_OK) {
			setRtcStatus(S_RTC_TIMEOUT);
		}
	}

	void setRtcStatus(uint8_t /* S_RTC_xxx */ status) {
		if (status != rtcStatus) {
			rtcStatus = status;
			f.rtcStatusReadyToShow = 1;
		}
	}

	void setRtcData(
		uint16_t year, uint8_t month, uint8_t day, uint8_t weekday,
		uint8_t hour, uint8_t minute
	) {
		f.rtcDateReadyToShow =
			year != rtcYear || month != rtcMonth || day != rtcDay ? 1 : 0;
		f.rtcWeekdayReadyToShow = weekday != rtcWeekday ? 1 : 0;
		f.rtcTimeReadyToShow = hour != rtcHour || minute != rtcMinute ? 1 : 0;

		rtcYear = year;
		rtcMonth = month;
		rtcDay = day;
		rtcWeekday = weekday;
		rtcHour = hour;
		rtcMinute = minute;

		rtcLastUpdatedMillis = millis();
	}
};

extern DataStore ds;
