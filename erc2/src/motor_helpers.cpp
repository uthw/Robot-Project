using namespace std;
#include <FEH.h>
#include <FEHIO.h>
#include <FEHMotor.h>
// Cmath and algorithm have been killed by ERC2
#include "utils.h"
#include <math.h>

DigitalEncoder rightEncoder(FEHIO::Pin9);
DigitalEncoder leftEncoder(FEHIO::Pin10);
FEHMotor rightMotor(FEHMotor::Motor0, 9.0);
FEHMotor leftMotor(FEHMotor::Motor1, 9.0);

// Convert desired degrees to number of counts required to rotate said number of degrees
#define DEGREES_TO_COUNTS(degrees) (int)(degrees * COUNTS_IN_90_DEGREES / 90)
#define INCHES_TO_COUNTS(inches) (int)(inches * COUNTS_IN_1_INCH)
#define HALF_INCHES_TO_COUNTS(halfInches) (int)(halfInches * COUNTS_IN_1_INCH / 2)

// Motors have less power as battery decreases
#define ACTUAL_PERCENTAGE_POWER(percent) min((int)((MAX_VOLTAGE / Battery.Voltage()) * percent), 100)

#define COUNTS_IN_90_DEGREES 200 // Old: 215
#define COUNTS_IN_1_INCH 20 // Old: 32

#define LEFT_MODIFIER 1 // Added to left motor speed. Set to 0 if it's going straight right now

void turnRight(int percent, int degrees)
{
    // Convert degrees to counts
    int counts = DEGREES_TO_COUNTS(degrees);
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);

    // Reset counts
    rightEncoder.ResetCounts();
    leftEncoder.ResetCounts();

    // Set motors to desired percent
    rightMotor.SetPercent(actualPercent * -1);
    leftMotor.SetPercent(actualPercent + LEFT_MODIFIER);

    // Run motors until avg of left and right encoder equals counts
    while ((leftEncoder.Counts() + rightEncoder.Counts()) / 2.0 < counts)
        ;

    // Turn off motors
    rightMotor.Stop();
    leftMotor.Stop();
}

void turnLeft(int percent, int degrees)
{
    // Convert degrees to counts
    int counts = DEGREES_TO_COUNTS(degrees);
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);

    // Reset counts
    rightEncoder.ResetCounts();
    leftEncoder.ResetCounts();

    // Set motors to desired percent
    rightMotor.SetPercent(actualPercent + LEFT_MODIFIER);
    leftMotor.SetPercent(actualPercent * -1);

    // Run motors until avg of left and right encoder equals counts
    while ((leftEncoder.Counts() + rightEncoder.Counts()) / 2.0 < counts)
        ;

    // Turn off motors
    rightMotor.Stop();
    leftMotor.Stop();
}

// Motors go forward at percent power for inches
void goForward(int percent, float inches)
{
    // Convert inches to counts
    // Could change this to mm later if more precision needed
    int counts = INCHES_TO_COUNTS(inches);
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);

    // Reset counts
    rightEncoder.ResetCounts();
    leftEncoder.ResetCounts();

    // Set motors to desired percent
    rightMotor.SetPercent(actualPercent);
    leftMotor.SetPercent(actualPercent + LEFT_MODIFIER);

    // Run motors until avg of left and right encoder equals counts
    while ((leftEncoder.Counts() + rightEncoder.Counts()) / 2.0 < counts)
        ;

    // Turn off motors
    rightMotor.Stop();
    leftMotor.Stop();
}

// Motors go forward at percent power for seconds
void goForwardTimed(int percent, float seconds)
{
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);

    rightMotor.SetPercent(actualPercent);
    leftMotor.SetPercent(actualPercent + LEFT_MODIFIER);

    Sleep(seconds);

    rightMotor.Stop();
    leftMotor.Stop();
}

void goForward(int percent) {
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);

    rightMotor.SetPercent(actualPercent);
    leftMotor.SetPercent(actualPercent + LEFT_MODIFIER);
}

void stopMotors() {
    rightMotor.Stop();
    leftMotor.Stop();
}

