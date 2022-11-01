#include "CoffinServos.h"
#include "PinConfig.h"
#include "CoffinMP3Player.h"

//#define DEBUG_COFFINSERVOS

#ifndef DEBUG_COFFINSERVOS
#undef DEBUG_PRINTLN
#undef DEBUG_PRINT
#define DEBUG_PRINTLN(a)
#define DEBUG_PRINT(a)
#endif

#define MIN_COVER 500
#define MAX_COVER 2500

#define MIN_CORPSE 500
#define MAX_CORPSE 2500

#define PWM_CORPSE_DOWN 2200
#define PWM_CORPSE_UP 880

#define PWM_COVER_OPEN 2100
#define PWM_ARM_TOUCHES_COVER 1350
#define PWM_COVER_CLOSED 1380

CoffinServo ServoCover(PIN_SERVO_COVER, MIN_COVER, MAX_COVER);
CoffinServo ServoCorpse(PIN_SERVO_CORPSE, MIN_CORPSE, MAX_CORPSE);

CoffinServo::CoffinServo(int pin, int min, int max)
{
	myPin = pin;
	myMin = min; // it is allowed that min is greater than max
	myMax = max;
	myPWM = (myMin + myMax) / 2;

	SmoothMode = false;
	TargetPWM = myPWM;
	increment = 0;
	PWMperStep = 0.0;
	TimeOfNextStep = millis();
}

void CoffinServo::Setup()
{
	RestartServo();
}

void CoffinServo::ShutdownServo()
{
	myServo.detach();
}

void CoffinServo::RestartServo()
{
	if (myMin < myMax)
		myServo.attach(myPin, myMin, myMax, myPWM);
	else
		myServo.attach(myPin, myMax, myMin, myPWM);
}

void CoffinServo::MoveTo(int pwm)
{
	if (pwm == myPWM)
		return;

	if (myMin < myMax)
	{
		if (pwm < myMin)
			pwm = myMin;
		else if (pwm > myMax)
			pwm = myMax;
	}
	else // (myMin > myMax)
	{
		if (pwm > myMin)
			pwm = myMin;
		else if (pwm < myMax)
			pwm = myMax;
	}

	myServo.writeMicroseconds(pwm);
	myPWM = pwm;
}

#define STEPTIME 20.0 // 50Hz servos take one setting every 20ms
void CoffinServo::MoveSmoothTo(int pwm, long time)
{
	DEBUG_PRINTLN("MoveSmoothTo " + String(pwm));
	if (myMin < myMax)
	{
		if (pwm < myMin)
			pwm = myMin;
		else if (pwm > myMax)
			pwm = myMax;
	}
	else // (myMin > myMax)
	{
		if (pwm > myMin)
			pwm = myMin;
		else if (pwm < myMax)
			pwm = myMax;
	}

	StartPWM = myPWM;
	TargetPWM = pwm;

	if (StartPWM == TargetPWM)
		return;

	if (time < STEPTIME)
		time = STEPTIME;

	double steps = double(time) / STEPTIME;
	PWMperStep = (TargetPWM - StartPWM) / steps;

	increment = 0;
	SmoothMode = true;
	TimeOfNextStep = millis();
	MakeSmoothStep();
}

void CoffinServo::MakeSmoothStep()
{
	if (TargetPWM == myPWM)
		return;

	increment++;
	int nextPWM = StartPWM + (PWMperStep * increment);
	if (PWMperStep > 0)
	{
		if (nextPWM > TargetPWM)
			nextPWM = TargetPWM;
	}
	else
	{
		if (nextPWM < TargetPWM)
			nextPWM = TargetPWM;
	}
	DEBUG_PRINTLN("Step: " + String(nextPWM));
	MoveTo(nextPWM);
	TimeOfNextStep += STEPTIME;
}

enum MovePending
{
	pending_none,
	pending_CorpseUp,
	pending_CoverClose

}MovePending;

void CoffinServo::Loop()
{
	if (!SmoothMode)
		return;

	if (millis() < TimeOfNextStep)
		return;

	if (TargetPWM == myPWM)
	{
		SmoothMode = false;
		DEBUG_PRINTLN("Smooth move done");
		if (MovePending == pending_CorpseUp)
		{
			MovePending = pending_none;
			CorpseUp();
		}
		else if (MovePending == pending_CoverClose)
		{
			MovePending = pending_none;
			CoverClose();
		}

		return;
	}

	MakeSmoothStep();
}

// -------------------------------------------------------

void SetupServos()
{
	ServoCover.Setup();
	ServoCover.MoveTo(PWM_COVER_CLOSED);
	ServoCorpse.Setup();
	ServoCorpse.MoveTo(PWM_CORPSE_DOWN);
}

void LoopServos()
{
	ServoCover.Loop();
	ServoCorpse.Loop();
}

bool  AllServosFinished()
{
	return ServoCover.IsMoveFinished() && ServoCorpse.IsMoveFinished();
}

void ShutdownAllServos()
{
	ServoCover.ShutdownServo();
	ServoCorpse.ShutdownServo();
}

void RestartAllServos()
{
	ServoCover.RestartServo();
	ServoCorpse.RestartServo();
}

#define TIME_TO_CLOSE 4500
#define TIME_TO_OPEN 6500

#define TIME_TO_RAISE 3500
#define TIME_TO_LAYDOWN 3000

void PlayCorpseUpDownSound()
{
	const int CorpsUpSoundSelection[] =
	{
		WANTYOUTODIE,
		TODESSCHREI,
		SCHREI,
		BRUELL,
		BRUELL2,
		GESPENST,
		GLOCKENSCHLAG,
		KICHERN,
		KRAEHE,
		MEINGERIPPE,
		WOLF1
	};

	int r = random(11);

	CoffinPlayMP3(CorpsUpSoundSelection[r]);
	DEBUG_PRINTLN("Corpse sound: " + String(no));
}


void CorpseUp()
{
	CoverOpen();
	if (!AllServosFinished())
	{
		MovePending = pending_CorpseUp;
		return;
	}
	if (ServoCorpse.GetPWM() == PWM_CORPSE_DOWN)
		PlayCorpseUpDownSound();

	ServoCorpse.MoveSmoothTo(PWM_CORPSE_UP, TIME_TO_RAISE);
}

void CorpseDown()
{
	if (ServoCorpse.GetPWM() == PWM_CORPSE_UP)
		PlayCorpseUpDownSound();

	ServoCorpse.MoveSmoothTo(PWM_CORPSE_DOWN, TIME_TO_LAYDOWN);
}

void CoverOpen()
{
	if (ServoCover.GetPWM() == PWM_COVER_CLOSED)
		CoffinPlayMP3(KNARREN);

	ServoCover.MoveSmoothTo(PWM_COVER_OPEN, TIME_TO_OPEN);
}

void CoverClose()
{
	CorpseDown();
	if (!AllServosFinished())
	{
		MovePending = pending_CoverClose;
		return;
	}

	if (ServoCover.GetPWM() == PWM_COVER_OPEN)
		CoffinPlayMP3(KNARREN2);

	ServoCover.MoveSmoothTo(PWM_COVER_CLOSED, TIME_TO_CLOSE);
}
