#include "PinConfig.h"


//#define DEBUG_ULTRASONIC

#ifndef DEBUG_ULTRASONIC
#undef DEBUG_PRINTLN
#undef DEBUG_PRINT
#define DEBUG_PRINTLN(a)
#define DEBUG_PRINT(a)
#endif

#define LOWPASS_SIZE 5
long buf[LOWPASS_SIZE];
long Lowpass(long val)
{
    static int writeIdx = 0;
    buf[writeIdx] = val;
    writeIdx++;
    if (writeIdx >= LOWPASS_SIZE)
        writeIdx = 0;

    int sum = 0;
    for (int i = 0; i < LOWPASS_SIZE; ++i)
    {
        sum += buf[i];
    }
    return sum / LOWPASS_SIZE;
}


long distance;                            // Definition of the distance variable to store the value of the distance to an object

long  measure_distance() 
{

    // Method for measuring the distance
    digitalWrite(PIN_ULTRASONIC_TRIGGER, LOW);                        // set the trigger pin of the HC-SR04 sensor to low level for 5 us.
    delayMicroseconds(5);
    digitalWrite(PIN_ULTRASONIC_TRIGGER, HIGH);                       // set the trigger pin of the HC-SR04 sensor to high level for 10 us.       
    delayMicroseconds(10);
    digitalWrite(PIN_ULTRASONIC_TRIGGER, LOW);                        // Again set the trigger pin of the HC-SR04 sensor to low level.

    long ultrasonic_time_measurement = pulseIn(PIN_ULTRASONIC_ECHO, HIGH); // With pulseIn we measure the time between pulses, 
                                                                      // in microseconds, and store it in the variable ultrasonic_time_measurement.

#define CENTIMETER_PER_MICROSECOND 0.01715 //  0.0343/2 cause its the time to target and back from target
    int cm = ultrasonic_time_measurement * CENTIMETER_PER_MICROSECOND;          // convert time into centimeters.

    DEBUG_PRINTLN(String(cm) + " cm");
    return Lowpass(cm);
}

#define MEASURE_INTERVAL 333 // three times a second will do nicely
static unsigned long NextTimeToMeasure;
void SetupUltrasonic()
{
	pinMode(PIN_ULTRASONIC_TRIGGER, OUTPUT);
	pinMode(PIN_ULTRASONIC_ECHO, INPUT);
    NextTimeToMeasure = millis() + MEASURE_INTERVAL;
    for (int i = 0; i < LOWPASS_SIZE; ++i)
    {
        buf[LOWPASS_SIZE] = 200;
    }
    distance = 200;
}


void LoopUltrasonic()
{
    unsigned long now = millis();
    if (now < NextTimeToMeasure)
        return;

    NextTimeToMeasure = now + MEASURE_INTERVAL;

	distance = measure_distance();                   // Called the method of measuring the distance to objects.
}
