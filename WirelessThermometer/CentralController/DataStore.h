#pragma once

#include <Arduino.h>

#define MAX_CHANNELS 6  // 1 centrol + 5 remotes

#define CS_OK ((uint8_t)0)
#define CS_NO_SIGNAL ((uint8_t)1)
#define CS_SENSOR_ERROR ((uint8_t)2)

#define CS_LIGHT_OK ((uint8_t)0)
#define CS_LIGHT_TIMEOUT ((uint8_t)1)

#define INVALID_TEMPERATURE ((float)-100.0f)
#define INVALID_HUMIDITY ((float)-1.0f)

#define IS_VALID_TEMPERATURE(t) ((t) >= -40 && (t) <= 80)
#define IS_VALID_HUMIDITY(h) ((h) >= 0 && (h) <= 100)

#define UI_STAGE_0 ((uint8_t)0x00)  // Show remote A, outdoor.
#define UI_STAGE_1 ((uint8_t)0x01)  // Show remote B, room 1.
#define UI_STAGE_2 ((uint8_t)0x02)  // Show remote C, room 2.
#define UI_STAGE_3 ((uint8_t)0x03)  // Show remote D, room 3.
#define UI_STAGE_4 ((uint8_t)0x04)  // Show remote E, light switch.
#define UI_STAGE_MAX UI_STAGE_4

#define UI_RTC_EDT_NONE    ((uint8_t)0x00)  // Editing target: none.
#define UI_RTC_EDT_YEAR    ((uint8_t)0x01)  // Editing target: year.
#define UI_RTC_EDT_MONTH   ((uint8_t)0x02)  // Editing target: month.
#define UI_RTC_EDT_DAY     ((uint8_t)0x03)  // Editing target: day.
#define UI_RTC_EDT_WEEKDAY ((uint8_t)0x04)  // Editing target: weekday.
#define UI_RTC_EDT_HOUR    ((uint8_t)0x05)  // Editing target: hour.
#define UI_RTC_EDT_MINUTE  ((uint8_t)0x06)  // Editing target: minute.

struct ChannelData {
	uint8_t /* CS_xxx */ status = CS_OK;

	float temperature = INVALID_TEMPERATURE;
	float humidity = INVALID_HUMIDITY;
	uint32_t thermometerDataUdpatedMillis = 0;

	uint8_t /* CS_LIGHT_xxx */ lightStatus = CS_LIGHT_TIMEOUT;
	bool lightIsOn = false;
	uint8_t lightLevel = 0;
	float lightPercentage = 0;
	uint32_t lightDataUpdatedMillis = 0;

	bool lightControlIsOn = false;
	uint8_t lightControlLevel = 0;

	uint8_t updateAnimationStep = 0;
	uint32_t updateAnimationMillis = 0;

	struct {
		uint8_t statusReadyToShow : 1;
		uint8_t thermometerDataReadyToShow : 1;
		uint8_t lightStatusReadyToShow : 1;
		uint8_t lightPowerReadyToShow : 1;
		uint8_t lightLevelReadyToShow : 1;

		uint8_t lightPowerControlReadyToSend : 1;
		uint8_t lightLevelControlReadyToSend : 1;
	} f = { 0 };

	void setStatus(uint8_t /* CS_xxx */ s) {
		if (s != status) {
			status = s;
			f.statusReadyToShow = 1;
		}
	}

	void setThermometerData(float t, float h) {
		temperature = t;
		humidity = h;
		f.thermometerDataReadyToShow = 1;
		if (!updateAnimationStep) {
			updateAnimationStep = 1;
		}
		thermometerDataUdpatedMillis = millis();
	}

	void setLightStatus(uint8_t /* CS_LIGHT_xxx */ status) {
		if (status != lightStatus) {
			lightStatus = status;
			f.lightStatusReadyToShow = 1;
		}
	}

	void setLightPower(bool isOn) {
		if (isOn != lightIsOn) {
			lightIsOn = isOn;
			f.lightPowerReadyToShow = 1;
		}
		lightDataUpdatedMillis = millis();
	}

	void setLightLevel(uint8_t level, float percentage) {
		if (level != lightLevel || percentage != lightPercentage) {
			lightLevel = level;
			lightPercentage = percentage;
			f.lightLevelReadyToShow = 1;
		}
		lightDataUpdatedMillis = millis();
	}

	void setLightControlIsOn(bool isOn) {
		lightControlIsOn = isOn;
		f.lightPowerControlReadyToSend = 1;
	}

