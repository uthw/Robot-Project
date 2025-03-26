#include "motor_helpers.h"
#include <FEHUtility.h>

#define DARK_THRESHOLD 1.0
#define IS_DARK(voltage) (voltage > DARK_THRESHOLD)

#define BLUE_THRESHOLD_HIGH 0.65
#define BLUE_THRESHOLD_LOW 0.35
#define IS_BLUE(voltage) (voltage > BLUE_THRESHOLD_LOW && voltage < BLUE_THRESHOLD_HIGH)

#define RED_THRESHOLD_HIGH 0.33
#define RED_THRESHOLD_LOW 0.0
#define IS_RED(voltage) (voltage > RED_THRESHOLD_LOW && voltage < RED_THRESHOLD_HIGH)

AnalogInputPin lightSensor(FEHIO::Pin8); // It's like yellow red orange or something from screen to bottom

// Detects the start light illuminating before continuing
void detectStartDebug()
{
    LCD.WriteLine("Waiting to start");
    while (IS_DARK(lightSensor.Value())) {
        LCD.WriteLine(lightSensor.Value());
    }
    LCD.Clear();
    LCD.WriteLine("Starting");
}

// Same as detectStartDebug but without the LCD output
void detectStart()
{
    while (IS_DARK(lightSensor.Value())) {
    }
}

// Motors go forward at percent power until light sensor detects light
void goForwardUntilLight(int percent)
{

    goForward(percent);

    while (IS_DARK(lightSensor.Value())) { }

    stopMotors();
}

// Caution: will need to update inches since this was written while motor speed depended on battery
void humidifier_button()
{
    LCD.Clear();
    bool wasRed = false;
    if (IS_RED(lightSensor.Value())) {
        wasRed = true;
        turnRight(25, 10);
        LCD.Clear();

        LCD.WriteLine("RED");
    } else if (IS_BLUE(lightSensor.Value())) {
        turnLeft(25, 10);
        LCD.Clear();

        LCD.WriteLine("BLUE");
    } else {
        // Just do a random one
        if (rand() % 2 == 0) {
            wasRed = true;
            turnRight(25, 10);
            LCD.Clear();

            LCD.WriteLine("RED");
        } else {
            turnLeft(25, 10);
            LCD.Clear();

            LCD.WriteLine("BLUE");
        }
    }

    goForwardTimed(30, 1);

    if (wasRed) {
        turnLeft(25, 10);
    } else {
        turnRight(25, 10);
    }
}