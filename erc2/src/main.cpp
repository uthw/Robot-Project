#include "motor_helpers.h"
#include "tasks.h"
#include "utils.h"
#include <Arduino.h>
#include <FEH.h>

// FEHServo leverArm(FEHServo::Servo7); // black on top
// FEHMotor servo(FEHMotor::Motor0, 5.0);

void ERCMain()
{
    DisplayBatteryPercent();
    Sleep(0.5);

    // // Uncomment this loop to run motors with touch screen
    // while (true) {
    //     motorControlGUI();
    //     // taskGUI();
    // }

    // while (true) {
    // }

    // calibrateLightSensor(); // Uncomment to calibrate light sensor
    // calibrateOptosensors(); // Uncomment to calibrate optosensors

    int speed = 35;

    while (true) {
        mainMenuGUI();
    }

    // LCD.Clear();
    // LCD.WriteLine("touch for window");
    // LCD.WaitForTouchToStart();
    // Sleep(TOUCH_BUFFER);
    // window(speed);

    while (true) {
        // motorControlGUI();
        taskGUI();
    }
}