	void setLightControlLevel(uint8_t level) {
		lightControlLevel = level;
		f.lightLevelControlReadyToSend = 1;
	}
};

struct DataStore {
	ChannelData cd[MAX_CHANNELS];
	uint8_t /* UI_STAGE_xxx */ uiStage = UI_STAGE_0;

	uint8_t /* UI_RTC_ET_xxx */ rtcEditingTarget = UI_RTC_EDT_NONE;
	uint32_t rtcEditingAnimationMillis = 0;
	uint8_t rtcEditingAnimationStep = 0;

	uint16_t rtcYear = 0xFFFF;
	uint8_t rtcMonth = 0xFF;
	uint8_t rtcDay = 0xFF;
	uint8_t rtcWeekday = 0xFF;
	uint8_t rtcHour = 0xFF;
	uint8_t rtcMinute = 0xFF;
	uint8_t rtcSecond = 0xFF;
	uint8_t rtcAnimationStep = 0;
	uint32_t rtcAnimationMillis = 0;

	uint16_t rtcYearEdit = 0xFFFF;
	uint8_t rtcMonthEdit = 0xFF;
	uint8_t rtcDayEdit = 0xFF;
	uint8_t rtcWeekdayEdit = 0xFF;
	uint8_t rtcHourEdit = 0xFF;
	uint8_t rtcMinuteEdit = 0xFF;

	struct {
		uint8_t uiStageChanged : 1;

		uint8_t rtcEditingChanged : 1;
		uint8_t rtcYearReadyToShow : 1;
		uint8_t rtcMonthReadyToShow : 1;
		uint8_t rtcDayReadyToShow : 1;
		uint8_t rtcWeekdayReadyToShow : 1;
		uint8_t rtcHourReadyToShow : 1;
		uint8_t rtcMinuteReadyToShow : 1;
		uint8_t rtcSecondReadyToShow : 1;

		uint8_t rtcYearEditReadyToSet : 1;
		uint8_t rtcMonthEditReadyToSet : 1;
		uint8_t rtcDayEditReadyToSet : 1;
		uint8_t rtcWeekdayEditReadyToSet : 1;
		uint8_t rtcHourEditReadyToSet : 1;
		uint8_t rtcMinuteEditReadyToSet : 1;

		uint8_t rtcDataReadyToSend : 1;
	} f = { 0 };

	void setUIStage(uint8_t /* UI_STAGE_xxx */ s) {
		if (s >= UI_STAGE_0 && s <= UI_STAGE_MAX && s != uiStage) {
			uiStage = s;
			f.uiStageChanged = 1;
		}
	}

	void setRtcEditing(uint8_t /* UI_RTC_ET_xxx */ target) {
		if (target != rtcEditingTarget) {
			rtcEditingTarget = target;
		}
	}

	void setRtcData(
		uint16_t year, uint8_t month, uint8_t day, uint8_t weekday,
		uint8_t hour, uint8_t minute, uint8_t second
	) {
		if (year != rtcYear) {
			f.rtcYearReadyToShow = 1;
		}
		if (month != rtcMonth) {
			f.rtcMonthReadyToShow = 1;
		}
		if (day != rtcDay) {
			f.rtcDayReadyToShow = 1;
		}
		if (weekday != rtcWeekday) {
			f.rtcWeekdayReadyToShow = 1;
		}
		if (hour != rtcHour) {
			f.rtcHourReadyToShow = 1;
		}
		if (minute != rtcMinute) {
			f.rtcMinuteReadyToShow = 1;
		}
		if (second != rtcSecond) {
			f.rtcSecondReadyToShow = 1;
		}

		rtcYear = year, rtcMonth = month, rtcDay = day, rtcWeekday = weekday;
		rtcHour = hour, rtcMinute = minute, rtcSecond = second;

		if (!rtcAnimationStep) {
			rtcAnimationStep = 1;
		}
		f.rtcDataReadyToSend = 1;
	}

	bool isValidRtc() {
		return (
			rtcYear >= 1900 && rtcYear <= 2099 &&
			rtcMonth >= 1 && rtcMonth <= 12 &&
			rtcDay >= 1 && rtcDay <= 31 &&
			rtcWeekday >= 1 && rtcWeekday <= 7 &&
			rtcHour < 24 &&
			rtcMinute < 60 &&
			rtcSecond < 60);
	}
};

extern DataStore ds;
