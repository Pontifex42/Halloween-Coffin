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

#if NO_MP3PLAYER
void play_music()
{                           // Method that executes the buzzer sound.
    SpecialTone(261, 117.1875);             // tone(pin, frequency, duration).
    SpecialDelay(130.208333333 + 376.041666667);
    SpecialTone(311, 116.25);
    SpecialDelay(129.166666667 + 362.5);
    SpecialTone(391, 124.6875);
    SpecialDelay(138.541666667 + 371.875);
    SpecialTone(523, 119.0625);
    SpecialDelay(132.291666667 + 347.916666667);
    SpecialDelay(3.125 + 3.125);
    SpecialTone(261, 834.375);
    SpecialDelay(927.083333333);
    SpecialTone(311, 11.25);
    SpecialDelay(12.5);
    SpecialTone(184, 17.8125);
    SpecialDelay(19.7916666667);
    SpecialTone(415, 16.875);
    SpecialDelay(18.75);
    SpecialTone(103, 13.125);
    SpecialDelay(14.5833333333 + 17.7083333333 + 2.08333333333 + 1.04166666667 + 2.08333333333);
    SpecialTone(293, 79.6875);
    SpecialDelay(88.5416666667);
    SpecialTone(246, 2.8125);
    SpecialDelay(3.125);
    SpecialTone(391, 48.75);
    SpecialDelay(54.1666666667 + 96.875);
    SpecialTone(349, 85.3125);
    SpecialDelay(94.7916666667 + 162.5);
    SpecialTone(311, 72.1875);
    SpecialDelay(80.2083333333 + 173.958333333);
    SpecialTone(293, 87.1875);
    SpecialDelay(96.875 + 167.708333333);
    
    noTone(PIN_TONE);
    digitalWrite(PIN_TONE, LOW); // Save power!
}
#endif

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

    // After swich on, give time to close cover and place the coffin
    yield();
    delay(1000);
    yield();
    delay(1000);
    yield();
    delay(1000);
    CoffinPlayMP3(SCHREI);
    DEBUG_PRINTLN("Setup done");
}

#define DISTANCE_SLEEP 60
#define DISTANCE_ATTENTION 35
#define DISTANCE_ALARM 15
#define DISTANCE_HORROR 15

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
#if NO_MP3PLAYER
            if ((currentState != horror) && (currentState != alarm)) // do not play music if target moves away
                play_music(); // Method call to play the melody.
#endif
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
