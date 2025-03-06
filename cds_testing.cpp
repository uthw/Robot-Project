#include "FEHIO.h"
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

#define LOWEST_INCONSISTENCY 0.05

// These may be different
DigitalEncoder rightencoder(FEHIO::P0_1);
DigitalEncoder leftencoder(FEHIO::P0_0);
FEHMotor rightmotor(FEHMotor::Motor1, 9.0);
FEHMotor leftmotor(FEHMotor::Motor0, 9.0);

// temporary most likely - just for testing
DigitalInputPin bumpswitch(FEHIO::P0_7);

// CdS cell
AnalogInputPin lightSensor(FEHIO::P1_0);

int consistentCount = 0;
int inconsistentCount = 0;
float consistencyProportion = 0.0;

// ~10% or less inconsistent = ideal
void checkSensorConsistency(float newValue, float oldValue, char* sensorName, FEHFile* outfile)
{
    if (oldValue == 0) {
        SD.FPrintf(outfile, "Skipping consistency check since old value is 0\n");
    }
    if (fabs(newValue - oldValue) / oldValue > LOWEST_INCONSISTENCY) {
        SD.FPrintf(outfile, "Inconsistent %s reading: %f compared to %f\n", sensorName, newValue, oldValue);
        inconsistentCount++;
    } else {
        consistentCount++;
    }

    consistencyProportion = (float)consistentCount / (consistentCount + inconsistentCount);
    SD.FPrintf(outfile, "Consistency proportion: %.3f\n", consistencyProportion);
}

void debugSensor(FEHFile* outfile, float startTime, float oldValue, char* name, AnalogInputPin sensor)
{
    LCD.Clear();

    LCD.Write(name);
    LCD.Write(": ");
    LCD.WriteLine(sensor.Value());

    float secondsElapsed = TimeNow() - startTime;
    SD.FPrintf(outfile, "%s: %f, Time: %f", name, sensor.Value(), secondsElapsed);

    checkSensorConsistency(sensor.Value(), oldValue, name, outfile);

    SD.FPrintf(outfile, "----------------------------------------\n");
}

int main()
{
    LCD.Clear();

    FEHFile* outfile = SD.FOpen("lightSensor.txt", "w");
    float startTime = TimeNow();
    float oldValue = 0.0;

    while (true) {
        debugSensor(outfile, TimeNow(), oldValue, "CdS cell", lightSensor);
        oldValue = lightSensor.Value();
        Sleep(0.1);
    }

    SD.FClose(outfile);
}
