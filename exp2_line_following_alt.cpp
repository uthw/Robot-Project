// This one is more complicated and messy than the other one and is less likely to work due to testing for on border (if your optosensors work, it should be fine)

#include "FEHIO.h"
#include <FEHBattery.h>
#include <FEHLCD.h>
#include <FEHMotor.h>
#include <FEHSD.h>
#include <FEHServo.h>
#include <FEHUtility.h>
#include <cmath>

#define RIGHT_ON_LINE .620
#define RIGHT_ON_BORDER .688 // The values for straight and curved are very different
#define LEFT_ON_LINE .775
#define LEFT_ON_BORDER 1.055
#define MIDDLE_ON_LINE 1.45
#define MIDDLE_ON_BORDER 1.46

#define ON_LINE(val, target) (fabs((val - target) / target) < ERROR_THRESHOLD)
#define ON_BORDER(val, target) (fabs((val - target) / target) < BORDER_THRESHOLD)

#define MAX_SPEED 25
#define MIN_SPEED 12
#define LOW_BATTERY 3.0
#define ERROR_THRESHOLD 0.2
#define BORDER_THRESHOLD 0.35
#define DIRECTION_TIME 0.01
#define REQUIRED_CONSISTENT_READINGS 3

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
float relativeError(float observed, float expected);
float relativeError(float observed, float expected, FEHFile& file, float startTime);
void debugSystem(FEHFile* file, float rightVal, float leftVal, float middleVal, float startTime, int currentState);

int main()
{
    float startTime = TimeNow();
    FEHFile* outfile = SD.FOpen("calcs.txt", "w");

    float rightVal = right_opto.Value();
    float middleVal = middle_opto.Value();
    float leftVal = left_opto.Value();

    int consistentReadings = 0;
    int lastState = MIDDLE;

    while (true) {
        rightVal = right_opto.Value();
        middleVal = middle_opto.Value();
        leftVal = left_opto.Value();

        debugSystem(outfile, rightVal, middleVal, leftVal, startTime, lastState);

        // Check line conditions
        bool rightOnLine = ON_LINE(rightVal, RIGHT_ON_LINE);
        bool middleOnLine = ON_LINE(middleVal, MIDDLE_ON_LINE);
        bool leftOnLine = ON_LINE(leftVal, LEFT_ON_LINE);

        bool rightOnBorder = ON_BORDER(rightVal, RIGHT_ON_BORDER);
        bool middleOnBorder = ON_BORDER(middleVal, MIDDLE_ON_BORDER);
        bool leftOnBorder = ON_BORDER(leftVal, LEFT_ON_BORDER);

        int currentState = MIDDLE;

        // Determine motor actions based on sensor combinations
        if (middleOnLine) {
            currentState = MIDDLE;
        } else if (leftOnLine) {
            currentState = LEFT;
        } else if (rightOnLine) {
            currentState = RIGHT;
        } else if (leftOnBorder && rightOnBorder) {
            // Both borders detected
            currentState = MIDDLE;
        } else {
            // No line detected :(
            currentState = MIDDLE;
        }

        if (currentState == lastState) {
            consistentReadings++;
        } else {
            consistentReadings = 0;
        }

        if (consistentReadings >= REQUIRED_CONSISTENT_READINGS) {
            // Only change motor commands after consistent readings
            switch (currentState) {
            case MIDDLE:
                goForward();
                break;
            case RIGHT:
                turnLeft();
                break;
            case LEFT:
                turnRight();
                break;
            }
            lastState = currentState;
        }

        Sleep(DIRECTION_TIME);
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

float relativeError(float observed, float expected)
{
    if (observed == expected || expected == 0) {
        return 0; // avoid div by 0
    }
    return (observed - expected) / expected;
}

float relativeError(float observed, float expected, FEHFile& file, float startTime)
{
    if (observed == expected || expected == 0) {
        SD.FPrintf(&file, "Div by 0\n");
        return 0; // avoid div by 0
    }
    float error = fabs((observed - expected) / expected);

    SD.FPrintf(&file, "Absolute Error: %f\n", error);
    return error;
}

// Update function signature to include state parameter
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

    // Add state display
    LCD.Write("\nState: ");
    switch (currentState) {
    case MIDDLE:
        LCD.WriteLine("MIDDLE");
        break;
    case RIGHT:
        LCD.WriteLine("RIGHT");
        break;
    case LEFT:
        LCD.WriteLine("LEFT");
        break;
    default:
        LCD.WriteLine("UNKNOWN");
        break;
    }

    float voltage = Battery.Voltage();
    LCD.Write("\nBattery: ");
    LCD.WriteLine(voltage);
    if (voltage < LOW_BATTERY) {
        LCD.WriteLine("WARNING: Low Battery!");
    }

    SD.FPrintf(file, "----------------------------------------\n");
    SD.FPrintf(file, "Time: %.2f seconds\n", elapsedTime);
    SD.FPrintf(file, "State: %s\n",
        currentState == MIDDLE ? "MIDDLE" : currentState == RIGHT ? "RIGHT"
            : currentState == LEFT                                ? "LEFT"
                                                                  : "UNKNOWN");
    SD.FPrintf(file, "Sensors (R/M/L): %f/%f/%f\n", rightVal, middleVal, leftVal);
    SD.FPrintf(file, "Battery: %f\n", voltage);
    SD.FPrintf(file, "----------------------------------------\n");

    Sleep(0);
}