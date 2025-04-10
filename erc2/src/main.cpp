#include <Arduino.h>
#include <FEH.h>
#include "motor_helpers.h"
#include "utils.h"
#include "tasks.h"

// FEHServo leverArm(FEHServo::Servo7); // black on top
// FEHMotor servo(FEHMotor::Motor0, 5.0);

void ERCMain()
{
    // calibrateLightSensor(); // Uncomment to calibrate light sensor
    // calibrateOptosensors(); // Uncomment to calibrate optosensors

    int speed = 25;

    RCS.InitializeTouchMenu("0410H5NPE"); // Uncomment for official runs (RCS)
    detectStart();

    compostBin(speed);
    appleBasket(speed);
    levers(speed); // RCS-dependent lever code

    // TODO: add code to go from lever to button to levers

    humidifier_button(speed); // may need to be adjusted for battery. this is located in tasks.cpp
    window(speed);

    // TODO: Go back and press final button

    // Uncomment this loop to run motors with touch screen
    while (true) {
        motorControlGUI();
    } 

    DisplayBatteryPercent();
}