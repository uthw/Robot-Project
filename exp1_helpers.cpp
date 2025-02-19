
#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHServo.h>
#include <FEHAccel.h>
#include <FEHBattery.h>
#include <FEHBuzzer.h>
// #include <FEHRCS.h>
#include <FEHSD.h>
#include <string.h>
#include <stdio.h>


FEHMotor leftMotor(FEHMotor::Motor0, 9.0);
FEHMotor rightMotor(FEHMotor::Motor1, 9.0);


DigitalInputPin frontLeftBump(FEHIO::P3_1);
DigitalInputPin frontRightBump(FEHIO::P0_1);
DigitalInputPin backLeftBump(FEHIO::P3_0);
DigitalInputPin backRightBump(FEHIO::P0_0);

AnalogInputPin lightSensor(FEHIO::P1_0);


FEHServo servo(FEHServo::Servo7);

float voltageToDegrees(float voltage);
void setAngleBasedOnLight(AnalogInputPin cdsSensor, FEHServo servo);

#define DEGREES_PER_VOLT 180 / 3.3

int main (void) {
    servo.SetMin(500);
    servo.SetMax(2383); // Calibrated max

    while (true) {
        setAngleBasedOnLight(lightSensor, servo);
    }
}

void setAngleBasedOnLight(AnalogInputPin cdsSensor, FEHServo servo) {
    float voltage = cdsSensor.Value();
    float angle = voltageToDegrees(voltage);
    servo.SetDegree(angle);
}

// Expects a value between 0 and 3.3
float voltageToDegrees(float voltage) {
    return voltage * DEGREES_PER_VOLT;
}

// 0 V when fully lit, 3.3 V when no light
void printLightValue(AnalogInputPin sensor) {
    LCD.Clear();
    LCD.SetFontColor(FEHLCD::Blue);
    LCD.Write("Light Output: ");
    LCD.WriteLine(sensor.Value());
}