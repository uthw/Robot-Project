#ifndef MOTOR_HELPERS_H
#define MOTOR_HELPERS_H

#include <FEHIO.h>
#include <FEHLCD.h>
#include <FEHMotor.h>

void turnRight(int percent, int degrees);
void turnLeft(int percent, int degrees);
void goForward(int percent, float inches);
void goForwardTimed(int percent, float seconds);
void goForward(int percent);
void stopMotors();
int getValueTouch(char *message, int min, int max, int increment, int initialValue);
void MotorControlGUI();

#endif