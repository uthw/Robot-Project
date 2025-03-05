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



#define DARK_THRESHOLD 2.5
#define IS_DARK(voltage) (voltage > DARK_THRESHOLD)

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
    while(IS_DARK(lightSensor.Value())){}
}
