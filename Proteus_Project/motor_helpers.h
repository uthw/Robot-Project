
#ifndef MOTOR_HELPERS_H
#define MOTOR_HELPERS_H

#include "FEHIO.h"
#include "FEHMotor.h"

void turnRight(int percent, int degrees, FEHMotor leftmotor, FEHMotor rightmotor, DigitalEncoder leftencoder, DigitalEncoder rightencoder);
void turnLeft(int percent, int degrees, FEHMotor leftmotor, FEHMotor rightmotor, DigitalEncoder leftencoder, DigitalEncoder rightencoder);
void goForward(int percent, int inches, FEHMotor leftmotor, FEHMotor rightmotor, DigitalEncoder leftencoder, DigitalEncoder rightencoder);

#endif