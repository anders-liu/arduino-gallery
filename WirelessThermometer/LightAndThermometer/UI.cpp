#include "UI.h"
#include "Bitmaps.h"

#define SPINNER_ANIMATION_INTERVAL 100
#define UI_OFF_INTERVAL 200000

#define VALUE_Y 20
#define MAX_VALUE_LEN 6

//
// Utiliteis
//

enum Unit {
	UC = 0,  // Celsius
	UF = 1,  // Fahrenheit
	UP = 2,  // Percentage
};

static void drawEmptyThermometerIcon(const Adafruit_SSD1306 &d) {
	d.fillRect(0, 1, 16, 14, BLACK);
	d.display();
}

static void drawThermometerIcon(const Adafruit_SSD1306 &d) {
	d.fillRect(0, 1, 16, 14, BLACK);
	d.drawBitmap(0, 1, BMP_TH, 16, 14, WHITE);
	d.display();
}

static void drawThermometerErrorIcon(const Adafruit_SSD1306 &d) {
	d.fillRect(0, 1, 16, 14, BLACK);
	d.drawBitmap(0, 1, BMP_TH_ERR, 16, 14, WHITE);
	d.display();
}

static void drawEmptyAntennaIcon(const Adafruit_SSD1306 &d) {
	d.fillRect(16, 1, 16, 14, BLACK);
	d.display();
}

static void drawAntennaIcon(const Adafruit_SSD1306 &d) {
	d.fillRect(16, 1, 16, 14, BLACK);
	d.drawBitmap(16, 1, BMP_ANT, 16, 14, WHITE);
	d.display();
}

static void drawAntennaErrorIcon(const Adafruit_SSD1306 &d) {
	d.fillRect(16, 1, 16, 14, BLACK);
	d.drawBitmap(16, 1, BMP_ANT_ERR, 16, 14, WHITE);
	d.display();
}

static void clearValueArea(const Adafruit_SSD1306 &d) {
	d.fillRect(0, VALUE_Y, 128, BMP24X32_ACTUAL_H + 1, BLACK);
}

static uint8_t measureTextWidth24x32(const char *str) {
	uint8_t w = 0;
	uint8_t i;
	for (i = 0; i < MAX_VALUE_LEN && str[i] > 0; i++) {
		switch (str[i]) {
		case '0': w += BMP24X32_0_ACTUAL_W; break;
		case '1': w += BMP24X32_1_ACTUAL_W; break;
		case '2': w += BMP24X32_2_ACTUAL_W; break;
		case '3': w += BMP24X32_3_ACTUAL_W; break;
		case '4': w += BMP24X32_4_ACTUAL_W; break;
		case '5': w += BMP24X32_5_ACTUAL_W; break;
		case '6': w += BMP24X32_6_ACTUAL_W; break;
		case '7': w += BMP24X32_7_ACTUAL_W; break;
		case '8': w += BMP24X32_8_ACTUAL_W; break;
		case '9': w += BMP24X32_9_ACTUAL_W; break;
		case '-': w += BMP24X32_NEG_ACTUAL_W; break;
		case 'C': w += BMP24X32_C_ACTUAL_W; break;
		case 'F': w += BMP24X32_F_ACTUAL_W; break;
		case '%': w += BMP24X32_PERCENT_ACTUAL_W; break;
		case '`': w += BMP24X32_DEG_ACTUAL_W; break;
		case ' ': w += BMP24X32_WHITESPACE_ACTUAL_W; break;
		}
	}
	if (i > 0) {
		w += (i - 1) * BMP24X32_SPACING;
	}
	return w;
}

