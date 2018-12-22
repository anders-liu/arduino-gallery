#include <EEPROM.h>

#include "DataStore.h"

DataStore ds;

#define DS_ADDR_SERVO_OFF 0
#define DS_ADDR_SERVO_ON  1
#define DS_ADDR_SERVO_CUR 2
#define DS_ADDR_POWER_ON  3

void DataStore::setup() {
	this->setUiStage(UI_STAGE_RUNNING);

	uint8_t value;

	value = EEPROM.read(DS_ADDR_SERVO_OFF);
	this->setServoOffValue(value);
	this->setCalibrationOffValue(value);

	value = EEPROM.read(DS_ADDR_SERVO_ON);
	this->setServoOnValue(value);
	this->setCalibrationOnValue(value);

	value = EEPROM.read(DS_ADDR_SERVO_CUR);
	this->setCurrentServoValue(value);

	value = EEPROM.read(DS_ADDR_POWER_ON);
	this->setIsPowerOn(value);
}

void DataStore::loop() {
	if (ds.getServoOffValueReadyToSave()) {
		EEPROM.update(DS_ADDR_SERVO_OFF, ds.getServoOffValue());
		ds.clearServoOffValueReadyToSave();
	}
	else if (ds.getServoOnValueReadyToSave()) {
		EEPROM.update(DS_ADDR_SERVO_ON, ds.getServoOnValue());
		ds.clearServoOnValueReadyToSave();
	}
	else if (ds.getCurrentServoValueReadyToSave()) {
		EEPROM.update(DS_ADDR_SERVO_CUR, ds.getCurrentServoValue());
		ds.clearCurrentServoValueReadyToSave();
	}
	else if (ds.getIsPowerOnReadyToSave()) {
		EEPROM.update(DS_ADDR_POWER_ON, ds.getIsPowerOn());
		ds.clearIsPowerOnReadyToSave();
	}
}
