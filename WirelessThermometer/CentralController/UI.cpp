#include "UI.h"

#define UPDATE_ANIMATION_INTERVAL 500
#define MAX_SUMMARY_CHANNELS 4

#define CUSTOM_CHAR_ID_DEGC ((uint8_t)1)
#define CUSTOM_CHAR_ID_DEGF ((uint8_t)2)

static uint8_t getChannelThemometerRow(uint8_t channel) {
	switch (channel) {
	case 0: return 0;
	case 1: return 3;
	case 2:
	case 3:
	case 4: return 2;
	default: return -1;
	}
}

void UI::setup() {
	pinMode(pinLed, OUTPUT);
	digitalWrite(pinLed, HIGH);

	lcd.begin(20, 4);

	uint8_t ccDegC[] = { 0x18, 0x18, 0x03, 0x04, 0x04, 0x04, 0x03, 0x00 };
	lcd.createChar(CUSTOM_CHAR_ID_DEGC, ccDegC);

	uint8_t ccDegF[] = { 0x18, 0x18, 0x07, 0x04, 0x07, 0x04, 0x04, 0x00 };
	lcd.createChar(CUSTOM_CHAR_ID_DEGF, ccDegF);

	uint8_t ccErr[] = { 0x1F, 0x00, 0x1F, 0x00, 0x1F, 0x00, 0x1F, 0x00 };
	lcd.createChar(3, ccErr);

	// Force an update.
	for (uint8_t i = 0; i < MAX_CHANNELS; i++) {
		ds.cd[i].f.statusReadyToShow = 1;
		ds.cd[i].f.thermometerDataReadyToShow = 1;
	}
	ds.f.uiStageChanged = 1;
}

void UI::loop() {
	loopChannelThemometerData(0, false);
	loopChannelUpdateAnimation(0);

	bool forceUpate = ds.f.uiStageChanged;
	if (ds.f.uiStageChanged) {
		ds.f.uiStageChanged = 0;
		clearRow(1);
		clearRow(2);
		clearRow(3);
		writeRemoteTitle();
	}

	switch (ds.uiStage)
	{
	case UI_STAGE_0:
		loopChannelThemometerData(1, forceUpate);
		loopChannelUpdateAnimation(1);
		loopRtcData(forceUpate);
		loopRtcAnimation();
		break;
	case UI_STAGE_1:
		loopChannelThemometerData(2, forceUpate);
		loopChannelUpdateAnimation(2);
		loopChannelLightData(2, forceUpate);
		break;
	case UI_STAGE_2:
		loopChannelThemometerData(3, forceUpate);
		loopChannelUpdateAnimation(3);
		loopChannelLightData(3, forceUpate);
		break;
	case UI_STAGE_3:
		loopChannelThemometerData(4, forceUpate);
		loopChannelUpdateAnimation(4);
		loopChannelLightData(4, forceUpate);
		break;
	}
}

void UI::loopChannelThemometerData(uint8_t channel, bool forceUpdate) {
	ChannelData &cd = ds.cd[channel];
	uint8_t row = getChannelThemometerRow(channel);

	if (cd.f.statusReadyToShow || forceUpdate) {
		cd.f.statusReadyToShow = 0;

		clearRow(row);

		switch (cd.status) {
		case CS_OK:
			writeUnit(row);
			break;

		case CS_SENSOR_ERROR:
			lcd.setCursor(1, row);
			lcd.print(" \x03 SENSOR ERROR \x03");
			break;

		case CS_NO_SIGNAL:
			lcd.setCursor(1, row);
			lcd.print("  \x03 NO SIGNAL \x03");
			break;
		}
	}

	if (cd.status == CS_OK) {
		if (cd.f.thermometerDataReadyToShow || forceUpdate) {
			cd.f.thermometerDataReadyToShow = 0;
			writeThemometerData(row, cd.temperature, cd.humidity);
		}
	}
}

