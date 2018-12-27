#include "IR.h"

static IRKey getKey(uint32_t value) {
	switch (value) {
		//	case 0xE0E008F7:  // SAMSUNG.CH-
	case 0xE0E0A659:  // SAMSUNG.Left
	case 0x000860B5:  // XIOAMI1.Left
		return IRKEY_LEFT;
		//	case 0xE0E048B7:  // SAMSUNG.CH+
	case 0xE0E046B9:  // SAMSUNG.Right
//	case 0xE0E0807F:  // SAMSUNG.Source
	case 0x000860C2:  // XIOAMI1.Right
		return IRKEY_RIGHT;
		//	case 0xE0E0E01F:  // SAMSUNG.Vol+
	case 0xE0E006F9:  // SAMSUNG.Up
	case 0x0008605B:  // XIOAMI1.Up
//	case 0x000860E0:  // XIOAMI1.+
		return IRKEY_UP;
		//	case 0xE0E0D02F:  // SAMSUNG.Vol-
	case 0xE0E08679:  // SAMSUNG.Down
	case 0x00086068:  // XIOAMI1.Down
//	case 0x000860F1:  // XIOAMI1.-
		return IRKEY_DOWN;
		//	case 0xE0E040BF:  // SAMSUNG.Power
	case 0xE0E016E9:  // SAMSUNG.Enter
//	case 0x0003CCCF:  // XIOAMI1.Power
	case 0x000860D3:  // XIOAMI1.OK
		return IRKEY_OK;
	case 0xE0E036C9:  // SAMSUNG.A
		return IRKEY_A;
	case 0xE0E028D7:  // SAMSUNG.B
		return IRKEY_B;
	case 0xE0E0A857:  // SAMSUNG.C
		return IRKEY_C;
	case 0xE0E06897:  // SAMSUNG.D
		return IRKEY_D;
	case 0xE0E0F00F:  // SAMSUNG.Mute
		return IRKEY_TEST;
	default:
		return IRKEY_UNKNOW;
	}
}
void IR::setup() {
	irRecv.enableIRIn();
}

void IR::loop() {
	decode_results result;

	if (millis() - lastMillis > 300) {
		lastCodeValue = 0;
		lastMillis = millis();
	}

	if (irRecv.decode(&result)) {
		if (result.value != lastCodeValue) {
			lastCodeValue = result.value;
			IRKey key = getKey(lastCodeValue);
			onKeyDown(key);
		}

		irRecv.resume();
		lastMillis = millis();
	}
}

void IR::onKeyDown(IRKey key) {
	switch (key) {
	case IRKEY_LEFT:
		switch (ds.uiStage) {
		case UI_STAGE_0:
			if (ds.rtcEditingTarget == UI_RTC_EDT_NONE) {
				ds.setUIStage(UI_STAGE_4);
			}
			else {
				switchRtcEditingDec();
			}
			break;
		case UI_STAGE_1: ds.setUIStage(UI_STAGE_0); break;
		case UI_STAGE_2: ds.setUIStage(UI_STAGE_1); break;
		case UI_STAGE_3: ds.setUIStage(UI_STAGE_2); break;
		case UI_STAGE_4: ds.setUIStage(UI_STAGE_3); break;
		}
		break;
	case IRKEY_RIGHT:
		switch (ds.uiStage) {
		case UI_STAGE_0:
			if (ds.rtcEditingTarget == UI_RTC_EDT_NONE) {
				ds.setUIStage(UI_STAGE_1);
			}
			else {
				switchRtcEditingInc();
			}
			break;
		case UI_STAGE_1: ds.setUIStage(UI_STAGE_2); break;
		case UI_STAGE_2: ds.setUIStage(UI_STAGE_3); break;
		case UI_STAGE_3: ds.setUIStage(UI_STAGE_4); break;
		case UI_STAGE_4: ds.setUIStage(UI_STAGE_0); break;
		}
		break;
	case IRKEY_UP:
		switch (ds.uiStage) {
		case UI_STAGE_0: rtcIncrease(); break;
		case UI_STAGE_1: lightControlLevelIncrease(2); break;
		case UI_STAGE_2: lightControlLevelIncrease(3); break;
		case UI_STAGE_3: lightControlLevelIncrease(4); break;
		}
		break;
	case IRKEY_DOWN:
		switch (ds.uiStage) {
		case UI_STAGE_0: rtcDecrease(); break;
		case UI_STAGE_1: lightControlLevelDecrease(2); break;
		case UI_STAGE_2: lightControlLevelDecrease(3); break;
		case UI_STAGE_3: lightControlLevelDecrease(4); break;
		}
		break;
	case IRKEY_OK:
		switch (ds.uiStage) {
		case UI_STAGE_0: enterRtcEditing(); break;
		case UI_STAGE_1: lightControlSwitchPower(2); break;
		case UI_STAGE_2: lightControlSwitchPower(3); break;
		case UI_STAGE_3: lightControlSwitchPower(4); break;
		case UI_STAGE_4: lightControlSwitchPower(5); break;
		}
		break;
	case IRKEY_A:
		ds.setUIStage(UI_STAGE_0);
		break;
	case IRKEY_B:
		ds.setUIStage(UI_STAGE_1);
		break;
	case IRKEY_C:
		ds.setUIStage(UI_STAGE_2);
		break;
	case IRKEY_D:
		ds.setUIStage(UI_STAGE_3);
		break;
	default:
		break;
	}
}

