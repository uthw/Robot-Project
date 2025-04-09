#ifndef MOTOR_HELPERS_H
#define MOTOR_HELPERS_H

#include <FEHIO.h>
#include <FEHLCD.h>
#include <FEHMotor.h>

#define LEVER_ARM_DEFAULT 120 // default (extending out towards robot) may need to be updated over time

void turnRight(int percent, int degrees);
void turnLeft(int percent, int degrees);
void goForward(int percent, float inches);
void goForwardTimed(int percent, float seconds);
void goForward(int percent);
void goForward(int percent, float inches, float downtime);
void stopMotors();
int getValueTouch(char *message, int min, int max, int increment, int initialValue);
void motorControlGUI();
void setLeverArmDegree(int degree);
void setLeverArmDegreeInstant(int degree);
void goBackward(int percent, float inches);
void goBackward(int percent);
void calibrateOptosensors();
float getOptoReading(AnalogInputPin& opto, int samples);
void turnComposter(int percent);
void stopComposter();
void turnComposter(int percent, float seconds);

#endif