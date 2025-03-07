#include "FEHIO.h"
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
// these conversions will need to account for battery lvl later
#define COUNTS_IN_90_DEGREES 210
#define COUNTS_IN_1_INCH 20
#define LEFT_MODIFIER 0 // Added to left motor speed. Set to 0 if it's going straight right now

// Convert desired degrees to number of counts required to rotate said number of degrees
#define DEGREES_TO_COUNTS(degrees) (int)(degrees * COUNTS_IN_90_DEGREES / 90)
#define INCHES_TO_COUNTS(inches) (int)(inches * COUNTS_IN_1_INCH)

// Motors have less power as battery decreases
#define ACTUAL_PERCENTAGE_POWER(percent) min((int)((11.5 / Battery.Voltage()) * percent), 100)

// These may be different
DigitalEncoder rightencoder(FEHIO::P0_0);
DigitalEncoder leftencoder(FEHIO::P0_1);
FEHMotor rightmotor(FEHMotor::Motor0, 9.0);
FEHMotor leftmotor(FEHMotor::Motor1, 9.0);

#define DARK_THRESHOLD 1.0
#define IS_DARK(voltage) (voltage > DARK_THRESHOLD)

// TODO: ADD THESE
#define BLUE_THRESHOLD
#define IS_BLUE(voltage) (false)

#define RED_THRESHOLD
#define IS_RED(voltage) (false)

AnalogInputPin lightSensor(FEHIO::P1_0);

void turnRight(int percent, int degrees)
{
    // Convert degrees to counts
    int counts = DEGREES_TO_COUNTS(degrees);

    LCD.WriteLine(counts);

    // Reset counts
    rightencoder.ResetCounts();
    leftencoder.ResetCounts();

    // Set motors to desired percent
    rightmotor.SetPercent(percent * -1);
    leftmotor.SetPercent(percent + LEFT_MODIFIER);

    // Run motors until avg of left and right encoder equals counts
    while ((leftencoder.Counts() + rightencoder.Counts()) / 2.0 < counts)
        ;

    // Turn off motors
    rightmotor.Stop();
    leftmotor.Stop();
}

void turnLeft(int percent, int degrees)
{
    // Convert degrees to counts
    int counts = DEGREES_TO_COUNTS(degrees);

    LCD.WriteLine(counts);

    // Reset counts
    rightencoder.ResetCounts();
    leftencoder.ResetCounts();

    // Set motors to desired percent
    rightmotor.SetPercent(percent + LEFT_MODIFIER);
    leftmotor.SetPercent(percent * -1);

    // Run motors until avg of left and right encoder equals counts
    while ((leftencoder.Counts() + rightencoder.Counts()) / 2.0 < counts)
        ;

    // Turn off motors
    rightmotor.Stop();
    leftmotor.Stop();
}

// Motors go forward at percent power for inches
void goForward(int percent, int inches)
{
    // Convert inches to counts
    // Could change this to mm later if more precision needed
    int counts = INCHES_TO_COUNTS(inches);

    LCD.Clear();
    LCD.WriteLine(counts);
    LCD.WriteLine(leftencoder.Counts());
    LCD.WriteLine(rightencoder.Counts());

    // Reset counts
    rightencoder.ResetCounts();
    leftencoder.ResetCounts();

    // Set motors to desired percent
    rightmotor.SetPercent(percent);
    leftmotor.SetPercent(percent + LEFT_MODIFIER);

    // Run motors until avg of left and right encoder equals counts
    while ((leftencoder.Counts() + rightencoder.Counts()) / 2.0 < counts)
        ;

    // Turn off motors
    rightmotor.Stop();
    leftmotor.Stop();
}
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
    if (IS_RED(lightSensor.Value())) {
        turnLeft(25, 200);
    } else if (IS_BLUE(lightSensor.Value())) {
        turnRight(25, 200);
    } else {
        LCD.WriteLine("ERROR READING COLOR DURING HUMIDIFIER TASK");
    }
    goForward(30, 8);
    goForward(-30, 10);
}

int main()
{
    LCD.Clear();
    LCD.WriteLine("RUNNING");
    while (IS_DARK(lightSensor.Value())) {
        LCD.WriteLine(lightSensor.Value());
    }

    LCD.WriteLine("STARTING");

    turnRight(25, 45);
    goForward(40, 41);
    turnLeft(25, 90);

    goForwardUntilLight(40);

    // Find button
    // Read color
    // Press correct button
}
