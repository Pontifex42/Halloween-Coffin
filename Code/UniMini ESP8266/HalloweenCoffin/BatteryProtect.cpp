#include "Arduino.h"
#include "BatteryProtect.h"
#include "PinConfig.h"
#include "CoffinServos.h"
#include "LEDs.h"

//#define DEBUG_BATTERYPROTECT

#ifndef DEBUG_BATTERYPROTECT
#undef DEBUG_PRINTLN
#undef DEBUG_PRINT
#define DEBUG_PRINTLN(a)
#define DEBUG_PRINT(a)
#endif


// measured values with actual reistors result into 190 digits per Volt
// what I measured:
// 792 at 4.2V
// 698 at 3.7V
// 619  at 3.3V
#define DIGITS_PER_VOLT 188.0

// on another chip, I measured:
// 823 at 4.2V
// 726 at 3.7V
// 649  at 3.3V
// which was 194 digits per volt

// on another chip, I measured:
// 796 at 4.2V
// 708 at 3.7V
// 626  at 3.3V


#define BATTERY_CHECK_INTERVALL 500000  // 2 times a second will do nicely
#define THRESHOLD_VALUE (3.3*DIGITS_PER_VOLT)
#define HYSTERESIS_VALUE (3.5*DIGITS_PER_VOLT)

bool BatteryAlarmState = false;
int batteryLowCounter = 0;
uint16_t measuredBattery = THRESHOLD_VALUE;

unsigned long nextTimeToCheckBattery;

bool GetBatteryAlarmState()
{
	return BatteryAlarmState;
}

void SetBatteryAlarmState(bool state)
{
	BatteryAlarmState = state;
}

void LoopBatteryProtect()
{
	unsigned long now = micros();
	if (now < nextTimeToCheckBattery)
		return;
		
	nextTimeToCheckBattery = now + BATTERY_CHECK_INTERVALL;

	measuredBattery = analogRead(PIN_CHECK_BATT);

	DEBUG_PRINT(measuredBattery);
	DEBUG_PRINT("   ");
	if (GetBatteryAlarmState())
	{	// needs to be a certain time above a higher voltage to return to non-alarm-state
		if (measuredBattery > HYSTERESIS_VALUE)
			batteryLowCounter--;
		else
			batteryLowCounter = 10;

		if (batteryLowCounter <= 0)
		{
			batteryLowCounter = 0;
			SetBatteryAlarmState(false);
		}
	}
	else
	{
		if (measuredBattery < THRESHOLD_VALUE)
			batteryLowCounter++;
		else
			batteryLowCounter = 0;

		if (batteryLowCounter >= 10) // one second with undervoltage
		{
			batteryLowCounter = 10;
			SetBatteryAlarmState(true);
		}
	}
	DEBUG_PRINTLN(batteryLowCounter);
}

void SetupBatteryProtect()
{
	pinMode(PIN_CHECK_BATT, INPUT);
	nextTimeToCheckBattery = micros() + BATTERY_CHECK_INTERVALL;
}

bool AlarmIsActive = false;
bool HandleBatteryAlarm()
{
	bool AlarmIsActive = GetBatteryAlarmState();
	if (!AlarmIsActive)
		return false;

	static bool AlarmWasActive = false;

	if (AlarmWasActive && !AlarmIsActive)
	{	// return to normal state
		AlarmWasActive = false;
		DEBUG_PRINTLN("Battery alarm ends.");

		RestartAllServos();
	}
	else if (!AlarmWasActive && AlarmIsActive)
	{	// Go to power save mode.
		AlarmWasActive = true;
		DEBUG_PRINTLN("Battery alarm!");

		ShutdownAllServos();
		ShutDownLEDs();
	}

	return AlarmIsActive;
}

bool IsBatteryAlarmActive()
{
	return AlarmIsActive;
}