#include <Arduino.h>
#include <FEH.h>
#include <FEHRCS.h>
// #include "motor_helpers.h"
#include "utils.h"

void ERCMain()
{
    Serial.begin(9600);
    Serial.print("HELLO WORLD!");
    while (true) {
        DisplayBatteryPercent();
        Sleep(0.2);
    }
}