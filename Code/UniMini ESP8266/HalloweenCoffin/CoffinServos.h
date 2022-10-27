#pragma once

#include <Servo.h>
#include "PinConfig.h"

// Servo timing is usually in a range of 1000us up to 2000us, but some servos act on a range 500us-2500us and some do more
#define PWM_MIN	500
#define PWM_MAX 2500

class CoffinServo
{
public:
	CoffinServo(int pin, int min = PWM_MIN, int max = PWM_MAX); 	// set min is greater than max if you want to reverse directions
	void Setup();
	void ShutdownServo();
	void RestartServo();
	void MoveTo(int pwm);
	void MoveSmoothTo(int pwm, long time);
	bool IsMoveFinished() { return !SmoothMode;  };
	void Loop();	
	int GetPWM() { return myPWM; };
private:
	Servo myServo;
	int myPin;
	int myPWM;
	int myMin;
	int myMax;
	// addon: smooth movement
	void MakeSmoothStep();
	bool SmoothMode;
	int StartPWM;
	int TargetPWM;
	double PWMperStep;
	int increment;
	int TimeOfNextStep;
};

void SetupServos();
void LoopServos();
bool AllServosFinished();
void RestartAllServos();
void ShutdownAllServos();

void CorpseUp();
void CorpseDown();
void CoverOpen();
void CoverClose();

extern CoffinServo ServoCover;
extern CoffinServo ServoCorpse;
