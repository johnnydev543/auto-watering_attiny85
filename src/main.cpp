#include <Arduino.h>
#include <SimpleSleep.h>

SimpleSleep Sleep;
SimpleSleep_Cal SleepCal;

const int floatSwtichPin = PB2;     // should have 10k pull-up resistor
const int moistureReadPin = PB1;    // should have 10k pull-up resistor
const int moistureOnPin = PB0;      // should have 10k pull-down resistor
const int waterPumpPin = PB3;       // should have current limiting resistor, pull-down resistor
const int wateringDurationPin = A2; // should have a 10K VR connect to A2(PB4)

int checkCount = 0;

long analogVal = 0;

// watering duration, controlled by a VR from min to max
// Unit: seconds
int wateringDuration = 30;
int wateringDurationMin = 10;
int wateringDurationMax = 60;
int forceStopSec = 120;

int checkInterval = 10;

uint32_t snoreDuration = 28800000U; // milli seconds

void setup()
{
    SleepCal = Sleep.getCalibration();
    pinMode(floatSwtichPin, INPUT);
    pinMode(moistureReadPin, INPUT);
    pinMode(moistureOnPin, OUTPUT);
    pinMode(waterPumpPin, OUTPUT);

    // read watering duration from VR
    analogVal = analogRead(wateringDurationPin);
    wateringDuration = map(analogVal, 0, 1023, wateringDurationMin, wateringDurationMax);
    if (wateringDurationMin >= checkInterval)
    {
        checkInterval = wateringDurationMin / 2; // moisture check interval, seconds
    }
}

void loop()
{
    int needWatering = false;

    // Read the water level switch to see if water is avaiable
    int floatSwitch = digitalRead(floatSwtichPin);

    // float switch == low means has water
    if (floatSwitch == LOW)
    {

        // Water is avaiable
        // Power on moisture sensor
        digitalWrite(moistureOnPin, HIGH);

        // milli seconds, make sure the sensor will have time to turn on
        delay(100);

        // read soil moisture value
        int moistureRead = digitalRead(moistureReadPin);

        delay(100);

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
    int checkDuration = checkInterval * checkCount;
    if (checkDuration >= forceStopSec || checkDuration >= wateringDuration)
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
        Sleep.deeplyFor(snoreDuration, SleepCal);
    }
}