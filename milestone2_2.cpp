#include "FEHIO.h"
#include "motor_helpers.h"
#include <FEHBattery.h>
#include <FEHLCD.h>
#include <FEHMotor.h>
#include <FEHRCS.h>
#include <FEHSD.h>
#include <FEHServo.h>
#include <FEHUtility.h>
#include <algorithm>
#include <cmath>

using namespace std;

// These may be different
DigitalEncoder rightencoder(FEHIO::P0_1);
DigitalEncoder leftencoder(FEHIO::P0_0);
FEHMotor rightmotor(FEHMotor::Motor1, 9.0);
FEHMotor leftmotor(FEHMotor::Motor0, 9.0);

// temporary most likely - just for testing
DigitalInputPin bumpswitch(FEHIO::P0_7);

// CdS cell
AnalogInputPin lightSensor(FEHIO::P1_0);

#define IS_DARK(voltage) (voltage > DARK_THRESHOLD)

#define DARK_THRESHOLD 2.5

int main()
{
    LCD.Clear();
    // wait for start light turn on
    while (IS_DARK(lightSensor.Value())) { }

    // go to the light on the ground
    goForward(50, 30, rightmotor, leftmotor, rightencoder, leftencoder);
    turnLeft(50, 90, rightmotor, leftmotor, rightencoder, leftencoder);
    goForward(50, 17, rightmotor, leftmotor, rightencoder, leftencoder);

    // read cds value
    Sleep(1.0);
    float cdsValue = lightSensor.Value();

    // idr which button is which
    if (IS_DARK(cdsValue)) {
        // blue --> turn left
        turnLeft(50, 90, rightmotor, leftmotor, rightencoder, leftencoder);
        goForward(50, 1, rightmotor, leftmotor, rightencoder, leftencoder);
        turnRight(50, 90, rightmotor, leftmotor, rightencoder, leftencoder);
    } else {
        // red --> turn right
        turnRight(50, 90, rightmotor, leftmotor, rightencoder, leftencoder);
        goForward(50, 1, rightmotor, leftmotor, rightencoder, leftencoder);
        turnLeft(50, 90, rightmotor, leftmotor, rightencoder, leftencoder);
    }

    goForward(50, 8, rightmotor, leftmotor, rightencoder, leftencoder);
}
