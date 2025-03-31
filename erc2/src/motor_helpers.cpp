using namespace std;
#include <FEH.h>
#include <FEHIO.h>
#include <FEHMotor.h>
// Cmath and algorithm have been killed by ERC2
#include "utils.h"
#include <math.h>

DigitalEncoder rightEncoder(FEHIO::Pin13);
DigitalEncoder leftEncoder(FEHIO::Pin14);
FEHMotor rightMotor(FEHMotor::Motor0, 9.0);
FEHMotor leftMotor(FEHMotor::Motor1, 9.0);

FEHServo leverArm2(FEHServo::Servo7); // black on top

// Convert desired degrees to number of counts required to rotate said number of degrees
#define DEGREES_TO_COUNTS(degrees) (int)(degrees * COUNTS_IN_90_DEGREES / 90)
#define INCHES_TO_COUNTS(inches) (int)(inches * COUNTS_IN_1_INCH)
#define HALF_INCHES_TO_COUNTS(halfInches) (int)(halfInches * COUNTS_IN_1_INCH / 2)

// Motors have less power as battery decreases
#define ACTUAL_PERCENTAGE_POWER(percent) min((int)((MAX_VOLTAGE / Battery.Voltage()) * percent), 100)

#define COUNTS_IN_90_DEGREES 223 // Old: 215
#define COUNTS_IN_1_INCH 32 // Old: 32

#define LEFT_MODIFIER 1 // Added to left motor speed. Set to 0 if it's going straight right now

#define MOTOR_DOWNTIME 0.2 // Time in seconds motors stop after a drivetrain method is called

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
    while ((leftEncoder.Counts() + rightEncoder.Counts()) / 2.0 < counts) {
        Sleep(0);
    }

    // Turn off motors
    rightMotor.Stop();
    leftMotor.Stop();
    Sleep(MOTOR_DOWNTIME);
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
    while ((leftEncoder.Counts() + rightEncoder.Counts()) / 2.0 < counts) {
        Sleep(0);
    }

    // Turn off motors
    rightMotor.Stop();
    leftMotor.Stop();
    Sleep(MOTOR_DOWNTIME);
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
    while ((leftEncoder.Counts() + rightEncoder.Counts()) / 2.0 < counts) {
        // For some reason empty while loops don't work on arduino. This is a workaround
        Sleep(0);
    }

    // Turn off motors
    rightMotor.Stop();
    leftMotor.Stop();
    Sleep(MOTOR_DOWNTIME);
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
    Sleep(MOTOR_DOWNTIME);
}

void goForward(int percent)
{
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);

    rightMotor.SetPercent(actualPercent);
    leftMotor.SetPercent(actualPercent + LEFT_MODIFIER);
}

void stopMotors()
{
    rightMotor.Stop();
    leftMotor.Stop();
    Sleep(MOTOR_DOWNTIME);
}

int getValueTouch(const char* message, int min, int max, int increment, int initialValue)
{
    LCD.Clear();
    LCD.SetFontColor(WHITE);
    LCD.DrawVerticalLine(XMAX / 2, 20, YMAX * 0.8); // goes all the way down
    LCD.DrawHorizontalLine(YMAX * 0.8, 0, XMAX); // goes all the way across

    LCD.WriteLine(message);

    LCD.WriteAt("Down", XMAX / 4, YMAX * 0.8 - 20);
    LCD.WriteAt("Up", XMAX / 2 + 10, YMAX * 0.8 - 20);
    LCD.WriteAt("Confirm", XMAX / 2 + 10, YMAX * 0.8 + 20);

    int value = initialValue;
    while (true) {
        // Redraw old value
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(XMAX / 4 - 20, YMAX * 0.8 + 10, 60, 20);
        LCD.SetFontColor(WHITE);
        LCD.WriteAt(value, XMAX / 4 - 20, YMAX * 0.8 + 10);

        // Wait for touch
        LCD.WaitForTouchToStart();
        // Check where the touch was
        int x, y;
        LCD.Touch(&x, &y);
        Sleep(TOUCH_BUFFER);

        if (x < XMAX / 2 && y < YMAX * 0.8) {
            // Left region, decrease value
            value -= increment;
            if (value < min) {
                value = min;
            }
        } else if (x >= XMAX / 2 && y < YMAX * 0.8) {
            // Right region, increase value
            value += increment;
            if (value > max) {
                value = max;
            }
        } else if (y >= YMAX * 0.8) {
            // Bottom region, return value
            LCD.Clear();
            LCD.WriteAt("Confirmed value: ", 0, 0);
            LCD.WriteLine(value);
            Sleep(0.25);
            return value;
        }
    }
}

// GUI for manually controlling motors
void MotorControlGUI()
{
    LCD.Clear();
    LCD.SetFontColor(WHITE);
    LCD.WriteLine("Touch a region:");
    LCD.WriteAt("Left", XMAX / 4, YMAX / 4); // left region
    LCD.WriteAt("Right", XMAX / 2 + 10, YMAX / 4); // right region
    LCD.WriteAt("Forward", XMAX / 4 - 20, YMAX / 2 + 10); // bottom left(?) region
    LCD.WriteAt("Servo", XMAX / 2 + 10, YMAX / 2 + 10); // bottom right(?) region

    int x, y;

    LCD.DrawVerticalLine(XMAX / 2, 20, YMAX); // goes halfway down
    LCD.DrawHorizontalLine(YMAX / 2, 0, XMAX); // goes all the way across

    // Wait for touch
    while (!LCD.Touch(&x, &y)) {
        Sleep(0.1);
    }

    int motorPower = 25;

    // Determine which region was touched
    if (x < XMAX / 2 && y < YMAX / 2) { // Left
        int deg = getValueTouch("Set degrees to turn left", 0, 360, 15, 90);
        turnLeft(motorPower, deg);
    } else if (x >= XMAX / 2 && y < YMAX / 2) { // Right
        int deg = getValueTouch("Set degrees to turn right", 0, 360, 15, 90);
        turnRight(motorPower, deg);
    } else if (x < XMAX / 2 && y >= YMAX / 2) { // Forward
        int inches = getValueTouch("Set inches to go forward", -60, 60, 1, 6);

        if (inches < 0) {
            inches *= -1;
            motorPower *= -1;
        }
        goForward(motorPower, inches);
    } else if (x >= XMAX / 2 && y >= YMAX / 2) { // Servo
        int angle = getValueTouch("Set servo angle", 0, 180, 5, 90);
        leverArm2.SetMin(500);
        leverArm2.SetMax(2500);
        leverArm2.SetDegree(angle);
    }
}