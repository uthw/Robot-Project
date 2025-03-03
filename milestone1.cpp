#include "FEHIO.h"
#include <FEHBattery.h>
#include <FEHLCD.h>
#include <FEHMotor.h>
#include <FEHSD.h>
#include <FEHServo.h>
#include <FEHUtility.h>
#include <cmath>

#define MAX_SPEED 20
#define MIN_SPEED 10
#define LOW_BATTERY 3.0
// these conversions will need to account for battery lvl later
#define COUNTS_IN_90_DEGREES 110 * 2
#define COUNTS_IN_1_INCH 20 * (17 / 10.5)

// Convert desired degrees to number of counts required to rotate said number of degrees
#define DEGREES_TO_COUNTS(degrees) (int)(degrees * COUNTS_IN_90_DEGREES / 90)
#define INCHES_TO_COUNTS(inches) (int)(inches * COUNTS_IN_1_INCH)

// These may be different
DigitalEncoder rightencoder(FEHIO::P0_0);
DigitalEncoder leftencoder(FEHIO::P0_1);
FEHMotor rightmotor(FEHMotor::Motor0, 9.0);
FEHMotor leftmotor(FEHMotor::Motor1, 9.0);

// temporary most likely - just for testing
DigitalInputPin bumpswitch(FEHIO::P0_7);

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
    leftmotor.SetPercent(percent);

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
    rightmotor.SetPercent(percent);
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
    leftmotor.SetPercent(percent);

    // Run motors until avg of left and right encoder equals counts
    while ((leftencoder.Counts() + rightencoder.Counts()) / 2.0 < counts)
        ;

    // Turn off motors
    rightmotor.Stop();
    leftmotor.Stop();
}

int main()
{
    LCD.Clear();
    // while (true) {
    //     // bump switch testing
    //     while (!bumpswitch.Value())
    //         ;
    //     LCD.WriteLine("Bump switch released");
    //     while (bumpswitch.Value())
    //         ;
    //     LCD.WriteLine("Bump switch pressed");
    // }

    int motorSpeed = 25;
    int robotWidth = 8;
    int p1Dist = 17; // Around 17 inches between corner of starting area and wall. Value may need to be lowered
    int p2Dist = 36; // 36 inches between the two walls, we have already traveled 17 inches
    int p2DistAdjusted = p2Dist - p1Dist - robotWidth; // 11 inches

    goForward(motorSpeed, p1Dist);
    Sleep(1.0);
    goForward(motorSpeed, p2DistAdjusted);

    // Wait until touch to start the last part
    float x, y;

    LCD.WriteLine("Waiting for touch to start ramp task");
    while (!LCD.Touch(&x, &y))
        ;
    while (LCD.Touch(&x, &y))
        ;
    while (!LCD.Touch(&x, &y))
        ;

    Sleep(1.0);

    // Distance between bottom and top of ramp is 12.374 in, we will need more most likely, change this as we go
    int rampDist = 25;
    goForward(motorSpeed, rampDist);
    Sleep(1.0);
    // Turn around
    turnRight(motorSpeed, 180);
    Sleep(1.0);
    // Go back
    goForward(motorSpeed, rampDist);
}
