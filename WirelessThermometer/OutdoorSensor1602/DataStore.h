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

	uint8_t /* S_RTC_xxx */ rtcStatus = S_RTC_TIMEOUT;
	uint8_t rtcMonth = 0;
	uint8_t rtcDay = 0;
	uint8_t rtcWeekday = 0;
	uint8_t rtcHour = 0;
	uint8_t rtcMinute = 0;
	uint32_t rtcLastUpdatedMillis = 0;

	struct {
		// For UI:
		uint8_t sensorValueReadyToShow : 1;
		uint8_t sensorStateReadyToShow : 1;
		uint8_t updateAnimReadyToShow : 1;
		uint8_t wirelessStateReadyToShow : 1;

		// For Wireless:
		uint8_t sensorValueReadyToSend : 1;

		// Fro RTC:
		uint8_t rtcStatusReadyToShow : 1;
		uint8_t rtcIndicatorReadyToShow : 1;
		uint8_t rtcMonthReadyToShow : 1;
		uint8_t rtcDayReadyToShow : 1;
		uint8_t rtcWeekdayReadyToShow : 1;
		uint8_t rtcHourReadyToShow : 1;
		uint8_t rtcMinutehReadyToShow : 1;
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
		uint8_t month, uint8_t day, uint8_t weekday,
		uint8_t hour, uint8_t minute
	) {
		if (month != rtcMonth) {
			rtcMonth = month;
			f.rtcMonthReadyToShow = 1;
		}
		if (day != rtcDay) {
			rtcDay = day;
			f.rtcDayReadyToShow = 1;
		}
		if (weekday != rtcWeekday) {
			rtcWeekday = weekday;
			f.rtcWeekdayReadyToShow = 1;
		}
		if (hour != rtcHour) {
			rtcHour = hour;
			f.rtcHourReadyToShow = 1;
		}
		if (minute != rtcMinute) {
			rtcMinute = minute;
			f.rtcMinutehReadyToShow = 1;
		}
		rtcLastUpdatedMillis = millis();
		f.rtcIndicatorReadyToShow = 1;
	}
};

extern DataStore ds;
