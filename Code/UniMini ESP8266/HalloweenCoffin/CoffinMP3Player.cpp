#include "DFRobotDFPlayerMini.h"
#include "CoffinMP3Player.h"

DFRobotDFPlayerMini myDFPlayer;

void SetupMP3Player()
{
	Serial1.begin(9600);
    myDFPlayer.begin(Serial1);
	myDFPlayer.setTimeOut(500);
	myDFPlayer.volume(20);  // range 0 - 30
	myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
	myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
}

#define KNARREN 1
#define WANTYOUTODIE 2
#define TODESSCHREI 3
#define SCHREI 4

void CoffinPlayMP3(int no)
{
    myDFPlayer.play(no);
	switch (no)
	{
	case KNARREN:
		myDFPlayer.volume(30);
		delay(100);
		break;
	case WANTYOUTODIE:
		myDFPlayer.volume(30);
		delay(100);
		break;
	case TODESSCHREI:
		myDFPlayer.volume(8);
		delay(100);
		break;
	case SCHREI:
		myDFPlayer.volume(20);
		delay(100);
		break;
	}
}
