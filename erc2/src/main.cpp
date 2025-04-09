#include <Arduino.h>
#include <FEH.h>
#include <FEHRCS.h>
#include "motor_helpers.h"
#include "utils.h"
#include "tasks.h"

// FEHServo leverArm(FEHServo::Servo7); // black on top
// FEHMotor servo(FEHMotor::Motor0, 5.0);

void ERCMain()
{
    // calibrateLightSensor(); // Uncomment to calibrate light sensor
    // calibrateOptosensors(); // Uncomment to calibrate optosensors

    // RCS.InitializeTouchMenu("0410H5NPE"); // Uncomment for official runs (RCS)
    detectStart();

    // Compost bin
    goBackward(25, 2);
    turnRight(25, 45);
    goForward(25, 6);
    turnComposter(75, 5); // Turn composter
    turnComposter(-75, 5); // Turn composter back

    // Go to apples
    turnRight(25, 45);
    goForward(25, 11);
    setLeverArmDegree(130);
    goForward(25, 7);

    // Pick up apples
    setLeverArmDegree(90);

    // Go to basket
    goBackward(25, 4);
    turnRight(25, 45);
    goBackward(25, 4);
    turnLeft(25, 45);
    goForward(-25, 20);
    goForward(25, 4);
    turnRight(25, 90);
    goForward(35, 25);
    turnLeft(25, 45);
    goForward(25, 10);
    turnRight(25, 47);
    goForward(25, 15.5);

    // Deposit apples
    setLeverArmDegree(140);

    // Lever
    goBackward(25, 4);
    setLeverArmDegree(90);
    turnLeft(25, 45);

    // This part will need to be replaced with the RCS-dependent code
    goForward(25, 8);
    setLeverArmDegree(160); // Lower the lever
    goBackward(25, 5);
    setLeverArmDegree(165); // Lower position to raise after - was originally
    Sleep(5.0); // Wait 5 seconds for extra points
    turnLeft(25, 5); // Realign to go straight into the lever
    goForward(25, 5);
    setLeverArmDegree(110); // Raise lever back up

    // Need to add code to go from lever to button

    // Go to button light and read
    goForwardUntilLight(25);
    Sleep(0.75);

    // Push correct light
    humidifier_button(); // may need to be adjusted for battery. this is located in tasks.cpp

    // Align with wall near ramp - this part uses dummy values
    goBackward(25, 40);
    turnLeft(25, 90);
    goForward(25, 6);
    turnRight(25, 90);

    // Facing window now
    goForward(40, 23); // Open window. Uses old value from milestone 3 code

    // Close window

    // Maneuver around the window handle
    goBackward(40, 2);
    turnRight(25, 15);
    goForward(40, 2);
    turnLeft(25, 10);
    goForward(40, 3); // Up against flowers now

    // Collide with window
    goBackward(40, 20);

    // Go back and press final button

    // NYI

    while (true) {
        motorControlGUI(); // Uncomment to control motors with screen
    }

    DisplayBatteryPercent();
}