void UI::loopChannelUpdateAnimation(uint8_t channel) {
	ChannelData &cd = ds.cd[channel];

	if (!cd.updateAnimationStep) {
		return;
	}

	uint8_t row = getChannelThemometerRow(channel);

	switch (cd.updateAnimationStep) {
	case 1:
		lcd.setCursor(0, row);
		lcd.print(">");
		cd.updateAnimationMillis = millis();
		cd.updateAnimationStep = 2;
		break;
	case 2:
		if (millis() - cd.updateAnimationMillis > UPDATE_ANIMATION_INTERVAL) {
			lcd.setCursor(0, row);
			lcd.print(" ");
			cd.updateAnimationStep = 0;
		}
		break;
	}
}

void UI::loopChannelLightData(uint8_t channel, bool forceUpdate) {
	ChannelData &cd = ds.cd[channel];

	if (cd.f.lightStatusReadyToShow || forceUpdate) {
		forceUpdate = cd.lightStatus == CS_LIGHT_OK;
		clearRow(3);
	}

	if (cd.status == CS_OK) {
		cd.f.lightStatusReadyToShow = 0;
		if (cd.f.lightPowerReadyToShow || forceUpdate) {
			cd.f.lightPowerReadyToShow = 0;
			lcd.setCursor(0, 3);
			lcd.print("Light is ");
			if (cd.lightIsOn) {
				lcd.print("ON. ");
			}
			else {
				lcd.print("OFF.");
			}
		}
		if (cd.f.lightLevelReadyToShow || forceUpdate) {
			cd.f.lightLevelReadyToShow = 0;
			char str[4] = { 0 };
			snprintf(str, 5, "%3d", (uint16_t)cd.lightPercentage);
			lcd.setCursor(16, 3);
			lcd.print(str);
			lcd.print("%");
		}
	}
}

