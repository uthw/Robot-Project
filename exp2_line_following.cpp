
// This file has a lot of redundancies and bloat, I can make a simpler version if it's needed

#include "FEHIO.h"
#include <FEHBattery.h>
#include <FEHLCD.h>
#include <FEHMotor.h>
#include <FEHSD.h>
#include <FEHServo.h>
#include <FEHUtility.h>
#include <cmath>

#define RIGHT_ON_LINE 2.7
#define RIGHT_ON_BORDER 1.75
#define LEFT_ON_LINE 0.936
#define LEFT_ON_BORDER 0.87
#define MIDDLE_ON_LINE 1.5
#define MIDDLE_ON_BORDER 1.35

#define ON_LINE_RELATIVE(val, target) (fabs((val - target) / target) < ERROR_THRESHOLD)
#define ON_BORDER(val, target) (fabs((val - target) / target) < BORDER_THRESHOLD)
#define ON_LINE_CONSTANT_THRESHOLD(val, target, tolerance) (fabs(val - target) < tolerance)
// #define ON_LINE_CONSTANT_THRESHOLD_2(val, target) (val + TOLERANCE < target)

#define MAX_SPEED 20
#define MIN_SPEED 10
#define LOW_BATTERY 3.0
#define ERROR_THRESHOLD 0.08
#define BORDER_THRESHOLD 0.12
#define TOLERANCE_RIGHT 0.2
#define TOLERANCE_LEFT 0.12
#define DIRECTION_TIME 0.01
#define REQUIRED_CONSISTENT_READINGS 3
#define LOWEST_INCONSISTENCY 0.05

enum State {
    MIDDLE,
    RIGHT,
    LEFT
};

// Declarations for analog optosensors
AnalogInputPin right_opto(FEHIO::P1_0);
AnalogInputPin middle_opto(FEHIO::P2_1);
AnalogInputPin left_opto(FEHIO::P2_2);

FEHMotor rightmotor(FEHMotor::Motor0, 9.0);
FEHMotor leftmotor(FEHMotor::Motor1, 9.0);
void turnRight();
void turnLeft();
void goForward();
void debugSystem(FEHFile* file, float rightVal, float leftVal, float middleVal, float startTime, int currentState);
void checkSensorConsistency(float newValue, float oldValue, char* sensorName, FEHFile* outfile);

int main()
{
    float startTime = TimeNow();
    FEHFile* outfile = SD.FOpen("calcs.txt", "w");

    float rightVal = right_opto.Value();
    float middleVal = middle_opto.Value();
    float leftVal = left_opto.Value();

    int lastState = -1;

    float rightValOld = rightVal;
    float middleValOld = middleVal;
    float leftValOld = leftVal;

    while (true) {
        rightVal = right_opto.Value();
        middleVal = middle_opto.Value();
        leftVal = left_opto.Value();

        debugSystem(outfile, rightVal, middleVal, leftVal, startTime, lastState);

        // Check for sensor consistency
        checkSensorConsistency(rightVal, rightValOld, "right", outfile);
        checkSensorConsistency(middleVal, middleValOld, "middle", outfile);
        checkSensorConsistency(leftVal, leftValOld, "left", outfile);

        rightValOld = rightVal;
        middleValOld = middleVal;
        leftValOld = leftVal;
        // Sleep(0); // Could be commented out

        // Check line conditions
        bool rightOnLine = ON_LINE_CONSTANT_THRESHOLD(rightVal, RIGHT_ON_LINE, TOLERANCE_RIGHT);
        bool leftOnLine = ON_LINE_CONSTANT_THRESHOLD(leftVal, LEFT_ON_LINE, TOLERANCE_LEFT);

        bool rightOnBorder = ON_BORDER(rightVal, RIGHT_ON_BORDER);
        bool middleOnBorder = ON_BORDER(middleVal, MIDDLE_ON_BORDER);
        bool leftOnBorder = ON_BORDER(leftVal, LEFT_ON_BORDER);

        if (rightOnLine) {
            turnLeft();
        } else if (leftOnLine) {
            turnRight();
        } else {
            goForward();
        }
    }
}

void turnRight()
{
    rightmotor.SetPercent(MIN_SPEED);
    leftmotor.SetPercent(MAX_SPEED);
}

void turnLeft()
{
    rightmotor.SetPercent(MAX_SPEED);
    leftmotor.SetPercent(MIN_SPEED);
}

void goForward()
{
    rightmotor.SetPercent(MAX_SPEED);
    leftmotor.SetPercent(MAX_SPEED);
}

// ~10% or less inconsistent = ideal
void checkSensorConsistency(float newValue, float oldValue, char* sensorName, FEHFile* outfile)
{
    if (fabs(newValue - oldValue) > LOWEST_INCONSISTENCY) {
        SD.FPrintf(outfile, "Inconsistent %s reading: %f compared to %f\n", sensorName, newValue, oldValue);
    }
}

void debugSystem(FEHFile* file, float rightVal, float leftVal, float middleVal, float startTime, int currentState)
{
    float elapsedTime = TimeNow() - startTime;

    LCD.Clear();

    LCD.WriteLine("\nSensor Values:");
    LCD.Write("Right: ");
    LCD.WriteLine(rightVal);
    LCD.Write("Middle: ");
    LCD.WriteLine(middleVal);
    LCD.Write("Left: ");
    LCD.WriteLine(leftVal);

    float voltage = Battery.Voltage();
    if (voltage < LOW_BATTERY) {
        LCD.WriteLine("WARNING: Low Battery!");
    }

    SD.FPrintf(file, "Time: %.2f seconds\n", elapsedTime);
    SD.FPrintf(file, "Sensors (R/M/L): %f/%f/%f\n", rightVal, middleVal, leftVal);
    SD.FPrintf(file, "----------------------------------------\n");
}