void IR::enterRtcEditing() {
	if (ds.rtcEditingTarget == UI_RTC_EDT_NONE) {
		ds.setRtcEditing(UI_RTC_EDT_MINUTE);
		ds.rtcEditingAnimationStep = 1;
	}
	else {
		switch (ds.rtcEditingTarget) {
		case UI_RTC_EDT_YEAR: ds.f.rtcYearReadyToShow = 1; break;
		case UI_RTC_EDT_MONTH: ds.f.rtcMonthReadyToShow = 1; break;
		case UI_RTC_EDT_DAY: ds.f.rtcDayReadyToShow = 1; break;
		case UI_RTC_EDT_WEEKDAY: ds.f.rtcWeekdayReadyToShow = 1; break;
		case UI_RTC_EDT_HOUR: ds.f.rtcHourReadyToShow = 1; break;
		case UI_RTC_EDT_MINUTE: ds.f.rtcMinuteReadyToShow = 1; break;
		}
		ds.setRtcEditing(UI_RTC_EDT_NONE);
		ds.rtcEditingAnimationStep = 0;
	}
}

void IR::switchRtcEditingInc() {
	switch (ds.rtcEditingTarget) {
	case UI_RTC_EDT_MINUTE:
		ds.f.rtcMinuteReadyToShow = 1;
		ds.setRtcEditing(UI_RTC_EDT_YEAR);
		break;
	case UI_RTC_EDT_HOUR:
		ds.f.rtcHourReadyToShow = 1;
		ds.setRtcEditing(UI_RTC_EDT_MINUTE);
		break;
	case UI_RTC_EDT_WEEKDAY:
		ds.f.rtcWeekdayReadyToShow = 1;
		ds.setRtcEditing(UI_RTC_EDT_HOUR);
		break;
	case UI_RTC_EDT_DAY:
		ds.f.rtcDayReadyToShow = 1;
		ds.setRtcEditing(UI_RTC_EDT_WEEKDAY);
		break;
	case UI_RTC_EDT_MONTH:
		ds.f.rtcMonthReadyToShow = 1;
		ds.setRtcEditing(UI_RTC_EDT_DAY);
		break;
	case UI_RTC_EDT_YEAR:
		ds.f.rtcYearReadyToShow = 1;
		ds.setRtcEditing(UI_RTC_EDT_MONTH);
		break;
	}
	ds.rtcEditingAnimationMillis = 0;
}

void IR::switchRtcEditingDec() {
	switch (ds.rtcEditingTarget) {
	case UI_RTC_EDT_MINUTE:
		ds.f.rtcMinuteReadyToShow = 1;
		ds.setRtcEditing(UI_RTC_EDT_HOUR);
		break;
	case UI_RTC_EDT_HOUR:
		ds.f.rtcHourReadyToShow = 1;
		ds.setRtcEditing(UI_RTC_EDT_WEEKDAY);
		break;
	case UI_RTC_EDT_WEEKDAY:
		ds.f.rtcWeekdayReadyToShow = 1;
		ds.setRtcEditing(UI_RTC_EDT_DAY);
		break;
	case UI_RTC_EDT_DAY:
		ds.f.rtcDayReadyToShow = 1;
		ds.setRtcEditing(UI_RTC_EDT_MONTH);
		break;
	case UI_RTC_EDT_MONTH:
		ds.f.rtcMonthReadyToShow = 1;
		ds.setRtcEditing(UI_RTC_EDT_YEAR);
		break;
	case UI_RTC_EDT_YEAR:
		ds.f.rtcYearReadyToShow = 1;
		ds.setRtcEditing(UI_RTC_EDT_MINUTE);
		break;
	}
	ds.rtcEditingAnimationMillis = 0;
}