void UI::loopRtcData(bool forceUpdate) {
	if (forceUpdate) {
		lcd.setCursor(0, 1);
		lcd.print("                    ");
	}

	if (!ds.isValidRtc()) {
		return;
	}

	uint8_t dayPos = ds.rtcMonth > 9 ? 8 : 7;

	if (forceUpdate) {
		lcd.setCursor(4, 1);
		lcd.print("-");
	}

	if (forceUpdate || ds.f.rtcMonthReadyToShow || ds.f.rtcDayReadyToShow) {
		ds.f.rtcMonthReadyToShow = 1;
		ds.f.rtcDayReadyToShow = 1;
		lcd.setCursor(5, 1);
		lcd.print("     ");
		lcd.setCursor(dayPos - 1, 1);
		lcd.print("-");
	}

	if (ds.f.rtcYearReadyToShow || forceUpdate) {
		ds.f.rtcYearReadyToShow = 0;
		writeYear(true);
	}

	if (ds.rtcEditingTarget == UI_RTC_EDT_YEAR) {
		if (millis() - ds.rtcEditingAnimationMillis > UPDATE_ANIMATION_INTERVAL) {
			ds.rtcEditingAnimationMillis = millis();

			switch (ds.rtcEditingAnimationStep) {
			case 1: writeYear(false); ds.rtcEditingAnimationStep = 2; break;
			case 2: ds.f.rtcYearReadyToShow = 1; ds.rtcEditingAnimationStep = 1; break;
			}
		}
	}

	if (ds.f.rtcMonthReadyToShow || forceUpdate) {
		ds.f.rtcMonthReadyToShow = 0;
		writeMonth(true);
	}

	if (ds.rtcEditingTarget == UI_RTC_EDT_MONTH) {
		if (millis() - ds.rtcEditingAnimationMillis > UPDATE_ANIMATION_INTERVAL) {
			ds.rtcEditingAnimationMillis = millis();

			switch (ds.rtcEditingAnimationStep) {
			case 1: writeMonth(false); ds.rtcEditingAnimationStep = 2; break;
			case 2: ds.f.rtcMonthReadyToShow = 1; ds.rtcEditingAnimationStep = 1; break;
			}
		}
	}

	if (ds.f.rtcDayReadyToShow || forceUpdate) {
		ds.f.rtcDayReadyToShow = 0;
		writeDay(true, dayPos);
	}

	if (ds.rtcEditingTarget == UI_RTC_EDT_DAY) {
		if (millis() - ds.rtcEditingAnimationMillis > UPDATE_ANIMATION_INTERVAL) {
			ds.rtcEditingAnimationMillis = millis();

			switch (ds.rtcEditingAnimationStep) {
			case 1: writeDay(false, dayPos); ds.rtcEditingAnimationStep = 2; break;
			case 2: ds.f.rtcDayReadyToShow = 1; ds.rtcEditingAnimationStep = 1; break;
			}
		}
	}

	if (ds.f.rtcWeekdayReadyToShow || forceUpdate) {
		ds.f.rtcWeekdayReadyToShow = 0;
		writeWeekday(true);
	}

	if (ds.rtcEditingTarget == UI_RTC_EDT_WEEKDAY) {
		if (millis() - ds.rtcEditingAnimationMillis > UPDATE_ANIMATION_INTERVAL) {
			ds.rtcEditingAnimationMillis = millis();

			switch (ds.rtcEditingAnimationStep) {
			case 1: writeWeekday(false); ds.rtcEditingAnimationStep = 2; break;
			case 2: ds.f.rtcWeekdayReadyToShow = 1; ds.rtcEditingAnimationStep = 1; break;
			}
		}
	}

	if (ds.f.rtcHourReadyToShow || forceUpdate) {
		ds.f.rtcHourReadyToShow = 0;
		writeHour(true);
	}

	if (ds.rtcEditingTarget == UI_RTC_EDT_HOUR) {
		if (millis() - ds.rtcEditingAnimationMillis > UPDATE_ANIMATION_INTERVAL) {
			ds.rtcEditingAnimationMillis = millis();

			switch (ds.rtcEditingAnimationStep) {
			case 1: writeHour(false); ds.rtcEditingAnimationStep = 2; break;
			case 2: ds.f.rtcHourReadyToShow = 1; ds.rtcEditingAnimationStep = 1; break;
			}
		}
	}

	if (ds.f.rtcMinuteReadyToShow || forceUpdate) {
		ds.f.rtcMinuteReadyToShow = 0;
		writeMinute(true);
	}

	if (ds.rtcEditingTarget == UI_RTC_EDT_MINUTE) {
		if (millis() - ds.rtcEditingAnimationMillis > UPDATE_ANIMATION_INTERVAL) {
			ds.rtcEditingAnimationMillis = millis();

			switch (ds.rtcEditingAnimationStep) {
			case 1: writeMinute(false); ds.rtcEditingAnimationStep = 2; break;
			case 2: ds.f.rtcMinuteReadyToShow = 1; ds.rtcEditingAnimationStep = 1; break;
			}
		}
	}
}

void UI::loopRtcAnimation() {
	switch (ds.rtcAnimationStep) {
	case 1:
		lcd.setCursor(17, 1);
		lcd.print(":");
		ds.rtcAnimationMillis = millis();
		ds.rtcAnimationStep = 2;
		break;
	case 2:
		if (millis() - ds.rtcAnimationMillis > UPDATE_ANIMATION_INTERVAL
			&& ds.rtcEditingTarget == UI_RTC_EDT_NONE) {
			lcd.setCursor(17, 1);
			lcd.print(" ");
			ds.rtcAnimationStep = 0;
		}
		break;
	}
}

void UI::clearRow(uint8_t row) {
	lcd.setCursor(0, row);
	lcd.print("                    ");
}

void UI::writeUnit(uint8_t row) {
	lcd.setCursor(4, row);
	lcd.write(CUSTOM_CHAR_ID_DEGC);
	lcd.setCursor(10, row);
	lcd.write(CUSTOM_CHAR_ID_DEGF);
	lcd.setCursor(16, row);
	lcd.print("%rH");
}

