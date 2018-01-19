#pragma once

#include <Arduino.h>

extern const unsigned char PROGMEM BMP_ANT[];
extern const unsigned char PROGMEM BMP_ANT_ERR[];
extern const unsigned char PROGMEM BMP_TH[];
extern const unsigned char PROGMEM BMP_TH_ERR[];

extern const unsigned char PROGMEM BMP8X16_0[];
extern const unsigned char PROGMEM BMP8X16_1[];
extern const unsigned char PROGMEM BMP8X16_2[];
extern const unsigned char PROGMEM BMP8X16_3[];
extern const unsigned char PROGMEM BMP8X16_4[];
extern const unsigned char PROGMEM BMP8X16_5[];
extern const unsigned char PROGMEM BMP8X16_6[];
extern const unsigned char PROGMEM BMP8X16_7[];
extern const unsigned char PROGMEM BMP8X16_8[];
extern const unsigned char PROGMEM BMP8X16_9[];

extern const unsigned char PROGMEM BMP16X16_A[];
extern const unsigned char PROGMEM BMP16X16_D[];
extern const unsigned char PROGMEM BMP16X16_E[];
extern const unsigned char PROGMEM BMP16X16_F[];
extern const unsigned char PROGMEM BMP16X16_H[];
extern const unsigned char PROGMEM BMP16X16_M[];
extern const unsigned char PROGMEM BMP16X16_N[];
extern const unsigned char PROGMEM BMP16X16_O[];
extern const unsigned char PROGMEM BMP16X16_R[];
extern const unsigned char PROGMEM BMP16X16_S[];
extern const unsigned char PROGMEM BMP16X16_T[];
extern const unsigned char PROGMEM BMP16X16_U[];
extern const unsigned char PROGMEM BMP16X16_W[];

extern const unsigned char PROGMEM BMP24X32_0[];
extern const unsigned char PROGMEM BMP24X32_1[];
extern const unsigned char PROGMEM BMP24X32_2[];
extern const unsigned char PROGMEM BMP24X32_3[];
extern const unsigned char PROGMEM BMP24X32_4[];
extern const unsigned char PROGMEM BMP24X32_5[];
extern const unsigned char PROGMEM BMP24X32_6[];
extern const unsigned char PROGMEM BMP24X32_7[];
extern const unsigned char PROGMEM BMP24X32_8[];
extern const unsigned char PROGMEM BMP24X32_9[];

extern const unsigned char PROGMEM BMP24X32_C[];
extern const unsigned char PROGMEM BMP24X32_F[];
extern const unsigned char PROGMEM BMP24X32_PERCENT[];

extern const unsigned char PROGMEM BMP24X32_DEG[];

#define BMP8X16_0_ACTUAL_W           ((uint8_t)7)
#define BMP8X16_1_ACTUAL_W           ((uint8_t)4)
#define BMP8X16_2_ACTUAL_W           ((uint8_t)7)
#define BMP8X16_3_ACTUAL_W           ((uint8_t)7)
#define BMP8X16_4_ACTUAL_W           ((uint8_t)8)
#define BMP8X16_5_ACTUAL_W           ((uint8_t)7)
#define BMP8X16_6_ACTUAL_W           ((uint8_t)7)
#define BMP8X16_7_ACTUAL_W           ((uint8_t)7)
#define BMP8X16_8_ACTUAL_W           ((uint8_t)7)
#define BMP8X16_9_ACTUAL_W           ((uint8_t)7)
#define BMP8X16_COLON_ACTUAL_W       ((uint8_t)3)
#define BMP8X16_DASH_ACTUAL_W        ((uint8_t)7)

#define BMP16X16_A_ACTUAL_W          ((uint8_t)11)
#define BMP16X16_D_ACTUAL_W          ((uint8_t)10)
#define BMP16X16_E_ACTUAL_W          ((uint8_t)9)
#define BMP16X16_F_ACTUAL_W          ((uint8_t)8)
#define BMP16X16_H_ACTUAL_W          ((uint8_t)9)
#define BMP16X16_I_ACTUAL_W          ((uint8_t)3)
#define BMP16X16_M_ACTUAL_W          ((uint8_t)11)
#define BMP16X16_N_ACTUAL_W          ((uint8_t)9)
#define BMP16X16_O_ACTUAL_W          ((uint8_t)10)
#define BMP16X16_R_ACTUAL_W          ((uint8_t)9)
#define BMP16X16_S_ACTUAL_W          ((uint8_t)9)
#define BMP16X16_T_ACTUAL_W          ((uint8_t)9)
#define BMP16X16_U_ACTUAL_W          ((uint8_t)9)
#define BMP16X16_W_ACTUAL_W          ((uint8_t)13)

#define BMP16X16_ACTUAL_H            ((uint8_t)12)
#define BMP16X16_SPACING             ((uint8_t)1)

#define BMP24X32_0_ACTUAL_W          ((uint8_t)17)
#define BMP24X32_1_ACTUAL_W          ((uint8_t)16)
#define BMP24X32_2_ACTUAL_W          ((uint8_t)17)
#define BMP24X32_3_ACTUAL_W          ((uint8_t)15)
#define BMP24X32_4_ACTUAL_W          ((uint8_t)18)
#define BMP24X32_5_ACTUAL_W          ((uint8_t)16)
#define BMP24X32_6_ACTUAL_W          ((uint8_t)15)
#define BMP24X32_7_ACTUAL_W          ((uint8_t)15)
#define BMP24X32_8_ACTUAL_W          ((uint8_t)17)
#define BMP24X32_9_ACTUAL_W          ((uint8_t)15)

#define BMP24X32_C_ACTUAL_W          ((uint8_t)17)
#define BMP24X32_F_ACTUAL_W          ((uint8_t)15)
#define BMP24X32_PERCENT_ACTUAL_W    ((uint8_t)19)

#define BMP24X32_ACTUAL_H            ((uint8_t)24)
#define BMP24X32_SPACING             ((uint8_t)2)

#define BMP24X32_DEG_ACTUAL_W        ((uint8_t)16)
#define BMP24X32_DEG_ACTUAL_H        ((uint8_t)10)

#define BMP24X32_NEG_ACTUAL_W        ((uint8_t)10)
#define BMP24X32_NEG_ACTUAL_H        ((uint8_t)5)

#define BMP24X32_WHITESPACE_ACTUAL_W ((uint8_t)4)
