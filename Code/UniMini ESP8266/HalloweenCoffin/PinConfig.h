#pragma once
#include "Arduino.h"


#define PIN_M1 0	// Used for bootloader and Incremental-Switch, too. Is pulled up externally.
#define PIN_M2 2	// This is TXD ofer Serial1
#define PIN_M3 4 // Used for I2C
#define PIN_M4 5 // Used for I2C
#define PIN_M5 12
#define PIN_M6 13 // Re-used for PWM bin
#define PIN_MOT_A	14
#define PIN_MOT_B	15
#define PIN_SPARE	16
#define PIN_PWM PIN_M6

// Overdischarge protection
#define PIN_CHECK_BATT	A0


#define PIN_LED_EYES			PIN_SPARE
#define PIN_LED_WHITE_A			PIN_MOT_A // Ensure other motor pin is held low
#define PIN_LED_WHITE_B			PIN_MOT_B
#define PIN_TONE				PIN_PWM 

// HC-SR04 ultrasonic sensor
#define PIN_ULTRASONIC_TRIGGER	PIN_M5 // (Yellow wire)
#define PIN_ULTRASONIC_ECHO		PIN_M4 // (Green wire)

#define PIN_SERVO_COVER			PIN_M3
#define PIN_SERVO_CORPSE		PIN_M1

#define DEBUG_PRINTLN(a) Serial.println(a)
#define DEBUG_PRINT(a) Serial.print(a)

