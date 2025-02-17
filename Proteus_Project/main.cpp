#include "FEHIO.h"
#include <FEHUtility.h>
#include <FEHServo.h>
#include <FEHMotor.h>

enum LineStates {
    MIDDLE,
    RIGHT,
    LEFT
};

#define RIGHT_ON_LINE 2.407
#define LEFT_ON_LINE 0.942

// Declarations for analog optosensors
AnalogInputPin right_opto(FEHIO::P1_0);
AnalogInputPin middle_opto(FEHIO::P2_1);
AnalogInputPin left_opto(FEHIO::P2_2);

FEHMotor rightmotor(FEHMotor::Motor0,9.0);
FEHMotor leftmotor(FEHMotor::Motor1,9.0);
void setStraightDrive();
void turnRight();
void turnLeft();
void goForward();
float relativeError(float observed, float expected);

int main() {
    int state = MIDDLE; // Set the initial state
    while (true) { // I will follow this line forever!
        switch(state) {
            // If I am in the middle of the line...
            case MIDDLE:
                // Set motor powers for driving straight
                /* Drive */
                if (relativeError(right_opto.Value(), RIGHT_ON_LINE) < 0.1) {
                    state = RIGHT; // update a new state
                } else if (relativeError(left_opto.Value(), LEFT_ON_LINE) < 0.1) {
                    state = LEFT; // update a new state
                }
                goForward();
                break;
            // If the right sensor is on the line...
            case RIGHT:
                // Set motor powers for right turn
                turnRight();
                goForward();
                if (relativeError(middle_opto.Value(), LEFT_ON_LINE) < 0.1) {
                    state = MIDDLE; // update a new state
                } else if (relativeError(left_opto.Value(), LEFT_ON_LINE) < 0.1) {
                    state = LEFT; // update a new state
                }
                break;
            // If the left sensor is on the line...
            case LEFT:
                // Set motor powers for left turn
                turnLeft();
                goForward();
                if (relativeError(middle_opto.Value(), RIGHT_ON_LINE) < 0.1) {
                    state = MIDDLE; // update a new state
                } else if (relativeError(right_opto.Value(), RIGHT_ON_LINE) < 0.1) {
                    state = RIGHT; // update a new state
                }
                break;
            }
    }
}

void setStraightDrive()
{
}

void turnRight()
{
    rightmotor.SetPercent(10);
    leftmotor.SetPercent(20);
}

void turnLeft()
{
    rightmotor.SetPercent(20);
    leftmotor.SetPercent(10);
}

void goForward()
{
    rightmotor.SetPercent(20);
    leftmotor.SetPercent(20);
}

float relativeError(float observed, float expected)
{
    if (observed == expected || expected == 0) {
        return 0; // avoid div by 0
    }
    return (observed - expected) / expected;
}
