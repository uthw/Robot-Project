// Battery needs to be around 10V for this to work. You also need to remove use of ACTUAL_PERCENTAGE_POWER in motor_helpers since this was written before that was added.
// COUNTS_IN_90_DEGREES: 215
// COUNTS_IN_1_INCH: 32
// LEFT_MODIFIER: 1

#include "FEHIO.h"
#include "motor_helpers.h"
#include <FEHBattery.h>
#include <FEHLCD.h>
#include <FEHMotor.h>
#include <FEHRCS.h>
#include <FEHSD.h>
#include <FEHServo.h>
#include <FEHUtility.h>
#include <algorithm>
#include <cmath>
using namespace std;

#define MAX_SPEED 20
#define MIN_SPEED 10
#define LOW_BATTERY 3.0
#define LEFT_MODIFIER 1 // Added to left motor speed. Set to 0 if it's going straight right now

// Convert desired degrees to number of counts required to rotate said number of degrees
#define DEGREES_TO_COUNTS(degrees) (int)(degrees * COUNTS_IN_90_DEGREES / 90)
#define INCHES_TO_COUNTS(inches) (int)(inches * COUNTS_IN_1_INCH)

DigitalEncoder rightencoder(FEHIO::P0_0);
DigitalEncoder leftencoder(FEHIO::P0_1);
FEHMotor rightmotor(FEHMotor::Motor0, 9.0);
FEHMotor leftmotor(FEHMotor::Motor1, 9.0);

#define DARK_THRESHOLD 1.0
#define IS_DARK(voltage) (voltage > DARK_THRESHOLD)

#define BLUE_THRESHOLD_HIGH 0.65
#define BLUE_THRESHOLD_LOW 0.35
#define IS_BLUE(voltage) (voltage > BLUE_THRESHOLD_LOW && voltage < BLUE_THRESHOLD_HIGH)

#define RED_THRESHOLD_HIGH 0.33
#define RED_THRESHOLD_LOW 0.0
#define IS_RED(voltage) (voltage > RED_THRESHOLD_LOW && voltage < RED_THRESHOLD_HIGH)

AnalogInputPin lightSensor(FEHIO::P1_0);

void goForwardUntilLight(int percent)
{

    rightmotor.SetPercent(percent);
    leftmotor.SetPercent(percent + LEFT_MODIFIER);

    while (IS_DARK(lightSensor.Value())) { }
    rightmotor.SetPercent(0);
    leftmotor.SetPercent(0);
}
void humidifier_button()
{
    LCD.Clear();
    bool wasRed = false;
    if (IS_RED(lightSensor.Value())) {
        wasRed = true;
        turnRight(25, 10, leftencoder, rightencoder, leftmotor, rightmotor);
        LCD.Clear();

        LCD.WriteLine("RED");
    } else if (IS_BLUE(lightSensor.Value())) {
        turnLeft(25, 10, leftencoder, rightencoder, leftmotor, rightmotor);
        LCD.Clear();

        LCD.WriteLine("BLUE");
    } else {
        // LCD.WriteLine("ERROR READING COLOR DURING HUMIDIFIER TASK");

        // Just do a random one
        if (rand() % 2 == 0) {
            wasRed = true;
            turnRight(25, 10, leftencoder, rightencoder, leftmotor, rightmotor);
            LCD.Clear();

            LCD.WriteLine("RED");
        } else {
            turnLeft(25, 10, leftencoder, rightencoder, leftmotor, rightmotor);
            LCD.Clear();

            LCD.WriteLine("BLUE");
        }
    }

    leftmotor.SetPercent(30);
    rightmotor.SetPercent(30);
    Sleep(1.0);
    leftmotor.SetPercent(0);
    rightmotor.SetPercent(0);

    if (wasRed) {
        turnLeft(25, 10, leftencoder, rightencoder, leftmotor, rightmotor);
    } else {
        turnRight(25, 10, leftencoder, rightencoder, leftmotor, rightmotor);
    }
}

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

int main()
{
    LCD.Clear();

    detectStartDebug();
    turnRight(25, 45, leftencoder, rightencoder, leftmotor, rightmotor);
    goForward(40, 37, leftencoder, rightencoder, leftmotor, rightmotor);
    turnLeft(25, 90, leftencoder, rightencoder, leftmotor, rightmotor);
    goForward(-25, 8, leftencoder, rightencoder, leftmotor, rightmotor);
    goForward(40, 23, leftencoder, rightencoder, leftmotor, rightmotor);
    // goForward(-40, 6, leftencoder, rightencoder, leftmotor, rightmotor);

    // BONUS
    goForward(-40, 2, leftencoder, rightencoder, leftmotor, rightmotor);
    Sleep(0.25);
    turnRight(25, 15, leftencoder, rightencoder, leftmotor, rightmotor);
    Sleep(0.25);

    goForward(40, 2, leftencoder, rightencoder, leftmotor, rightmotor);
    Sleep(0.25);
    turnLeft(25, 10, leftencoder, rightencoder, leftmotor, rightmotor);
    Sleep(0.25);
    goForward(40, 3, leftencoder, rightencoder, leftmotor, rightmotor);
    Sleep(0.25);
    goForward(-40, 20, leftencoder, rightencoder, leftmotor, rightmotor);

    // while (true) {
    //     // show battery percent
    //     LCD.Write("Battery: ");
    //     LCD.Write(Battery.Voltage());
    //     LCD.WriteLine(" V");
    //     Sleep(0.25);
    //     LCD.Clear();
    // }
}