void IR::rtcIncrease() {
	switch (ds.rtcEditingTarget) {
	case UI_RTC_EDT_YEAR:
		ds.rtcYearEdit = ds.rtcYear + 1;
		if (ds.rtcYearEdit > 2099) {
			ds.rtcYearEdit = 1900;
		}
		ds.f.rtcYearEditReadyToSet = 1;
		break;
	case UI_RTC_EDT_MONTH:
		ds.rtcMonthEdit = ds.rtcMonth + 1;
		if (ds.rtcMonthEdit > 12) {
			ds.rtcMonthEdit = 1;
		}
		ds.f.rtcMonthEditReadyToSet = 1;
		break;
	case UI_RTC_EDT_DAY:
		ds.rtcDayEdit = ds.rtcDay + 1;
		if (ds.rtcDayEdit > 31) {
			ds.rtcDayEdit = 1;
		}
		ds.f.rtcDayEditReadyToSet = 1;
		break;
	case UI_RTC_EDT_WEEKDAY:
		ds.rtcWeekdayEdit = ds.rtcWeekday + 1;
		if (ds.rtcWeekdayEdit > 7) {
			ds.rtcWeekdayEdit = 1;
		}
		ds.f.rtcWeekdayEditReadyToSet = 1;
		break;
	case UI_RTC_EDT_HOUR:
		ds.rtcHourEdit = ds.rtcHour + 1;
		if (ds.rtcHourEdit > 23) {
			ds.rtcHourEdit = 0;
		}
		ds.f.rtcHourEditReadyToSet = 1;
		break;
	case UI_RTC_EDT_MINUTE:
		ds.rtcMinuteEdit = ds.rtcMinute + 1;
		if (ds.rtcMinuteEdit > 59) {
			ds.rtcMinuteEdit = 0;
		}
		ds.f.rtcMinuteEditReadyToSet = 1;
		break;
	}
}

void IR::rtcDecrease() {
	switch (ds.rtcEditingTarget) {
	case UI_RTC_EDT_YEAR:
		ds.rtcYearEdit = ds.rtcYear - 1;
		if (ds.rtcYearEdit < 1900) {
			ds.rtcYearEdit = 2099;
		}
		ds.f.rtcYearEditReadyToSet = 1;
		break;
	case UI_RTC_EDT_MONTH:
		ds.rtcMonthEdit = ds.rtcMonth - 1;
		if (ds.rtcMonthEdit < 1) {
			ds.rtcMonthEdit = 12;
		}
		ds.f.rtcMonthEditReadyToSet = 1;
		break;
	case UI_RTC_EDT_DAY:
		ds.rtcDayEdit = ds.rtcDay - 1;
		if (ds.rtcDayEdit < 1) {
			ds.rtcDayEdit = 31;
		}
		ds.f.rtcDayEditReadyToSet = 1;
		break;
	case UI_RTC_EDT_WEEKDAY:
		ds.rtcWeekdayEdit = ds.rtcWeekday - 1;
		if (ds.rtcWeekdayEdit < 1) {
			ds.rtcWeekdayEdit = 7;
		}
		ds.f.rtcWeekdayEditReadyToSet = 1;
		break;
	case UI_RTC_EDT_HOUR:
		ds.rtcHourEdit = ds.rtcHour - 1;
		if (ds.rtcHourEdit > 23) {
			ds.rtcHourEdit = 23;
		}
		ds.f.rtcHourEditReadyToSet = 1;
		break;
	case UI_RTC_EDT_MINUTE:
		ds.rtcMinuteEdit = ds.rtcMinute - 1;
		if (ds.rtcMinuteEdit > 59) {
			ds.rtcMinuteEdit = 59;
		}
		ds.f.rtcMinuteEditReadyToSet = 1;
		break;
	}
}

void IR::lightControlSwitchPower(uint8_t channel) {
	ds.cd[channel].setLightControlIsOn(!ds.cd[channel].lightIsOn);
}

void IR::lightControlLevelIncrease(uint8_t channel) {
	ds.cd[channel].setLightControlLevel(ds.cd[channel].lightLevel + 1);
}

void IR::lightControlLevelDecrease(uint8_t channel) {
	ds.cd[channel].setLightControlLevel(ds.cd[channel].lightLevel - 1);
}
