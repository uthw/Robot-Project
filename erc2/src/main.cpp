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
    // calibrateLightSensor();
    while (true) {
        motorControlGUI();
    }
    
    // RCS.InitializeTouchMenu();
    detectStart();

    DisplayBatteryPercent();
}