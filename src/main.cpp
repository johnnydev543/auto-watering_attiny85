#include <Arduino.h>
#include <tinysnore.h> // Include TinySnore Library

const int floatSwtichPin = 2;  // should have 10k pull-up resistor
const int moistureReadPin = 1; // should have 10k pull-up resistor
const int moistureOnPin = 0;   // should have 10k pull-down resistor
const int waterPumpPin = 3;    // should have current limiting resistor, pull-down resistor

int checkInterval = 30;        // moisture check interval, seconds
int checkCount = 0;
int forceStopSec = 120;             // seconds

uint32_t snoreDuration = 86400 * 1000; // milli seconds

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

        // into deep sleep mode
        snore(snoreDuration);
    }
}