static uint8_t measureTextWidth16(const char *str) {
	uint8_t w = 0;
	uint8_t i;
	for (i = 0; i < 10 && str[i] > 0; i++) {
		switch (str[i]) {
		case '0': w += BMP8X16_0_ACTUAL_W; break;
		case '1': w += BMP8X16_1_ACTUAL_W; break;
		case '2': w += BMP8X16_2_ACTUAL_W; break;
		case '3': w += BMP8X16_3_ACTUAL_W; break;
		case '4': w += BMP8X16_4_ACTUAL_W; break;
		case '5': w += BMP8X16_5_ACTUAL_W; break;
		case '6': w += BMP8X16_6_ACTUAL_W; break;
		case '7': w += BMP8X16_7_ACTUAL_W; break;
		case '8': w += BMP8X16_8_ACTUAL_W; break;
		case '9': w += BMP8X16_9_ACTUAL_W; break;
		case ':': w += BMP8X16_COLON_ACTUAL_W; break;
		case '-': w += BMP8X16_DASH_ACTUAL_W; break;

		case 'A': w += BMP16X16_A_ACTUAL_W; break;
		case 'D': w += BMP16X16_D_ACTUAL_W; break;
		case 'E': w += BMP16X16_E_ACTUAL_W; break;
		case 'F': w += BMP16X16_F_ACTUAL_W; break;
		case 'H': w += BMP16X16_H_ACTUAL_W; break;
		case 'I': w += BMP16X16_I_ACTUAL_W; break;
		case 'M': w += BMP16X16_M_ACTUAL_W; break;
		case 'N': w += BMP16X16_N_ACTUAL_W; break;
		case 'O': w += BMP16X16_O_ACTUAL_W; break;
		case 'R': w += BMP16X16_R_ACTUAL_W; break;
		case 'S': w += BMP16X16_S_ACTUAL_W; break;
		case 'T': w += BMP16X16_T_ACTUAL_W; break;
		case 'U': w += BMP16X16_U_ACTUAL_W; break;
		case 'W': w += BMP16X16_W_ACTUAL_W; break;
		}
	}
	if (i > 0) {
		w += (i - 1) * BMP16X16_SPACING;
	}
	return w;
}

