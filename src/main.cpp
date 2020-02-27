#include <Arduino.h>
#include <tinysnore.h> // Include TinySnore Library

const int floatSwtichPin = PB2;  // should have 10k pull-up resistor
const int moistureReadPin = PB1; // should have 10k pull-up resistor
const int moistureOnPin = PB0;   // should have 10k pull-down resistor
const int waterPumpPin = PB3;    // should have current limiting resistor, pull-down resistor
const int snoreDurationPin = A2; // should have a 10K VR connect to A2(PB4)

int checkInterval = 30; // moisture check interval, seconds
int checkCount = 0;
int forceStopSec = 120; // seconds

long analogVal = 0;

// snore duration, here to specify a default value, actually control by a VR
long snoreDurationMin = 3600;
long snoreDurationMax = 86400;

uint32_t snoreDuration;

void setup()
{
    pinMode(floatSwtichPin, INPUT_PULLUP);
    pinMode(moistureReadPin, INPUT);
    pinMode(moistureOnPin, OUTPUT);
    pinMode(waterPumpPin, OUTPUT);
}

void loop()
{
    bool needWatering = false;

    // Read the water level switch to see if water is avaiable
    bool floatSwitch = digitalRead(floatSwtichPin);

    // float switch == low means has water
    if (floatSwitch == LOW)
    {

        // Water is avaiable
        // Power on moisture sensor
        digitalWrite(moistureOnPin, HIGH);

        // milli seconds, make sure the sensor will have time to turn on
        delay(20);

        // read soil moisture value
        int moistureRead = digitalRead(moistureReadPin);

        delay(20);

        // Power off moisture sensor
        digitalWrite(moistureOnPin, LOW);

        if (moistureRead == HIGH)
        {
            needWatering = true;
        }
    }
    else
    {
        // Water is not avaiable
    }

    // force stop if watering time is more than X seconds
    // in case the moisture sensor or something else out of control.
    if (checkInterval * checkCount >= forceStopSec)
    {
        // Force stop watering
        needWatering = false;
    }

    if (needWatering == true)
    {

        // water pump on
        digitalWrite(waterPumpPin, HIGH);

        // delay milli seconds
        delay(checkInterval * 1000);

        checkCount++;
    }
    else
    {

        // close water pump whether it's on or not
        digitalWrite(waterPumpPin, LOW);

        // reset check counter
        checkCount = 0;

        analogVal = analogRead(snoreDurationPin);

        // transform the value ranging from 1 hour(3600 seconds) to 24 hours(86400 seconds)
        analogVal = map(analogVal, 1, 1023, snoreDurationMin, snoreDurationMax);

        snoreDuration = analogVal * 1000; // milli seconds

        // into deep sleep mode
        snore(snoreDuration);
    }
}