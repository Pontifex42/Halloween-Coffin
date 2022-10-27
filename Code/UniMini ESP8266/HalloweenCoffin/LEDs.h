#pragma once

void SetupLEDs();
void LoopLEDs();

void SetLight(bool onoroff);
enum EyeMode
{
	off,
	glow,
	hypno
};

void SetEye(EyeMode mode);

void ShutDownLEDs();