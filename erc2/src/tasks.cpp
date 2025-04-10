#include "motor_helpers.h"
#include "utils.h"
#include <FEHUtility.h>
#include <FEH.h>

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


AnalogInputPin lightSensor(FEHIO::Pin8); // Red yellow orange from screen to battery

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

// Travel to and complete button task. 
// Caution: will need to update inches since this was written while motor speed depended on battery
void humidifier_button(int speed)
{
    // Go to button light and read
    goForwardUntilLight(speed);
    Sleep(0.75);

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

// Travels from start button to compost bin and rotates it
void compostBin(int speed) {
    goForward(speed, 2);
    turnRight(speed, 45);
    goForward(speed, 6);
    turnComposter(75, 5); // Turn composter
    turnComposter(-75, 5); // Turn composter back
}

// Travels from compost bin to apple basket and carries the basket to the depot to drop it
void appleBasket(int speed) {
    // Go to apples
    turnRight(speed, 45);
    goForward(speed, 11);
    setLeverArmDegree(130);
    goForward(speed, 7);

    // Pick up apples
    setLeverArmDegree(90);

    // Go to basket
    goBackward(speed, 4);
    turnRight(speed, 45);
    goBackward(speed, 4);
    turnLeft(speed, 45);
    goForward(-speed, 20);
    goForward(speed, 4);
    turnRight(speed, 90);
    goForward(35, 25);
    turnLeft(speed, 45);
    goForward(speed, 10);
    turnRight(speed, 47);
    goForward(speed, 15.5);

    // Deposit apples
    setLeverArmDegree(140);
}

// Travels from depot to levers to pull down the right one.
void levers(int speed) {
    // Lever
    goBackward(speed, 4);
    setLeverArmDegree(90);
    turnLeft(speed, 135); // Might be 45?

    // RCS-independent (always goes to lever C and pulls it)
    // goForward(speed, 8);
    // setLeverArmDegree(160); // Lower the lever
    // goBackward(speed, 5);
    // setLeverArmDegree(165); // Lower position to raise after - was originally
    // Sleep(5.0); // Wait 5 seconds for extra points
    // turnLeft(speed, 5); // Realign to go straight into the lever
    // goForward(speed, 5);
    // setLeverArmDegree(110); // Raise lever back up

    // RCS-dependent
    int correctLever = RCS.GetLever(); // 0: left, 1: middle, 2: right
    switch (correctLever) {
    case 0:
        goForward(speed, 12);
        turnRight(speed, 90);
        goForward(speed, 4);
        setLeverArmDegree(130); // Lower the lever
        goBackward(speed, 4);
        setLeverArmDegree(170);
        Sleep(5.0); // Wait 5 seconds for extra points
        goForward(speed, 4);
        setLeverArmDegree(110); // Raise lever back up
        goBackward(speed, 10);
        turnLeft(speed, 45);
        goForward(speed, 3);
        break;
    case 1:
        goForward(speed, 7);
        turnRight(speed, 90);
        goForward(speed, 4);
        setLeverArmDegree(130); // Lower the lever
        goBackward(speed, 4);
        setLeverArmDegree(170);
        Sleep(5.0); // Wait 5 seconds for extra points
        goForward(speed, 4);
        setLeverArmDegree(110); // Raise lever back up
        goBackward(speed, 14);
        turnLeft(speed, 45);
    case 2:
        goForward(speed, 3);
        turnRight(speed, 90);
        goForward(speed, 4);
        setLeverArmDegree(130); // Lower the lever
        goBackward(speed, 4);
        setLeverArmDegree(170);
        Sleep(5.0); // Wait 5 seconds for extra points
        goForward(speed, 4);
        setLeverArmDegree(110); // Raise lever back up
        // add new code later
    default:
        LCD.WriteLine("RCS error");
        break;
    }
}

// Travels from button to window to open and close it
void window(int speed) {
    // Align with wall near ramp - this part uses dummy values
    goBackward(speed, 40);
    turnLeft(speed, 90);
    goForward(speed, 6);
    turnRight(speed, 90);

    // Facing window now
    goForward(40, 23); // Open window. Uses old value from milestone 3 code

    // Close window

    // Maneuver around the window handle
    goBackward(40, 2);
    turnRight(speed, 15);
    goForward(40, 2);
    turnLeft(speed, 10);
    goForward(40, 3); // Up against flowers now

    // Collide with window
    goBackward(40, 20);

    // Could add something here where the robot tries to run it back if it fails the first time
}