static void drawText16(const Adafruit_SSD1306 &d, uint8_t x, uint8_t y, const char *str) {
	for (uint8_t i = 0; i < 10 && str[i]>0; i++) {
		switch (str[i]) {
		case '0':
			d.drawBitmap(x, y, BMP8X16_0, 8, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP8X16_0_ACTUAL_W;
			break;
		case '1':
			d.drawBitmap(x, y, BMP8X16_1, 8, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP8X16_1_ACTUAL_W;
			break;
		case '2':
			d.drawBitmap(x, y, BMP8X16_2, 8, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP8X16_2_ACTUAL_W;
			break;
		case '3':
			d.drawBitmap(x, y, BMP8X16_3, 8, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP8X16_3_ACTUAL_W;
			break;
		case '4':
			d.drawBitmap(x, y, BMP8X16_4, 8, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP8X16_4_ACTUAL_W;
			break;
		case '5':
			d.drawBitmap(x, y, BMP8X16_5, 8, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP8X16_5_ACTUAL_W;
			break;
		case '6':
			d.drawBitmap(x, y, BMP8X16_6, 8, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP8X16_6_ACTUAL_W;
			break;
		case '7':
			d.drawBitmap(x, y, BMP8X16_7, 8, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP8X16_7_ACTUAL_W;
			break;
		case '8':
			d.drawBitmap(x, y, BMP8X16_8, 8, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP8X16_8_ACTUAL_W;
			break;
		case '9':
			d.drawBitmap(x, y, BMP8X16_9, 8, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP8X16_9_ACTUAL_W;
			break;
		case ':':
			d.drawPixel(x + 1, y + 1, WHITE);
			d.drawPixel(x + 1, y + 9, WHITE);
			x += BMP16X16_SPACING + BMP8X16_COLON_ACTUAL_W;
			break;
		case '-':
			d.drawFastHLine(x + 1, y + 6, BMP8X16_DASH_ACTUAL_W - 2, WHITE);
			x += BMP16X16_SPACING + BMP8X16_DASH_ACTUAL_W;
			break;

		case 'A':
			d.drawBitmap(x, y, BMP16X16_A, 16, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP16X16_A_ACTUAL_W;
			break;
		case 'D':
			d.drawBitmap(x, y, BMP16X16_D, 16, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP16X16_D_ACTUAL_W;
			break;
		case 'E':
			d.drawBitmap(x, y, BMP16X16_E, 16, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP16X16_E_ACTUAL_W;
			break;
		case 'F':
			d.drawBitmap(x, y, BMP16X16_F, 16, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP16X16_F_ACTUAL_W;
			break;
		case 'H':
			d.drawBitmap(x, y, BMP16X16_H, 16, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP16X16_H_ACTUAL_W;
			break;
		case 'I':
			d.drawFastVLine(x + BMP16X16_I_ACTUAL_W / 2, y, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP16X16_I_ACTUAL_W;
			break;
		case 'M':
			d.drawBitmap(x, y, BMP16X16_M, 16, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP16X16_M_ACTUAL_W;
			break;
		case 'N':
			d.drawBitmap(x, y, BMP16X16_N, 16, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP16X16_N_ACTUAL_W;
			break;
		case 'O':
			d.drawBitmap(x, y, BMP16X16_O, 16, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP16X16_O_ACTUAL_W;
			break;
		case 'R':
			d.drawBitmap(x, y, BMP16X16_R, 16, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP16X16_R_ACTUAL_W;
			break;
		case 'S':
			d.drawBitmap(x, y, BMP16X16_S, 16, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP16X16_S_ACTUAL_W;
			break;
		case 'T':
			d.drawBitmap(x, y, BMP16X16_T, 16, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP16X16_T_ACTUAL_W;
			break;
		case 'U':
			d.drawBitmap(x, y, BMP16X16_U, 16, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP16X16_U_ACTUAL_W;
			break;
		case 'W':
			d.drawBitmap(x, y, BMP16X16_W, 16, BMP16X16_ACTUAL_H, WHITE);
			x += BMP16X16_SPACING + BMP16X16_W_ACTUAL_W;
			break;
		}
	}
	d.display();
}

static void drawValue(const Adafruit_SSD1306 &d, const char *str) {
	uint8_t x = (128 - measureTextWidth24x32(str)) / 2;
	for (uint8_t i = 0; i < MAX_VALUE_LEN && str[i] > 0; i++) {
		switch (str[i]) {
		case '0':
			d.drawBitmap(x, VALUE_Y + 1, BMP24X32_0, 24, BMP24X32_ACTUAL_H, WHITE);
			x += BMP24X32_SPACING + BMP24X32_0_ACTUAL_W;
			break;
		case '1':
			d.drawBitmap(x, VALUE_Y + 1, BMP24X32_1, 24, BMP24X32_ACTUAL_H, WHITE);
			x += BMP24X32_SPACING + BMP24X32_1_ACTUAL_W;
			break;
		case '2':
			d.drawBitmap(x, VALUE_Y + 1, BMP24X32_2, 24, BMP24X32_ACTUAL_H, WHITE);
			x += BMP24X32_SPACING + BMP24X32_2_ACTUAL_W;
			break;
		case '3':
			d.drawBitmap(x, VALUE_Y + 1, BMP24X32_3, 24, BMP24X32_ACTUAL_H, WHITE);
			x += BMP24X32_SPACING + BMP24X32_3_ACTUAL_W;
			break;
		case '4':
			d.drawBitmap(x, VALUE_Y + 1, BMP24X32_4, 24, BMP24X32_ACTUAL_H, WHITE);
			x += BMP24X32_SPACING + BMP24X32_4_ACTUAL_W;
			break;
		case '5':
			d.drawBitmap(x, VALUE_Y + 1, BMP24X32_5, 24, BMP24X32_ACTUAL_H, WHITE);
			x += BMP24X32_SPACING + BMP24X32_5_ACTUAL_W;
			break;
		case '6':
			d.drawBitmap(x, VALUE_Y + 1, BMP24X32_6, 24, BMP24X32_ACTUAL_H, WHITE);
			x += BMP24X32_SPACING + BMP24X32_6_ACTUAL_W;
			break;
		case '7':
			d.drawBitmap(x, VALUE_Y + 1, BMP24X32_7, 24, BMP24X32_ACTUAL_H, WHITE);
			x += BMP24X32_SPACING + BMP24X32_7_ACTUAL_W;
			break;
		case '8':
			d.drawBitmap(x, VALUE_Y + 1, BMP24X32_8, 24, BMP24X32_ACTUAL_H, WHITE);
			x += BMP24X32_SPACING + BMP24X32_8_ACTUAL_W;
			break;
		case '9':
			d.drawBitmap(x, VALUE_Y + 1, BMP24X32_9, 24, BMP24X32_ACTUAL_H, WHITE);
			x += BMP24X32_SPACING + BMP24X32_9_ACTUAL_W;
			break;
		case '-':
			d.fillRect(x, VALUE_Y + 10, BMP24X32_NEG_ACTUAL_W, BMP24X32_NEG_ACTUAL_H, WHITE);
			x += BMP24X32_SPACING + BMP24X32_NEG_ACTUAL_W;
			break;
		case 'C':
			d.drawBitmap(x, VALUE_Y + 1, BMP24X32_C, 24, BMP24X32_ACTUAL_H, WHITE);
			x += BMP24X32_SPACING + BMP24X32_C_ACTUAL_W;
			break;
		case 'F':
			d.drawBitmap(x, VALUE_Y + 1, BMP24X32_F, 24, BMP24X32_ACTUAL_H, WHITE);
			x += BMP24X32_SPACING + BMP24X32_F_ACTUAL_W;
			break;
		case '%':
			d.drawBitmap(x, VALUE_Y + 1, BMP24X32_PERCENT, 24, BMP24X32_ACTUAL_H, WHITE);
			x += BMP24X32_SPACING + BMP24X32_PERCENT_ACTUAL_W;
			break;
		case '`':
			d.drawBitmap(x, VALUE_Y, BMP24X32_DEG, 16, BMP24X32_DEG_ACTUAL_H, WHITE);
			x += BMP24X32_SPACING + BMP24X32_DEG_ACTUAL_W;
			break;
		case ' ':
			x += BMP24X32_SPACING + BMP24X32_WHITESPACE_ACTUAL_W;
			break;
		}
	}
	d.display();
}

static void displayNoValue(const Adafruit_SSD1306 &d, Unit u) {
	clearValueArea(d);
	switch (u) {
	case UC: drawValue(d, "-- `C"); break;
	case UF: drawValue(d, "-- `F"); break;
	case UP: drawValue(d, "-- %"); break;
	}
}

static void displayValue(const Adafruit_SSD1306 &d, float vf, Unit u) {
	clearValueArea(d);
	char str[10] = { 0 };
	int n = snprintf(str, 10, "%d ", (int16_t)vf);
	switch (u) {
	case UC: str[n] = '`'; str[n + 1] = 'C'; break;
	case UF: str[n] = '`'; str[n + 1] = 'F'; break;
	case UP: str[n] = '%'; break;
	}
	drawValue(d, str);
}

static void clearDateArea(const Adafruit_SSD1306 &d) {
	d.fillRect(1, 52, 89, BMP16X16_ACTUAL_H, BLACK);
}

static void clearWeekdayArea(const Adafruit_SSD1306 &d) {
	d.fillRect(85, 52, 42, BMP16X16_ACTUAL_H, BLACK);
}

static void clearTimeArea(const Adafruit_SSD1306 &d) {
	d.fillRect(80, 2, 47, BMP16X16_ACTUAL_H, BLACK);
}

static void clearRtcAreas(const Adafruit_SSD1306 &d) {
	clearDateArea(d);
	clearWeekdayArea(d);
	clearTimeArea(d);
}

static void drawDate(const Adafruit_SSD1306 &d) {
	char str[11] = { 0 };
	snprintf(str, 11, "%d-%d-%d", ds.rtcYear, ds.rtcMonth, ds.rtcDay);
	drawText16(d, 1, 52, str);
}

static void drawWeekday(const Adafruit_SSD1306 &d) {
	const char *str = nullptr;

	switch (ds.rtcWeekday) {
	case 1: str = "MON"; break;
	case 2: str = "TUE"; break;
	case 3: str = "WED"; break;
	case 4: str = "THU"; break;
	case 5: str = "FRI"; break;
	case 6: str = "SAT"; break;
	case 7: str = "SUN"; break;
	}

	if (str) {
		uint8_t w = measureTextWidth16(str);
		drawText16(d, 127 - w, 52, str);
	}
}

static void drawTime(const Adafruit_SSD1306 &d) {
	char str[6] = { 0 };
	snprintf(str, 6, "%d:%02d", ds.rtcHour, ds.rtcMinute);
	uint8_t w = measureTextWidth16(str);
	drawText16(d, 127 - w, 2, str);
}

static void displayRtc(const Adafruit_SSD1306 &d) {
	drawDate(d);
	drawWeekday(d);
	drawTime(d);
	d.display();
}

//
// Implementations
//

void UI::setup() {
	display.begin(SSD1306_SWITCHCAPVCC, addr);
	display.clearDisplay();
	display.display();
}

void UI::loop() {
	loopSpinner();
	loopAutoOff();
	loopStatus();
	loopRtc();
}

void UI::startSpinner() {
	if (spinnerStep == 0) {
		spinnerStep = 1;
	}
}

void UI::displayCelsius() {
	if (ds.sensorState != S_S_OK || !IS_VALID_TEMPERATURE(ds.temperature)) {
		displayNoValue(display, UC);
	}
	else {
		displayValue(display, ds.temperature, UC);
	}
}

void UI::displayFahrenheit() {
	if (ds.sensorState != S_S_OK || !IS_VALID_TEMPERATURE(ds.temperature)) {
		displayNoValue(display, UF);
	}
	else {
		float tf = ds.temperature * 9 / 5 + 32;
		displayValue(display, tf, UF);
	}
}

void UI::displayHumidity() {
	if (ds.sensorState != S_S_OK || !IS_VALID_HUMIDITY(ds.humidity)) {
		displayNoValue(display, UP);
	}
	else {
		displayValue(display, ds.humidity, UP);
	}
}

void UI::switchUI() {
	switch (uiState) {
	case UI_D_OFF:
		ds.f.sensorStateReadyToShow = 1;
		ds.f.wirelessStateReadyToShow = 1;
		display.drawLine(0, 16, 127, 16, WHITE);
		display.drawLine(0, 49, 127, 49, WHITE);
		displayCelsius();

		ds.f.rtcDateReadyToShow = 1;
		ds.f.rtcWeekdayReadyToShow = 1;
		ds.f.rtcTimeReadyToShow = 1;

		uiState = UI_D_TC;
		break;

	case UI_D_TC:
		displayFahrenheit();
		uiState = UI_D_TF;
		break;

	case UI_D_TF:
		displayHumidity();
		uiState = UI_D_H;
		break;

	case UI_D_H:
		display.clearDisplay();
		display.display();
		uiState = UI_D_OFF;
		break;
	}

	lastActiveMillis = millis();
}

void UI::loopSpinner() {
	if (ds.f.updateAnimReadyToShow) {
		ds.f.updateAnimReadyToShow = 0;
		spinnerStep = 1;
	}

	if (millis() - lastSpinnerMillis > SPINNER_ANIMATION_INTERVAL) {
		if (uiState != UI_D_OFF) {
			switch (spinnerStep) {
			case 1: display.drawRect(36, 7, 2, 2, WHITE); display.display(); spinnerStep = 2; break;
			case 2: display.drawRect(39, 7, 2, 2, WHITE); display.display(); spinnerStep = 3; break;
			case 3: display.drawRect(42, 7, 2, 2, WHITE); display.display(); spinnerStep = 4; break;
			case 4: display.drawRect(45, 7, 2, 2, WHITE); display.display(); spinnerStep = 5; break;
			case 5: display.drawRect(48, 7, 2, 2, WHITE); display.display(); spinnerStep = 6; break;

			case 6: display.drawRect(36, 7, 2, 2, BLACK); display.display(); spinnerStep = 7; break;
			case 7: display.drawRect(39, 7, 2, 2, BLACK); display.display(); spinnerStep = 8; break;
			case 8: display.drawRect(42, 7, 2, 2, BLACK); display.display(); spinnerStep = 9; break;
			case 9: display.drawRect(45, 7, 2, 2, BLACK); display.display(); spinnerStep = 10; break;
			case 10: display.drawRect(48, 7, 2, 2, BLACK); display.display(); spinnerStep = 0; break;
			}
		}

		lastSpinnerMillis = millis();
	}
}

void UI::loopAutoOff() {
	if (millis() - lastActiveMillis > UI_OFF_INTERVAL) {
		if (uiState != UI_D_OFF) {
			display.clearDisplay();
			display.display();
			uiState = UI_D_OFF;
		}
	}
}

void UI::loopStatus() {
	if (uiState != UI_D_OFF) {
		if (ds.f.wirelessStateReadyToShow) {
			ds.f.wirelessStateReadyToShow = 0;

			switch (ds.wirelessState)
			{
			case S_WL_OK: drawAntennaIcon(display); break;
			case S_WL_NOSIGNAL: drawAntennaErrorIcon(display); break;
			default: drawEmptyAntennaIcon(display); break;
			}
		}

		if (ds.f.sensorStateReadyToShow) {
			ds.f.sensorStateReadyToShow = 0;

			switch (ds.sensorState) {
			case S_S_OK: drawThermometerIcon(display); break;
			case S_S_ERROR: drawThermometerErrorIcon(display); break;
			default: drawEmptyThermometerIcon(display); break;
			}
		}
	}
}

void UI::loopRtc() {
	if (uiState != UI_D_OFF) {
		if (ds.f.rtcStatusReadyToShow) {
			ds.f.rtcStatusReadyToShow = 0;
			clearRtcAreas(display);
			display.display();

			if (ds.rtcStatus == S_RTC_OK) {
				displayRtc(display);
			}
		}
		else if (ds.rtcStatus == S_RTC_OK) {
			if (ds.f.rtcDateReadyToShow) {
				ds.f.rtcDateReadyToShow = 0;
				clearDateArea(display);
				drawDate(display);
			}

			if (ds.f.rtcWeekdayReadyToShow) {
				ds.f.rtcWeekdayReadyToShow = 0;
				clearWeekdayArea(display);
				drawWeekday(display);
			}

			if (ds.f.rtcTimeReadyToShow) {
				ds.f.rtcTimeReadyToShow = 0;
				clearTimeArea(display);
				drawTime(display);
			}
			display.display();
		}
	}
}
