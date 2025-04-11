#include "motor_helpers.h"
#include "utils.h"
#include <FEH.h>
#include <FEHUtility.h>

#define DARK_THRESHOLD 3.3
#define DARK_TOLERANCE 0.1 // Unused
#define IS_DARK(voltage) (voltage >= DARK_THRESHOLD - DARK_TOLERANCE)

// #define BLUE_THRESHOLD_HIGH 0.65
// #define BLUE_THRESHOLD_LOW 0.35
// #define IS_BLUE(voltage) (voltage > BLUE_THRESHOLD_LOW && voltage < BLUE_THRESHOLD_HIGH)

// #define RED_THRESHOLD_HIGH 0.33
// #define RED_THRESHOLD_LOW 0.0
// #define IS_RED(voltage) (voltage > RED_THRESHOLD_LOW && voltage < RED_THRESHOLD_HIGH)

#define BLUE_AVG 0.5
#define BLUE_TOLERANCE 0.15 
#define IS_BLUE(voltage) (voltage > BLUE_AVG - BLUE_TOLERANCE && voltage < BLUE_AVG + BLUE_TOLERANCE)

#define RED_AVG 0.17
#define RED_TOLERANCE 0.17
#define IS_RED(voltage) (voltage > RED_AVG - RED_TOLERANCE && voltage < RED_AVG + RED_TOLERANCE)

enum LeverPosition {
    LEFT = 0,
    MIDDLE = 1,
    RIGHT = 2
};

AnalogInputPin lightSensor(FEHIO::Pin7); // Red yellow orange from screen to battery

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
        LCD.WaitForTouchToStart();
        Sleep(TOUCH_BUFFER);
    } else if (IS_BLUE(lightSensor.Value())) {
        turnLeft(25, 10);
        LCD.Clear();
        
        LCD.WriteLine("BLUE");
        LCD.WaitForTouchToStart();
        Sleep(TOUCH_BUFFER);
    } else {
        // Just do a random one
        if (rand() % 2 == 0) {
            wasRed = true;
            turnRight(25, 10);
            LCD.Clear();

            LCD.WriteLine("Random (red)");
        } else {
            turnLeft(25, 10);
            LCD.Clear();

            LCD.WriteLine("Random (blue)");
        }
    }

    setLeverArmDegree(50); // 
    goForwardTimed(30, 1);

    if (wasRed) {
        turnRight(25, 5);
        // turnLeft(25, 10);
        Sleep(0);
    } else {
        goBackward(speed, 2);
        turnRight(25, 25);
    }

    // Robot moves backwards to align with window, opens it, then finishes

    // waitForTouch("touch to open window");

    goBackward(30, 35); // Go back into the plexiglass

    float startTime = TimeNow();
    goForward(speed * 0.8);
    // Goes forward until window is open or 15 seconds have passed
    while (RCS.isWindowOpen() == 0 && TimeNow() - startTime < 15) {
        Sleep(0);
    }
    stopMotors();

    // waitForTouch("touch for final button");

    goBackward(speed, 30); // Go back into plexiglass again
    goForward(speed, 4);
    turnLeft(speed, 90); // Turn towards ending button
    goForward(speed, 40); // Finish
}

void finalButton() {
    
}

