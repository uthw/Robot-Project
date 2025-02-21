#include "FEHIO.h"
#include <FEHBattery.h>
#include <FEHLCD.h>
#include <FEHMotor.h>
#include <FEHSD.h>
#include <FEHServo.h>
#include <FEHUtility.h>
#include <cmath>

#define RIGHT_ON_LINE 2.425
#define RIGHT_ON_BORDER 1.75
#define LEFT_ON_LINE 1.1
#define LEFT_ON_BORDER 0.87
#define MIDDLE_ON_LINE 1.5
#define MIDDLE_ON_BORDER 1.35

#define ON_LINE(val, target) (fabs((val - target) / target) < ERROR_THRESHOLD)
#define ON_BORDER(val, target) (fabs((val - target) / target) < BORDER_THRESHOLD)
#define ON_LINE_CONSTANT_THRESHOLD(val, target) (fabs(val - target) < TOLERANCE)

#define MAX_SPEED 20
#define MIN_SPEED 10
#define LOW_BATTERY 3.0
#define ERROR_THRESHOLD 0.08
#define BORDER_THRESHOLD 0.12
#define TOLERANCE 0.25
#define DIRECTION_TIME 0.01
#define REQUIRED_CONSISTENT_READINGS 3
#define LOWEST_INCONSISTENCY 0.4

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
void checkSensorConsistency(float newValue, float oldValue, char *sensorName, FEHFile* outfile);

int main()
{
    float startTime = TimeNow();
    FEHFile* outfile = SD.FOpen("calcs.txt", "w");

    float rightVal = right_opto.Value();
    float middleVal = middle_opto.Value();
    float leftVal = left_opto.Value();

    int lastState = -1;

    int rightValOld = rightVal;
    int middleValOld = middleVal;
    int leftValOld = leftVal;

    while (true) {
        rightVal = right_opto.Value();
        middleVal = middle_opto.Value();
        leftVal = left_opto.Value();

        debugSystem(outfile, rightVal, middleVal, leftVal, startTime, lastState);

        rightValOld = rightVal;
        middleValOld = middleVal;
        leftValOld = leftVal;
        Sleep(0); // Could be commented out

        // Check line conditions
        bool rightOnLine = ON_LINE_CONSTANT_THRESHOLD(rightVal, RIGHT_ON_LINE);
        bool middleOnLine = ON_LINE_CONSTANT_THRESHOLD(middleVal, MIDDLE_ON_LINE);
        bool leftOnLine = ON_LINE_CONSTANT_THRESHOLD(leftVal, LEFT_ON_LINE);

        bool rightOnBorder = ON_BORDER(rightVal, RIGHT_ON_BORDER);
        bool middleOnBorder = ON_BORDER(middleVal, MIDDLE_ON_BORDER);
        bool leftOnBorder = ON_BORDER(leftVal, LEFT_ON_BORDER);

        // Check for sensor consistency
        checkSensorConsistency(rightVal, rightValOld, "right", outfile);
        checkSensorConsistency(middleVal, middleValOld, "middle", outfile);
        checkSensorConsistency(leftVal, leftValOld, "left", outfile);

        // // Determine motor actions based on sensor combinations
        // if (middleOnLine) {
        //     // currentState = MIDDLE;
        //     goForward();
        // } else if (leftOnLine || (leftOnBorder && !rightOnBorder)) {
        //     // currentState = LEFT;
        //     turnRight();
        // } else if (rightOnLine || (rightOnBorder && !leftOnBorder)) {
        //     // currentState = RIGHT;
        //     turnLeft();
        // } else if (leftOnBorder && rightOnBorder) {
        //     // Both borders detected
        //     // currentState = MIDDLE;
        //     goForward();
        // } else {
        //     // No line detected :(
        //     // currentState = MIDDLE;
        //     goForward();
        // }

        if (leftOnLine) {
            turnRight();
        } else if (rightOnLine) {
            turnLeft();
        } else {
            goForward();
        }

        // Sleep(DIRECTION_TIME);
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

void checkSensorConsistency(float newValue, float oldValue, char *sensorName, FEHFile* outfile) {
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

    // // State display not needed while the state machine is not used
    // LCD.Write("\nState: ");
    // switch (currentState) {
    // case MIDDLE:
    //     LCD.WriteLine("MIDDLE");
    //     break;
    // case RIGHT:
    //     LCD.WriteLine("RIGHT");
    //     break;
    // case LEFT:
    //     LCD.WriteLine("LEFT");
    //     break;
    // default:
    //     LCD.WriteLine("UNKNOWN");
    //     break;
    // }

    float voltage = Battery.Voltage();
    // LCD.Write("\nBattery: ");
    // LCD.WriteLine(voltage);
    if (voltage < LOW_BATTERY) {
        LCD.WriteLine("WARNING: Low Battery!");
    }

    SD.FPrintf(file, "Time: %.2f seconds\n", elapsedTime);
    // SD.FPrintf(file, "State: %s\n",
        // currentState == MIDDLE ? "MIDDLE" : currentState == RIGHT ? "RIGHT"
        //     : currentState == LEFT                                ? "LEFT"
        //                                                           : "UNKNOWN");
    SD.FPrintf(file, "Sensors (R/M/L): %f/%f/%f\n", rightVal, middleVal, leftVal);
    // SD.FPrintf(file, "Battery: %f\n", voltage);
    SD.FPrintf(file, "----------------------------------------\n");
}