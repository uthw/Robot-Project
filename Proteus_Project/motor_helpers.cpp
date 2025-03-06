#include "FEHLCD.h"
#include "FEHIO.h"
#include "FEHMotor.h"

// Convert desired degrees to number of counts required to rotate said number of degrees
#define DEGREES_TO_COUNTS(degrees) (int)(degrees * COUNTS_IN_90_DEGREES / 90)
#define INCHES_TO_COUNTS(inches) (int)(inches * COUNTS_IN_1_INCH)

// Motors have less power as battery decreases
#define ACTUAL_PERCENTAGE_POWER(percent) min((int)((11.5 / Battery.Voltage()) * percent), 100)

#define COUNTS_IN_90_DEGREES 210
#define COUNTS_IN_1_INCH 20

#define LEFT_MODIFIER 0 // Added to left motor speed. Set to 0 if it's going straight right now

void turnRight(int percent, int degrees, FEHMotor leftmotor, FEHMotor rightmotor, DigitalEncoder leftencoder, DigitalEncoder rightencoder)
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

void turnLeft(int percent, int degrees, FEHMotor leftmotor, FEHMotor rightmotor, DigitalEncoder leftencoder, DigitalEncoder rightencoder)
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
void goForward(int percent, int inches, FEHMotor leftmotor, FEHMotor rightmotor, DigitalEncoder leftencoder, DigitalEncoder rightencoder)
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