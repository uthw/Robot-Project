#include "FEHLCD.h"
#include "FEHIO.h"
#include "FEHMotor.h"
#include "FEHBattery.h"
#include <algorithm>
#include <cmath>
using namespace std;

// // These may be different
// DigitalEncoder rightencoder(FEHIO::P0_0);
// DigitalEncoder leftencoder(FEHIO::P0_1);
// FEHMotor rightmotor(FEHMotor::Motor0, 9.0);
// FEHMotor leftmotor(FEHMotor::Motor1, 9.0);

// Convert desired degrees to number of counts required to rotate said number of degrees
#define DEGREES_TO_COUNTS(degrees) (int)(degrees * COUNTS_IN_90_DEGREES / 90)
#define INCHES_TO_COUNTS(inches) (int)(inches * COUNTS_IN_1_INCH)
#define HALF_INCHES_TO_COUNTS(halfInches) (int)(halfInches * COUNTS_IN_1_INCH / 2)

// Motors have less power as battery decreases
#define ACTUAL_PERCENTAGE_POWER(percent) min((int)((11.5 / Battery.Voltage()) * percent), 100)

#define COUNTS_IN_90_DEGREES 215
#define COUNTS_IN_1_INCH 32

#define LEFT_MODIFIER 1 // Added to left motor speed. Set to 0 if it's going straight right now

void turnRight(int percent, int degrees, DigitalEncoder leftencoder, DigitalEncoder rightencoder, FEHMotor leftmotor, FEHMotor rightmotor)
{
    // Convert degrees to counts
    int counts = DEGREES_TO_COUNTS(degrees);
    int actualPercent = percent;

    // Reset counts
    rightencoder.ResetCounts();
    leftencoder.ResetCounts();

    // Set motors to desired percent
    rightmotor.SetPercent(actualPercent * -1);
    leftmotor.SetPercent(actualPercent + LEFT_MODIFIER);

    // Run motors until avg of left and right encoder equals counts
    while ((leftencoder.Counts() + rightencoder.Counts()) / 2.0 < counts)
        ;

    // Turn off motors
    rightmotor.Stop();
    leftmotor.Stop();
}

void turnLeft(int percent, int degrees, DigitalEncoder leftencoder, DigitalEncoder rightencoder, FEHMotor leftmotor, FEHMotor rightmotor)
{
    // Convert degrees to counts
    int counts = DEGREES_TO_COUNTS(degrees);
    int actualPercent = percent;

    // Reset counts
    rightencoder.ResetCounts();
    leftencoder.ResetCounts();

    // Set motors to desired percent
    rightmotor.SetPercent(actualPercent + LEFT_MODIFIER);
    leftmotor.SetPercent(actualPercent * -1);

    // Run motors until avg of left and right encoder equals counts
    while ((leftencoder.Counts() + rightencoder.Counts()) / 2.0 < counts)
        ;

    // Turn off motors
    rightmotor.Stop();
    leftmotor.Stop();
}

// Motors go forward at percent power for inches
void goForward(int percent, int inches, DigitalEncoder leftencoder, DigitalEncoder rightencoder, FEHMotor leftmotor, FEHMotor rightmotor)
{
    // Convert inches to counts
    // Could change this to mm later if more precision needed
    int counts = INCHES_TO_COUNTS(inches);
    int actualPercent = percent;

    // Reset counts
    rightencoder.ResetCounts();
    leftencoder.ResetCounts();

    // Set motors to desired percent
    rightmotor.SetPercent(actualPercent);
    leftmotor.SetPercent(actualPercent + LEFT_MODIFIER);

    // Run motors until avg of left and right encoder equals counts
    while ((leftencoder.Counts() + rightencoder.Counts()) / 2.0 < counts)
        ;

    // Turn off motors
    rightmotor.Stop();
    leftmotor.Stop();
}

// Motors go forward at percent power for inches
void goForwardHalfInches(int percent, int halfInches, DigitalEncoder leftencoder, DigitalEncoder rightencoder, FEHMotor leftmotor, FEHMotor rightmotor)
{
    // Convert inches to counts
    // Could change this to mm later if more precision needed
    int counts = HALF_INCHES_TO_COUNTS(halfInches);
    int actualPercent = percent;

    // Reset counts
    rightencoder.ResetCounts();
    leftencoder.ResetCounts();

    // Set motors to desired percent
    rightmotor.SetPercent(actualPercent);
    leftmotor.SetPercent(actualPercent + LEFT_MODIFIER);

    // Run motors until avg of left and right encoder equals counts
    while ((leftencoder.Counts() + rightencoder.Counts()) / 2.0 < counts)
        ;

    // Turn off motors
    rightmotor.Stop();
    leftmotor.Stop();
}