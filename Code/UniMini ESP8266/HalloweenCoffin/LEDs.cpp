#include "PinConfig.h"
#include "LEDs.h"

// #define DEBUG_LEDS
#ifndef DEBUG_LEDS
#undef DEBUG_PRINTLN
#undef DEBUG_PRINT
#define DEBUG_PRINTLN(a)
#define DEBUG_PRINT(a)
#endif

EyeMode myEyeMode = off;

#define EYE_MIN_BRIGHT 245  // inverted polarity on this pin
#define EYE_MAX_BRIGHT 0
#define HYPNO_TIME 3

int hypnoValue = EYE_MIN_BRIGHT;
int hypnoInc = 3;
int nextHypnoTime = 0;

void HypnoStep()
{
    unsigned now = millis();
    if (now < nextHypnoTime)
        return;

    nextHypnoTime = now + HYPNO_TIME;

    hypnoValue += hypnoInc;
    if (hypnoValue < EYE_MAX_BRIGHT)
    {
        hypnoValue = EYE_MAX_BRIGHT;
        hypnoInc *= -1;
    }
    else if (hypnoValue > EYE_MIN_BRIGHT)
    {
        hypnoValue = EYE_MIN_BRIGHT;
        hypnoInc *= -1;
    }
    analogWrite(PIN_LED_EYES, hypnoValue);
}


void SetEye(EyeMode mode)
{
    myEyeMode = mode;
    if (myEyeMode == off)
    {
        analogWrite(PIN_LED_EYES, 255); // inverted polarity on this pin
        DEBUG_PRINTLN("Eyes off");
    }
    else if (myEyeMode == glow)
    {
        analogWrite(PIN_LED_EYES, 160); // a bit of red glowing
        DEBUG_PRINTLN("Eyes glow");
    }
    else if (myEyeMode == hypno)
    {
        nextHypnoTime = millis();
        HypnoStep();
        DEBUG_PRINTLN("Eyes hypno");
    }
}

void SetLight(bool onoroff)
{
    digitalWrite(PIN_LED_WHITE_A, onoroff ? HIGH : LOW);
    // PIN_LED_WHITE_A remains low forever
}

void SetupLEDs()
{
    pinMode(PIN_LED_EYES, OUTPUT);
    analogWrite(PIN_LED_EYES, 0);
    pinMode(PIN_LED_WHITE_A, OUTPUT);
    pinMode(PIN_LED_WHITE_B, OUTPUT);
    digitalWrite(PIN_LED_WHITE_A, LOW);
    digitalWrite(PIN_LED_WHITE_B, LOW);
}

void LoopLEDs()
{
    if (myEyeMode == hypno)
    {
        HypnoStep();
    }
}

void ShutDownLEDs()
{
    SetLight(false);
    SetEye(off);
}