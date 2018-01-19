// 
// UI_Demo.cpp
//
// Run a series of UI demo.
// 

#include "DataStore.h"

static uint32_t long UI_Demo_LastMillis = 0;
static uint8_t UI_Demo_Step = 0;
static uint8_t UI_Demo_Delay_Count = 0;

static void UI_Demo_Setup() {
	ds.setRtcData(2018, 1, 2, 3, 12, 34, 56);
}

static void UI_Demo_Loop() {
	if (millis() - UI_Demo_LastMillis > 2000) {
		switch (UI_Demo_Step) {
		case 0:
			ds.setUIStage(UI_STAGE_0);
			ds.cd[0].setThermometerData(80, 100);
			UI_Demo_Step = 1;
			break;

		case 1:
			ds.cd[0].setStatus(CS_SENSOR_ERROR);
			UI_Demo_Step = 2;
			break;

		case 2:
			ds.cd[0].setStatus(CS_OK);
			ds.cd[0].setThermometerData(-40, 0);
			UI_Demo_Step = 3;
			break;

		case 3:
			ds.cd[1].setStatus(CS_SENSOR_ERROR);
			UI_Demo_Step = 4;
			break;

		case 4:
			ds.cd[1].setStatus(CS_NO_SIGNAL);
			UI_Demo_Step = 5;
			break;

		case 5:
			ds.cd[1].setStatus(CS_OK);
			ds.cd[1].setThermometerData(1, 1);
			UI_Demo_Step = 6;
			break;

		case 6:
			ds.setUIStage(UI_STAGE_1);
			ds.cd[2].setThermometerData(20, 20);
			UI_Demo_Step = 7;
			break;

		case 7:
			ds.cd[2].setThermometerData(INVALID_TEMPERATURE, 20);
			UI_Demo_Step = 8;
			break;

		case 8:
			ds.cd[2].setThermometerData(20, INVALID_HUMIDITY);
			UI_Demo_Step = 9;
			break;

		case 9:
			ds.setUIStage(UI_STAGE_2);
			UI_Demo_Step = 10;
			break;

		case 10:
			ds.cd[3].setStatus(CS_OK);
			ds.cd[3].setThermometerData(30, 30);
			UI_Demo_Step = 11;
			break;

		case 11:
			ds.cd[3].setStatus(CS_NO_SIGNAL);
			UI_Demo_Step = 0;
			break;
		}

		UI_Demo_LastMillis = millis();
	}
}