void UI::writeThemometerData(uint8_t row, float t, float h) {
	char str[4] = { 0 };

	if (IS_VALID_TEMPERATURE(t)) {
		int16_t tempC = (int16_t)t;
		int16_t tempF = (int16_t)(t * 9 / 5 + 32);

		memset(str, 0, 4);
		snprintf(str, 4, "%3d", tempC);
		lcd.setCursor(1, row);
		lcd.print(str);

		memset(str, 0, 4);
		snprintf(str, 4, "%3d", tempF);
		lcd.setCursor(7, row);
		lcd.print(str);
	}
	else {
		lcd.setCursor(1, row);
		lcd.print(" --");
		lcd.setCursor(7, row);
		lcd.print(" --");
	}

	if (IS_VALID_HUMIDITY(h)) {
		int16_t hum = (int16_t)h;
		memset(str, 0, 4);
		snprintf(str, 4, "%3d", hum);
		lcd.setCursor(13, row);
		lcd.print(str);
	}
	else {
		lcd.setCursor(13, row);
		lcd.print(" --");
	}
}

void UI::writeRemoteTitle() {
	switch (ds.uiStage) {
	case UI_STAGE_0:
		lcd.setCursor(4, 2);
		lcd.print("[ Outdoor ]");
		break;
	case UI_STAGE_1:
		lcd.setCursor(4, 1);
		lcd.print("[ Room 1 ]");
		break;
	case UI_STAGE_2:
		lcd.setCursor(4, 1);
		lcd.print("[ Room 2 ]");
		break;
	case UI_STAGE_3:
		lcd.setCursor(4, 1);
		lcd.print("[ Room 3 ]");
		break;
	}
}

void UI::writeYear(bool show) {
	lcd.setCursor(0, 1);
	if (show) {
		char str[5] = { 0 };
		snprintf(str, 5, "%4d", ds.rtcYear);
		lcd.print(str);
	}
	else {
		lcd.print("    ");
	}
}

void UI::writeMonth(bool show) {
	lcd.setCursor(5, 1);
	if (show) {
		char str[3] = { 0 };
		snprintf(str, 3, "%d", ds.rtcMonth);
		lcd.print(str);
	}
	else {
		if (ds.rtcMonth > 9) {
			lcd.print("  ");
		}
		else {
			lcd.print(" ");
		}
	}
}

void UI::writeDay(bool show, uint8_t pos) {
	lcd.setCursor(pos, 1);
	if (show) {
		char str[3] = { 0 };
		snprintf(str, 3, "%d", ds.rtcDay);
		lcd.print(str);
	}
	else {
		if (ds.rtcDay > 9) {
			lcd.print("  ");
		}
		else {
			lcd.print(" ");
		}
	}
}

void UI::writeWeekday(bool show) {
	lcd.setCursor(11, 1);
	if (show) {
		switch (ds.rtcWeekday) {
		case 1: lcd.print("Mon"); break;
		case 2: lcd.print("Tue"); break;
		case 3: lcd.print("Wed"); break;
		case 4: lcd.print("Thu"); break;
		case 5: lcd.print("Fri"); break;
		case 6: lcd.print("Sat"); break;
		case 7: lcd.print("Sun"); break;
		}
	}
	else {
		lcd.print("   ");
	}
}

void UI::writeHour(bool show) {
	lcd.setCursor(15, 1);
	if (show) {
		char str[3] = { 0 };
		sprintf(str, "%2d", ds.rtcHour);
		lcd.print(str);
	}
	else {
		lcd.print("  ");
	}
}

void UI::writeMinute(bool show) {
	lcd.setCursor(18, 1);
	if (show) {
		char str[3] = { 0 };
		sprintf(str, "%02d", ds.rtcMinute);
		lcd.print(str);
	}
	else {
		lcd.print("  ");
	}
}
