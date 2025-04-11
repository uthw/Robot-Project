#include "motor_helpers.h"
#include "tasks.h"
#include "utils.h"
#include <Arduino.h>
#include <FEH.h>

// FEHServo leverArm(FEHServo::Servo7); // black on top
// FEHMotor servo(FEHMotor::Motor0, 5.0);

void ERCMain()
{
    // // Uncomment this loop to run motors with touch screen
    // while (true) {
    //     motorControlGUI();
    // }

    // while (true) {
    DisplayBatteryPercent();
    Sleep(0.5);
    // }

    // calibrateLightSensor(); // Uncomment to calibrate light sensor
    // calibrateOptosensors(); // Uncomment to calibrate optosensors

    int speed = 35;

    RCS.InitializeTouchMenu("0410H5NPE"); // Uncomment for official runs (RCS)
    detectStart();

    // start button
    goBackward(25, 2);
    Sleep(0.5);
    goForward(25, 3);

    // goBackwardsAndReturn(25);

    compostBin(speed);
    appleBasket(speed);

    // waitForTouch("touch for levers");

    levers(speed); // RCS-dependent lever code

    // TODO: add code to go from lever to button to levers

    // waitForTouch("touch for humidifier");

    humidifier_button(speed); // this one also does window and final button

    // LCD.Clear();
    // LCD.WriteLine("touch for window");
    // LCD.WaitForTouchToStart();
    // Sleep(TOUCH_BUFFER);
    // window(speed);
}