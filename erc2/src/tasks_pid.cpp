
#include "motor_helpers.h"
#include <FEH.h>
#include "utils.h"
#include "tasks.h"

#define WINDOW_TIMEOUT 5

// Travels from start button to compost bin and rotates it
void compostBinPID(int speed)
{
    setLeverArmDegree(50);
    // goForward(speed, 2);
    turnLeftPID(speed, 45);
    goForward(speed, 7);
    turnLeftPID(speed, 4);
    goForward(speed, 2.85);
    // waitForTouch("touch to turn composter");
    turnComposter(100, 2); // Turn composter
    turnComposter(-100, 2); // Turn composter back
}

// Apple bucket m4 task, but all turns use PID
void appleBucketPID(int speed)
{
    // Navigate to the stump
    goBackward(speed, 3);
    turnRightPID(speed, 102);
    goBackward(speed, 10);
    goForward(speed, 21.25);
    turnLeftPID(speed, 85); // Now facing the stump

    setLeverArmDegree(120);

    goForward(speed, 10, MOTOR_DOWNTIME, 3.0);
    // waitForTouch("Touch to pick up apples");
    setLeverArmDegree(90);

    goBackward(speed, 3);
    turnRightPID(speed, 20);
    goBackward(speed, 27);
    goForward(25, 4);
    turnRightPID(35, 94);
    goForward(35, 26);
    turnLeftPID(25, 46);
    goForward(25, 9); // Going Diagonal
    turnRightPID(25, 46);
    goForward(25, 15.5, 0.2, 4.0);
    // drop bucket off
    setLeverArmDegree(130);
    Sleep(0.2);
}

void leverAAltPID(int speed)
{
    goForward(speed, 9.25); // Goes across blue lines on ground
    turnRightPID(speed, 92); // Facing lever (up from 92)
    waitForTouch("touch to pull lever");
    goForward(speed, 6.5);
    setLeverArmDegreeInstant(160); // Lower the lever
    Sleep(1.0);
    // if (RCS.isLeverFlipped() != 0) { // Try again if failed
    //     goBackward(speed, 4);
    //     setLeverArmDegree(90); // Raise
    //     turnRight(speed, 5);
    //     goForward(speed, 6.5);
    //     setLeverArmDegreeInstant(160); // Lower the leer
    //     Sleep(1.0);
    // }
    goBackward(speed, 4);
    setLeverArmDegree(170);
    Sleep(5.0); // Wait 5 seconds for extra points
    goForward(speed, 4.5);
    setLeverArmDegreeInstant(110); // Raise lever back up
    Sleep(1.0);
    goBackward(speed, 8.5);
    turnLeftPID(speed, 45);
}

void leversPID(int speed)
{
    goBackward(speed, 30);
    setLeverArmDegree(90);
    turnLeftPID(speed, 135);
    leverAAltPID(speed);
}

void windowPID(int speed)
{
    // Robot moves backwards to align with window, opens it, then finishes

    // waitForTouch("touch to open window");

    goBackward(30, 30); // Go back into the plexiglass

    float startTime = TimeNow();
    goForward(speed * 0.8);
    // Goes forward until window is open or 15 seconds have passed
    while (RCS.isWindowOpen() == 0 && TimeNow() - startTime < WINDOW_TIMEOUT) {
        // Sleep(0);
        LCD.Clear();
        LCD.Write("isWindowOpen: ");
        LCD.WriteLine(RCS.isWindowOpen());
        LCD.Write("Elapsed Time: ");
        LCD.WriteLine(TimeNow() - startTime);
        Sleep(0.1); // Add a small delay to avoid excessive polling
    }
    stopMotors();

    // waitForTouch("touch for final button");

    goBackward(speed, 30); // Go back into plexiglass again
    goForward(speed, 5);
    turnRightPID(speed, 90); // Turn towards ending button
    goBackward(speed, 45); // Finish
}

void runPID()
{
    int speed = 35;

    RCS.InitializeTouchMenu("0410H5NPE"); // Uncomment for official runs (RCS)
    detectStartDebug();

    // start button
    goBackward(25, 2);
    Sleep(0.5);
    goForward(25, 1.5);

    // goBackwardsAndReturn(25, 0.3);

    compostBinPID(speed);
    appleBucketPID(speed);

    waitForTouch("touch for levers");

    leversPID(speed); // RCS-dependent lever code

    waitForTouch("touch for humidifier");

    humidifier_button(speed); // this one also does window and final button

    window(speed); // RCS Independent
}