// This can be used to determine what the light sensor thinks a red, blue, and dark LED is. The values are shown on screen after calibration is done.
void calibrateLightSensor()
{
    LCD.Clear();

    LCD.WriteLine("Place light sensor over unlit (dark) LED and touch the screen");
    LCD.WaitForTouchToStart();
    Sleep(TOUCH_BUFFER);
    float* voltagesDark = new float[READING_COUNT];
    float avgDark = getVoltages(voltagesDark, lightSensor, READING_COUNT);
    float stdDark = standardDeviationOfVoltages(voltagesDark, avgDark, READING_COUNT);
    delete[] voltagesDark;
    LCD.Clear();

    LCD.WriteLine("Place light sensor over red LED and touch the screen");
    LCD.WaitForTouchToStart();
    Sleep(TOUCH_BUFFER);
    float* voltagesRed = new float[READING_COUNT];
    float avgRed = getVoltages(voltagesRed, lightSensor, READING_COUNT);
    float stdRed = standardDeviationOfVoltages(voltagesRed, avgRed, READING_COUNT);
    delete[] voltagesRed;
    LCD.Clear();

    LCD.WriteLine("Place light sensor over blue LED and touch the screen");
    LCD.WaitForTouchToStart();
    Sleep(TOUCH_BUFFER);
    float* voltagesBlue = new float[READING_COUNT];
    float avgBlue = getVoltages(voltagesBlue, lightSensor, READING_COUNT);
    float stdBlue = standardDeviationOfVoltages(voltagesBlue, avgBlue, READING_COUNT);
    delete[] voltagesBlue;
    LCD.Clear();

    // Display calibration results
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
    LCD.WriteLine("Touch to continue");
    LCD.WaitForTouchToStart();
    Sleep(TOUCH_BUFFER);
    LCD.Clear();
}

// Travels from start button to compost bin and rotates it
void compostBin(int speed)
{
    setLeverArmDegree(50);
    // goForward(speed, 2);
    turnLeft(speed, 45);
    goForward(speed, 7);
    turnLeft(speed, 10);
    goForward(speed, 3);
    turnComposter(75, 3); // Turn composter
    turnComposter(-75, 3); // Turn composter back
}

// Travels from compost bin to apple basket and carries the basket to the depot to drop it
void appleBasket(int speed)
{
    // Navigate to the stump
    goBackward(speed, 3);
    turnRight(speed, 102);
    goBackward(speed, 10);
    goForward(speed, 21);
    turnLeft(speed, 85); // Now facing the stump

    setLeverArmDegree(120); // Forward

    goForward(speed, 10);
    setLeverArmDegree(70); // Pick up apples

    goBackward(speed, 3);
    turnRight(speed, 20);
    goBackward(speed, 27);
    goForward(speed, 3);
    turnRight(speed, 90);
    goForward(speed * 2, 30); // Go up ramp

    // Go to basket
    turnLeft(speed, 100);
    goBackward(speed, 8);
    goForward(speed, 10);
    turnRight(speed, 100);
    goForward(speed, 15.5);

    // Deposit apples
    setLeverArmDegree(140);

}

// Travels from depot to levers to pull down the right one.
void levers(int speed)
{
    // Lever
    goBackward(speed, 3);
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
    case LEFT:
        goForward(speed, 12);
        turnRight(speed, 90);
        goForward(speed, 6.5);
        setLeverArmDegree(130); // Lower the lever
        Sleep(1.0);
        goBackward(speed, 4);
        setLeverArmDegree(170);
        Sleep(5.0); // Wait 5 seconds for extra points
        goForward(speed, 4);
        setLeverArmDegree(110); // Raise lever back up

        goBackward(speed, 9);
        turnLeft(speed, 45);
        break;
    case MIDDLE:
        goForward(speed, 7);
        turnRight(speed, 90);
        goForward(speed, 6.5);
        setLeverArmDegreeInstant(130); // Lower the lever
        Sleep(1.0);
        goBackward(speed, 4);
        setLeverArmDegree(170);
        Sleep(5.0); // Wait 5 seconds for extra points
        goForward(speed, 4);
        setLeverArmDegreeInstant(110); // Raise lever back up

        goBackward(speed, 14);
        turnLeft(speed, 45);
        break;
    case RIGHT:
        goForward(speed, 3);
        turnRight(speed, 90);
        goForward(speed, 6.5);
        setLeverArmDegreeInstant(130); // Lower the lever
        Sleep(1.0);
        goBackward(speed, 4);
        setLeverArmDegree(170);
        Sleep(5.0); // Wait 5 seconds for extra points
        goForward(speed, 4);
        setLeverArmDegreeInstant(110); // Raise lever back up

        // experimental
        goBackward(speed, 18);
        turnLeft(speed, 45);
        break;

    default:
        LCD.WriteLine("RCS error");
        break;
    }
}

// Travels from button to window to open and close it
void window(int speed)
{
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
