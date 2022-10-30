#include "DFRobotDFPlayerMini.h"
#include "CoffinMP3Player.h"
#include "PinConfig.h"

DFRobotDFPlayerMini myDFPlayer;

unsigned long lastSoundTime;
void CoffinPlayMP3(int no)
{
	switch (no)
	{
	case KNARREN:
	case KNARREN2:
	case WANTYOUTODIE:
	case BRUELL:
	case BRUELL2:
		myDFPlayer.volume(30);
		break;
	case TODESSCHREI:
		myDFPlayer.volume(10);
		break;
	case KICHERN:
		myDFPlayer.volume(25);
		break;
	case SCHREI:
	default: 
		myDFPlayer.volume(20);
		break;
	}
	delay(100); // let volume setting become active
	lastSoundTime = millis();
	myDFPlayer.play(no);
}

#define SILENCE_LIMIT 30000
void SetupMP3Player()
{
	Serial1.begin(9600);
	myDFPlayer.begin(Serial1);
	myDFPlayer.setTimeOut(500);
	myDFPlayer.volume(20);  // range 0 - 30
	myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
	myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
	randomSeed(analogRead(PIN_CHECK_BATT));
	lastSoundTime = millis() + SILENCE_LIMIT;
}

void PlayRandomSound()
{
	int no = random(KNARREN, WOLF2 + 1);
	CoffinPlayMP3(no);
	DEBUG_PRINTLN("Random sound: " + String(no));
}

void LoopMP3Player()
{
	unsigned long now = millis();
	if ((now - lastSoundTime) < SILENCE_LIMIT)
		return;
	PlayRandomSound();
}