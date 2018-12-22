// DataStore.h

#ifndef DATASTORE_H__
#define DATASTORE_H__

#include <Arduino.h>

enum UiStage_t : uint8_t
{
	UI_STAGE_RUNNING,
	UI_STAGE_CALIBRATE_OFF_VALUE,
	UI_STAGE_CALIBRATE_ON_VALUE,
};

class DataStore {
public:
	void setup();
	void loop();

public:
	UiStage_t getUiStage() { return uiStage; }
	void setUiStage(UiStage_t v) { uiStage = v; f.uiStageChanged = 1; }
	bool getUiStageChanged() { return f.uiStageChanged; }
	void clearUiStageChanged() { f.uiStageChanged = 0; }

	uint8_t getCalibrationOffValue() { return calibrateOffValue; }
	void setCalibrationOffValue(uint8_t v) { calibrateOffValue = v; f.calibrateOffValueReadyToShow = 1; }
	bool getCalibrationOffValueReadyToShow() { return f.calibrateOffValueReadyToShow; }
	void clearCalibrationOffValueReadyToShow() { f.calibrateOffValueReadyToShow = 0; }

	uint8_t getCalibrationOnValue() { return calibrateOnValue; }
	void setCalibrationOnValue(uint8_t v) { calibrateOnValue = v; f.calibrateOnValueReadyToShow = 1; }
	bool getCalibrationOnValueReadyToShow() { return f.calibrateOnValueReadyToShow; }
	void clearCalibrationOnValueReadyToShow() { f.calibrateOnValueReadyToShow = 0; }

	uint8_t getServoOffValue() { return servoOffValue; }
	void setServoOffValue(uint8_t v) { servoOffValue = v; f.servoOffValueReadyToSave = 1; }
	bool getServoOffValueReadyToSave() { return f.servoOffValueReadyToSave; }
	void clearServoOffValueReadyToSave() { f.servoOffValueReadyToSave = 0; }

	uint8_t getServoOnValue() { return servoOnValue; }
	void setServoOnValue(uint8_t v) { servoOnValue = v; f.servoOnValueReadyToSave = 1; }
	bool getServoOnValueReadyToSave() { return f.servoOnValueReadyToSave; }
	void clearServoOnValueReadyToSave() { f.servoOnValueReadyToSave = 0; }

	uint8_t getCurrentServoValue() { return currentServoValue; }
	void setCurrentServoValue(uint8_t v) {
		currentServoValue = v;
		f.currentServoValueReadyToSend = 1;
		f.currentServoValueReadyToSave = 1;
	}
	bool getCurrentServoValueReadyToSend() { return f.currentServoValueReadyToSend; }
	void clearCurrentServoValueReadyToSend() { f.currentServoValueReadyToSend = 0; }
	bool getCurrentServoValueReadyToSave() { return f.currentServoValueReadyToSave; }
	void clearCurrentServoValueReadyToSave() { f.currentServoValueReadyToSave = 0; }

	bool getIsPowerOn() { return isPowerOn; }
	void setIsPowerOn(bool v) {
		isPowerOn = v;
		f.isPowerOnReadyToShow = 1;
		f.isPowerOnReadyToSave = 1;
	}
	bool getIsPowerOnReadyToShow() { return f.isPowerOnReadyToShow; }
	void clearIsPowerOnReadyToShow() { f.isPowerOnReadyToShow = 0; }
	bool getIsPowerOnReadyToSave() { return f.isPowerOnReadyToSave; }
	void clearIsPowerOnReadyToSave() { f.isPowerOnReadyToSave = 0; }

private:
	UiStage_t uiStage = UI_STAGE_RUNNING;

	uint8_t calibrateOffValue = 0;
	uint8_t calibrateOnValue = 0;

	uint8_t servoOffValue = 0;
	uint8_t servoOnValue = 0;

	uint8_t currentServoValue = 0;

	bool isPowerOn = false;

	struct {
		uint8_t uiStageChanged : 1;
		uint8_t calibrateOffValueReadyToShow : 1;
		uint8_t calibrateOnValueReadyToShow : 1;
		uint8_t servoOffValueReadyToSave : 1;
		uint8_t servoOnValueReadyToSave : 1;
		uint8_t currentServoValueReadyToSend : 1;
		uint8_t currentServoValueReadyToSave : 1;
		uint8_t isPowerOnReadyToShow : 1;
		uint8_t isPowerOnReadyToSave : 1;
	} f = { 0 };
};

extern DataStore ds;

#endif  // DATASTORE_H__

