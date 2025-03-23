
#ifndef MOTOR_HELPERS_H
#define MOTOR_HELPERS_H

#include "FEHIO.h"
#include "FEHMotor.h"

// extern DigitalEncoder rightencoder;
// extern DigitalEncoder leftencoder;
// extern FEHMotor rightmotor;
// extern FEHMotor leftmotor;

void turnRight(int percent, int degrees, DigitalEncoder leftencoder, DigitalEncoder rightencoder, FEHMotor leftmotor, FEHMotor rightmotor);
void turnLeft(int percent, int degrees, DigitalEncoder leftencoder, DigitalEncoder rightencoder, FEHMotor leftmotor, FEHMotor rightmotor);
void goForward(int percent, int inches, DigitalEncoder leftencoder, DigitalEncoder rightencoder, FEHMotor leftmotor, FEHMotor rightmotor);
void goForwardHalfInches(int percent, int halfInches, DigitalEncoder leftencoder, DigitalEncoder rightencoder, FEHMotor leftmotor, FEHMotor rightmotor);

#endif