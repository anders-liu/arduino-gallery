#include "UI.h"

#define ANIM_INTERVAL 500
#define INDICATOR_INTERVAL 500

#define CUSTOM_CHAR_ID_DEGC ((byte)1)
#define CUSTOM_CHAR_ID_DEGF ((byte)2)
#define CUSTOM_CHAR_ID_ANT ((byte)7)

void UI::setup() {
	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, HIGH);

	lcd.begin(16, 2);

	byte ccDegC[] = { 0x18, 0x18, 0x03, 0x04, 0x04, 0x04, 0x03, 0x00 };
	lcd.createChar(CUSTOM_CHAR_ID_DEGC, ccDegC);

	byte ccDegF[] = { 0x18, 0x18, 0x07, 0x04, 0x07, 0x04, 0x04, 0x00 };
	lcd.createChar(CUSTOM_CHAR_ID_DEGF, ccDegF);

	uint8_t ccErr[] = { 0x1F, 0x00, 0x1F, 0x00, 0x1F, 0x00, 0x1F, 0x00 };
	lcd.createChar(3, ccErr);

	byte ccAntenna[] = { 0x04, 0x15, 0x0E, 0x04, 0x04, 0x04, 0x04, 0x00 };
	lcd.createChar(CUSTOM_CHAR_ID_ANT, ccAntenna);
}

void UI::loopSensor() {
	if (ds.f.sensorStateReadyToShow) {
		ds.f.sensorStateReadyToShow = 0;
		lcd.setCursor(1, 0);
		lcd.print("               ");

		if (ds.sensorState == S_S_OK) {
			writeUnits();
		}
	}

	switch (ds.sensorState)
	{
	case S_S_OK:
		if (ds.f.sensorValueReadyToShow) {
			ds.f.sensorValueReadyToShow = 0;
			writeValue();
		}
		break;

	case S_S_ERROR:
		lcd.setCursor(2, 0);
		lcd.print("SENSOR ERROR");
		break;
	}
}

void UI::loopWireless() {
	if (ds.f.wirelessStateReadyToShow) {
		ds.f.wirelessStateReadyToShow = 0;

		lcd.setCursor(0, 1);
		switch (ds.wirelessState)
		{
		case S_WL_OK:       lcd.print("                "); break;
		case S_WL_NOSIGNAL: lcd.print("    NO SIGNAL   "); break;
		case S_WL_NOCHIP:   lcd.print("     NO CHIP    "); break;
		default:            lcd.print("     UNKNOWN    "); break;
		}
	}
}

void UI::loopAnimation() {
	if (animStep == 0) return;

	uint32_t ms = millis();
	if (ms - lastAnimMillis > ANIM_INTERVAL) {
		lastAnimMillis = ms;

		switch (animStep)
		{
		case 1:
			lcd.setCursor(0, 0);
			lcd.print(">");
			animStep = 2;
			break;

		case 2:
			lcd.setCursor(0, 0);
			lcd.print(" ");
			animStep = 0;
			break;
		}
	}
}

void UI::loopRtc() {
	if (ds.wirelessState != S_WL_OK) {
		return;
	}

	bool forceUpdate = ds.f.rtcStatusReadyToShow
		|| ds.f.sensorStateReadyToShow
		|| ds.f.wirelessStateReadyToShow;

	if (ds.f.rtcStatusReadyToShow) {
		ds.f.rtcStatusReadyToShow = 0;
		lcd.setCursor(0, 1);
		lcd.print("                ");
		if (ds.rtcStatus != S_RTC_OK) {
			return;
		}
	}

	if (ds.f.rtcMonthReadyToShow || forceUpdate) {
		ds.f.rtcMonthReadyToShow = 0;
		lcd.setCursor(0, 1);
		switch (ds.rtcMonth) {
		case  1: lcd.print("Jan."); break;
		case  2: lcd.print("Feb."); break;
		case  3: lcd.print("Mar."); break;
		case  4: lcd.print("Apr."); break;
		case  5: lcd.print("May."); break;
		case  6: lcd.print("Jun."); break;
		case  7: lcd.print("Jul."); break;
		case  8: lcd.print("Aug."); break;
		case  9: lcd.print("Sep."); break;
		case 10: lcd.print("Oct."); break;
		case 11: lcd.print("Nov."); break;
		case 12: lcd.print("Dec."); break;
		}
	}

	if (ds.f.rtcDayReadyToShow || forceUpdate) {
		ds.f.rtcDayReadyToShow = 0;
		char str[3] = { 0 };
		sprintf(str, "%-2d", ds.rtcDay);
		lcd.setCursor(4, 1);
		lcd.print(str);
	}

	if (ds.f.rtcWeekdayReadyToShow || forceUpdate) {
		ds.f.rtcWeekdayReadyToShow = 0;
		lcd.setCursor(7, 1);
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

	if (ds.f.rtcHourReadyToShow || forceUpdate) {
		ds.f.rtcHourReadyToShow = 0;
		char str[3] = { 0 };
		sprintf(str, "%2d", ds.rtcHour);
		lcd.setCursor(11, 1);
		lcd.print(str);
	}

	if (ds.f.rtcMinutehReadyToShow || forceUpdate) {
		ds.f.rtcMinutehReadyToShow = 0;
		char str[3] = { 0 };
		sprintf(str, "%02d", ds.rtcMinute);
		lcd.setCursor(14, 1);
		lcd.print(str);
	}
}

void UI::loopRtcIndicator() {
	if (ds.wirelessState != S_WL_OK) {
		return;
	}

	if (ds.f.rtcIndicatorReadyToShow) {
		if (rtcIndicatorStep == 0) {
			rtcIndicatorStep = 1;
			ds.f.rtcIndicatorReadyToShow = 0;
		}
	}

	uint32_t ms = millis();
	if (ms - lastRtcIndicatorMillis < INDICATOR_INTERVAL) {
		return;
	}

	lastRtcIndicatorMillis = ms;
	lcd.setCursor(13, 1);
	switch (rtcIndicatorStep) {
	case 1: lcd.print(":"); rtcIndicatorStep = 2; break;
	case 2: lcd.print(" "); rtcIndicatorStep = 0; break;
	}
}

void UI::writeUnits() {
	lcd.setCursor(5, 0);
	lcd.write(CUSTOM_CHAR_ID_DEGC);
	lcd.setCursor(10, 0);
	lcd.write(CUSTOM_CHAR_ID_DEGF);
	lcd.setCursor(15, 0);
	lcd.write('%');
}

void UI::writeValue() {
	char str[4] = { 0 };

	if (IS_VALID_TEMPERATURE(ds.temperature)) {
		int16_t tempC = (int16_t)ds.temperature;
		int16_t tempF = (int16_t)(ds.temperature * 9 / 5 + 32);

		memset(str, 0, 4);
		snprintf(str, 4, "%3d", tempC);
		lcd.setCursor(2, 0);
		lcd.print(str);

		memset(str, 0, 4);
		snprintf(str, 4, "%3d", tempF);
		lcd.setCursor(7, 0);
		lcd.print(str);
	}
	else {
		lcd.setCursor(2, 0);
		lcd.print(" --");
		lcd.setCursor(7, 0);
		lcd.print(" --");
	}

	if (IS_VALID_HUMIDITY(ds.humidity)) {
		int16_t hum = (int16_t)ds.humidity;
		memset(str, 0, 4);
		snprintf(str, 4, "%3d", hum);
		lcd.setCursor(12, 0);
		lcd.print(str);
	}
	else {
		lcd.setCursor(12, 0);
		lcd.print(" --");
	}
}
