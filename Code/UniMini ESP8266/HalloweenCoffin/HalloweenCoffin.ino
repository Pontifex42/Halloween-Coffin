#include "PinConfig.h"
#include "BatteryProtect.h"
#include "CoffinServos.h"
#include "Ultrasonic.h"
#include "LEDs.h"
#include "Remote.h"
#include "CoffinMP3Player.h"

// #define DEBUG_INO
#ifndef DEBUG_INO
#undef DEBUG_PRINTLN
#undef DEBUG_PRINT
#define DEBUG_PRINTLN(a)
#define DEBUG_PRINT(a)
#endif

void SpecialDelay(double wait)
{
    unsigned long finish = millis() + wait;

    while (millis() < finish)
    {
        LoopServos();
        yield();
    }
}

void SpecialTone(int frequency, unsigned long duration = 0)
{
    tone(PIN_TONE, frequency, duration);
    LoopServos();
    yield();
}

void setup() 
{
    Serial.begin(115200);
    DEBUG_PRINTLN(""); // ESP sends garbage using 74880 baud at boot.
    DEBUG_PRINTLN("Halloween 2022");    // Serial monitor message

    SetupBatteryProtect();
    SetupServos();
    SetupUltrasonic();
    SetupLEDs();
    SetupRemote();
    SetupMP3Player();

    // After swiching on, give time to close cover and place the coffin
    delay(6000);
    CoffinPlayMP3(KICHERN);
    delay(6000);
    DEBUG_PRINTLN("Setup done");
}

#define DISTANCE_SLEEP 80
#define DISTANCE_ATTENTION 50
#define DISTANCE_ALARM 25
#define DISTANCE_HORROR 25

void loop() 
{
    LoopBatteryProtect();
    if (IsBatteryAlarmActive())
        return;

    LoopServos();

    LoopLEDs();

    LoopRemote();

    if (!AllServosFinished())
        return;

    LoopUltrasonic();
    
    LoopMP3Player();

    if (RemoteXYIsConnected())
    {
        DoRemoteControlledAction();
        return;
    }

    enum CoffinState
    {
        none, // only for boot
        sleep,
        attention,
        alarm,
        horror
    };
    static CoffinState currentState = none;
    if (distance >= DISTANCE_SLEEP)
    {
        if (currentState != sleep)
        {
            currentState = sleep;
            DEBUG_PRINTLN("Enter state: Sleep");
            SetEye(off);
            SetLight(false);
            CoverClose();
        }
    }
    else if (distance > DISTANCE_ATTENTION)
    {
        if (currentState != attention)
        {
            DEBUG_PRINTLN("Enter state: Attention");
            SetEye(off);
            SetLight(true);
            CoverClose();

            if ((currentState != horror) && (currentState != alarm)) // do not play music if target moves away
                CoffinPlayMP3(BRUELL);
            
            currentState = attention;
        }
    }
    else if (distance > DISTANCE_ALARM)
    {
        if (currentState != alarm)
        {
            currentState = alarm;
            DEBUG_PRINTLN("Enter state: Alarm");
            SetEye(glow);
            SetLight(true);
            CoverOpen();
            CorpseDown();
        }
    }
    else // if (distance <= DISTANCE_HORROR)
    {
        if (currentState != horror)
        {
            currentState = horror;
            DEBUG_PRINTLN("Enter state: Horror");
            CoffinPlayMP3(SCHREI);
            SetLight(true);
            SetEye(hypno);
            CorpseUp();
        }
    }
}
