#include "motor_helpers.h"
#include "utils.h"
#include <FEHUtility.h>

#define DARK_THRESHOLD 3.3
#define DARK_TOLERANCE 0.1 // Dummy value
#define IS_DARK(voltage) (voltage >= DARK_THRESHOLD - DARK_TOLERANCE)

// #define BLUE_THRESHOLD_HIGH 0.65
// #define BLUE_THRESHOLD_LOW 0.35
// #define IS_BLUE(voltage) (voltage > BLUE_THRESHOLD_LOW && voltage < BLUE_THRESHOLD_HIGH)

// #define RED_THRESHOLD_HIGH 0.33
// #define RED_THRESHOLD_LOW 0.0
// #define IS_RED(voltage) (voltage > RED_THRESHOLD_LOW && voltage < RED_THRESHOLD_HIGH)

#define BLUE_AVG 0.6 // Dummy value
#define BLUE_TOLERANCE 0.1 // Dummy value
#define IS_BLUE(voltage) (voltage > BLUE_AVG - BLUE_TOLERANCE && voltage < BLUE_AVG + BLUE_TOLERANCE)

#define RED_AVG 0.2 // Dummy value
#define RED_TOLERANCE 0.1 // Dummy value
#define IS_RED(voltage) (voltage > RED_AVG - RED_TOLERANCE && voltage < RED_AVG + RED_TOLERANCE)


AnalogInputPin lightSensor(FEHIO::Pin8); // It's like yellow red orange or something from screen to bottom

// Detects the start light illuminating before continuing
void detectStartDebug()
{
    LCD.WriteLine("Waiting to start");
    while (IS_DARK(lightSensor.Value())) {
        LCD.WriteLine(lightSensor.Value());
    }
    LCD.Clear();
    LCD.WriteLine("Starting");
}

// Same as detectStartDebug but without the LCD output
void detectStart()
{
    while (IS_DARK(lightSensor.Value())) {
    }
}

// Motors go forward at percent power until light sensor detects light
void goForwardUntilLight(int percent)
{
    goForward(percent);

    while (IS_DARK(lightSensor.Value())) { }

    stopMotors();
}

// Caution: will need to update inches since this was written while motor speed depended on battery
void humidifier_button()
{
    LCD.Clear();
    bool wasRed = false;
    if (IS_RED(lightSensor.Value())) {
        wasRed = true;
        turnRight(25, 10);
        LCD.Clear();

        LCD.WriteLine("RED");
    } else if (IS_BLUE(lightSensor.Value())) {
        turnLeft(25, 10);
        LCD.Clear();

        LCD.WriteLine("BLUE");
    } else {
        // Just do a random one
        if (rand() % 2 == 0) {
            wasRed = true;
            turnRight(25, 10);
            LCD.Clear();

            LCD.WriteLine("RED");
        } else {
            turnLeft(25, 10);
            LCD.Clear();

            LCD.WriteLine("BLUE");
        }
    }

    goForwardTimed(30, 1);

    if (wasRed) {
        turnLeft(25, 10);
    } else {
        turnRight(25, 10);
    }
}

// This can be used to determine what the light sensor thinks a red, blue, and dark LED is. The values are shown on screen after calibration is done.
void calibrateLightSensor() {
    LCD.Clear();

    LCD.WriteLine("Place light sensor over unlit (dark) LED and touch the screen");
    LCD.WaitForTouchToStart();
    Sleep(TOUCH_BUFFER);
    float* voltagesDark = new float[READING_COUNT];
    float avgDark = getVoltages(voltagesDark, lightSensor);
    float stdDark = standardDeviationOfVoltages(voltagesDark, avgDark);
    delete[] voltagesDark;
    LCD.Clear();

    LCD.WriteLine("Place light sensor over red LED and touch the screen");
    LCD.WaitForTouchToStart();
    Sleep(TOUCH_BUFFER);
    float* voltagesRed = new float[READING_COUNT];
    float avgRed = getVoltages(voltagesRed, lightSensor);
    float stdRed = standardDeviationOfVoltages(voltagesRed, avgRed);
    delete[] voltagesRed;
    LCD.Clear();

    LCD.WriteLine("Place light sensor over blue LED and touch the screen");
    LCD.WaitForTouchToStart();
    Sleep(TOUCH_BUFFER);
    float* voltagesBlue = new float[READING_COUNT];
    float avgBlue = getVoltages(voltagesBlue, lightSensor);
    float stdBlue = standardDeviationOfVoltages(voltagesBlue, avgBlue);
    delete[] voltagesBlue;
    LCD.Clear();

    // The value is going to be the average of the readings
    // The tolerance is stddev and is how much the value can vary while still being the same color
    // LCD.SetBackgroundColor(WHITE);
    // LCD.SetFontColor(BLACK);
    LCD.Write("Dark value: ");
    LCD.WriteLine(avgDark);
    LCD.Write("Dark tolerance (not used): ");
    LCD.WriteLine(stdDark);
    LCD.SetFontColor(RED);
    LCD.Write("Red value: ");
    LCD.WriteLine(avgRed);
    LCD.Write("Red tolerance: ");
    LCD.WriteLine(stdRed);
    LCD.SetFontColor(BLUE);
    LCD.Write("Blue value: ");
    LCD.WriteLine(avgBlue);
    LCD.Write("Blue tolerance: ");
    LCD.WriteLine(stdBlue);
    LCD.SetFontColor(WHITE);
    LCD.WriteLine("Even if tolerance is 0, still");
    LCD.WriteLine("set it to ~0.05-0.1 just in case");
    LCD.WriteLine("Touch to continue");
    LCD.WaitForTouchToStart();
    Sleep(TOUCH_BUFFER);
    LCD.Clear();
    // LCD.SetBackgroundColor(BLACK);
    LCD.SetFontColor(WHITE);
    LCD.WriteLine("Calibration complete");
}