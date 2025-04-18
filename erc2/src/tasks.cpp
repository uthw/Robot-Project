#include "motor_helpers.h"
#include "utils.h"
#include <FEH.h>
#include <FEHUtility.h>

#define DARK_THRESHOLD 1.5 // Brightest light level (lowest voltage) that is considered dark
#define DARK_TOLERANCE 0.0 // Not needed
#define IS_DARK(voltage) (voltage >= DARK_THRESHOLD - DARK_TOLERANCE)

#define BLUE_AVG 0.72
#define BLUE_TOLERANCE 0.15
#define IS_BLUE(voltage) (voltage > BLUE_AVG - BLUE_TOLERANCE && voltage < BLUE_AVG + BLUE_TOLERANCE)

#define RED_AVG 0.34
#define RED_TOLERANCE 0.15
#define IS_RED(voltage) (voltage > RED_AVG - RED_TOLERANCE && voltage < RED_AVG + RED_TOLERANCE)

#define WINDOW_TIMEOUT 5

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
        
    }
    LCD.Clear();
    LCD.Write("Lit value found: ");
    LCD.WriteLine(lightSensor.Value());
    LCD.WriteLine("Starting");
}

// Same as detectStartDebug but without the LCD output
void detectStart()
{
    // 1 sample of not being dark within N/A seconds
    while (IS_DARK(lightSensor.Value())) {
        Sleep(0);
    }
}

// Motors go forward at percent power until light sensor detects light
void goForwardUntilLight(int percent)
{
    if (IS_DARK(lightSensor.Value())) {
        goForward(percent);
    }

    float startTime = TimeNow();

    while (IS_DARK(lightSensor.Value()) && TimeNow() - startTime < 10) {
        Sleep(0);
    }

    stopMotors();
}

void printLightSensorValue() {
    LCD.Write("Light sensor value: ");
    LCD.WriteLine(lightSensor.Value());
}

// Travel to and complete button task.
// Caution: will need to update inches since this was written while motor speed depended on battery
void humidifier_button(int speed)
{
    setLeverArmDegree(50); // So the arm doesn't affect pressing the button
    // Go to button light and read
    goForwardUntilLight(speed);
    goForward(speed, 0.5); // experimental correction
    Sleep(0.75);

    LCD.Clear();
    bool wasRed = false;
    if (IS_RED(lightSensor.Value())) {
        wasRed = true;
        turnRight(22, 10);
        LCD.Clear();

        LCD.WriteLine("RED");
        printLightSensorValue();
    } else if (IS_BLUE(lightSensor.Value())) {
        turnLeft(22, 10);
        LCD.Clear();

        LCD.WriteLine("BLUE");
        printLightSensorValue();
    } else {
        // Just do a random one
        if (rand() % 2 == 0) {
            wasRed = true;
            turnRight(22, 10);
            LCD.Clear();

            LCD.WriteLine("Random (red)");
            printLightSensorValue();
        } else {
            turnLeft(22, 10);
            LCD.Clear();

            LCD.WriteLine("Random (blue)");
            printLightSensorValue();
        }
    }

    goForwardTimed(30, 1);

    if (wasRed) {
        turnRight(25, 5);
        // turnLeft(25, 10);
        Sleep(0);
    } else {
        goBackward(speed, 2);
        turnRight(25, 25);
    }
}

void finalButton()
{
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
    goForward(speed, 6);
    // turnLeft(speed, 4);
    goForward(speed, 2.85);
    // waitForTouch("touch to turn composter");
    turnComposter(100, 2); // Turn composter
    turnComposter(-100, 2); // Turn composter back
}


void appleBasketM4(int speed) {
    // Navigate to the stump
    goBackward(speed, 3);
    turnRight(speed, 102);
    goBackward(speed, 10);
    goForward(speed, 21.25);
    turnLeft(speed, 85); // Now facing the stump

    setLeverArmDegree(120);

    goForward(speed, 10, MOTOR_DOWNTIME, 3.0);
    // waitForTouch("Touch to pick up apples");
    setLeverArmDegree(90);

    goBackward(speed, 3);
    turnRight(speed, 20);
    goBackward(speed, 27);
    goForward(25, 4);
    turnRight(35, 90);
    goForward(35, 28);
    turnLeft(25, 46);
    goForward(25, 7.5); // Going Diagonal
    turnRight(25, 50);
    goForward(25, 15.5, 0.2, 4.0);
    // drop bucket off
    setLeverArmDegree(130);
    Sleep(0.2);
}

// Travels from compost bin to apple basket and carries the basket to the depot to drop it
void appleBasket(int speed)
{
    // Navigate to the stump
    goBackward(speed, 3);
    turnRight(speed, 102);
    goBackward(speed, 10);
    goForward(speed, 21.25);
    turnLeft(speed, 85); // Now facing the stump

    setLeverArmDegree(120); // Forward

    goForward(speed, 10, MOTOR_DOWNTIME, 5.0);
    // waitForTouch("touch to pick up apples");
    setLeverArmDegree(65); // Pick up apples

    goBackward(speed, 3);
    turnRight(speed, 20);
    goBackward(speed, 27); // Bump into plexiglass
    goForward(speed, 3);
    turnRight(speed, 95);
    goForward(speed * 2, 30); // Go up ramp

    // Go to basket
    turnLeft(speed, 100);
    goBackward(speed, 9);
    goForward(speed, 9.5);
    turnRight(speed, 102); // this value may need to be adjusted (original 105)
    goForward(speed, 15.5);

    // Deposit apples
    setLeverArmDegree(140);
}

void leverA(int speed)
{
    goForward(speed, 12);
    turnRight(speed, 90);
    goForward(speed, 6.5);
    setLeverArmDegreeInstant(140); // Lower the lever
    Sleep(1.0);
    goBackward(speed, 4);
    setLeverArmDegree(170);
    Sleep(5.0); // Wait 5 seconds for extra points
    goForward(speed, 4);
    setLeverArmDegreeInstant(110); // Raise lever back up
    Sleep(1.0);

    goBackward(speed, 9);
    turnLeft(speed, 45);
}

void leverAAlt(int speed)
{
    goForward(speed, 9.0); // Goes across blue lines on ground
    turnRight(speed, 92); // Facing lever (up from 92)
    // waitForTouch("touch to pull lever");
    goForward(speed, 6.5);
    setLeverArmDegreeInstant(160); // Lower the lever
    Sleep(1.0);
    // if (RCS.isLeverFlipped() != 0) { // Try again if failed
    //     goBackward(speed, 4);
    //     setLeverArmDegree(90); // Raise
    //     turnRight(speed, 5);
    //     goForward(speed, 6.5);
    //     setLeverArmDegreeInstant(160); // Lower the leer
    //     Sleep(1.0);
    // }
    goBackward(speed, 4);
    setLeverArmDegree(170);
    Sleep(5.0); // Wait 5 seconds for extra points
    goForward(speed, 4.5);
    setLeverArmDegreeInstant(110); // Raise lever back up
    Sleep(1.0);
    goBackward(speed, 8.5);
    turnLeft(speed, 45);
    goForward(speed, 1);
}


void leverRCS(int speed)
{
    // RCS-dependent
    int correctLever = RCS.GetLever(); // 0: left, 1: middle, 2: right
    switch (correctLever) {
    case LEFT:
        leverA(speed);
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

    leverAAlt(speed);
}


// Travels from button to window to open and close it
void windowBonus(int speed)
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

void window(int speed) {
    // Robot moves backwards to align with window, opens it, then finishes

    // waitForTouch("touch to open window");

    goBackward(30, 30); // Go back into the plexiglass

    float startTime = TimeNow();
    goForward(speed * 0.8);
    // Goes forward until window is open or 15 seconds have passed
    while (RCS.isWindowOpen() == 0 && TimeNow() - startTime < WINDOW_TIMEOUT) {
        // Sleep(0);
        LCD.Clear();
        LCD.Write("isWindowOpen: ");
        LCD.WriteLine(RCS.isWindowOpen());
        LCD.Write("Elapsed Time: ");
        LCD.WriteLine(TimeNow() - startTime);
        Sleep(0.1); // Add a small delay to avoid excessive polling
    }
    stopMotors();

    // waitForTouch("touch for final button");

    goBackward(speed, 30); // Go back into plexiglass again
    goForward(speed, 5);
    turnRight(speed, 90); // Turn towards ending button
    goBackward(speed, 45); // Finish
}


// Like motorControlGUI but for tasks
void taskGUI()
{
    LCD.Clear();
    LCD.SetFontColor(WHITE);
    LCD.WriteLine("What should I do?");
    LCD.WriteAt("Button", XMAX / 4, YMAX / 4); // left region
    LCD.WriteAt("Lever", XMAX / 2 + 10, YMAX / 4); // right region
    LCD.WriteAt("Compost", XMAX / 4 - 20, YMAX / 2 + 10); // bottom left region
    LCD.WriteAt("Apple", XMAX / 2 + 10, YMAX / 2 + 10); // top part of bottom right region
    LCD.WriteAt("Window", XMAX / 2 + 10, YMAX * 0.75 + 20); // bottom part of bottom right region

    int x, y;

    LCD.DrawVerticalLine(XMAX / 2, 20, YMAX); // goes halfway down
    LCD.DrawHorizontalLine(YMAX / 2, 0, XMAX); // goes all the way across
    LCD.DrawHorizontalLine(YMAX * 0.75, XMAX / 2, XMAX);

    LCD.WaitForTouchToStart();
    while (!LCD.Touch(&x, &y)) {
        Sleep(0.1);
    }

    int spd = 35;

    // Determine which region was touched
    if (x < XMAX / 2 && y < YMAX / 2) { // Button
        waitForTouch("Touch for humidifier");
        humidifier_button(spd);
    } else if (x >= XMAX / 2 && y < YMAX / 2) { // Lever
        waitForTouch("Touch for lever");
        leverAAlt(spd);
    } else if (x < XMAX / 2 && y >= YMAX / 2) { // Compost
        waitForTouch("Touch for compost");
        detectStartDebug();
        compostBin(spd);
    } else if (x >= XMAX / 2 && y >= YMAX / 2) { // Apple
        waitForTouch("Touch for apple basket");
        appleBasketM4(spd);
    } else if (y >= YMAX * 0.75) { // Window
        waitForTouch("Touch for window");
        window(spd);
    }
}

void run() {
    int speed = 35;

    RCS.InitializeTouchMenu("0410H5NPE"); // Uncomment for official runs (RCS)
    detectStartDebug();

    // start button
    goBackward(25, 2);
    Sleep(0.5);
    goForward(25, 1.5);

    // goBackwardsAndReturn(25, 0.3);

    compostBin(speed);
    appleBasketM4(speed);

    // waitForTouch("touch for levers");

    levers(speed); // RCS-dependent lever code

    // waitForTouch("touch for humidifier");

    humidifier_button(speed); // this one also does window and final button

    window(speed); // RCS Independent

    // LCD.Clear();
    // LCD.WriteLine("touch for window");
    // LCD.WaitForTouchToStart();
    // Sleep(TOUCH_BUFFER);
    // window(speed);
}

// Asks user what they want to do (run, motor control, task control)
// Main menu interface that lets users choose between running a competition sequence,
// accessing motor controls, or running individual tasks
void mainMenuGUI()
{
    while (true) {
        LCD.Clear();
        LCD.SetFontColor(WHITE);
        LCD.WriteLine("What should I do?");
        LCD.WriteAt("Off. Run", XMAX / 4 - 40, YMAX / 3); // top left region
        LCD.WriteAt("Motor", XMAX * 3 / 4 - 30, YMAX / 3); // top right region
        LCD.WriteAt("Task", XMAX / 4 - 20, YMAX * 2 / 3 + 20); // bottom left region
        LCD.WriteAt("Calibrate", XMAX * 3 / 4 - 30, YMAX * 2 / 3 + 20); // bottom right region

        int x, y;

        // Draw dividing lines
        LCD.DrawVerticalLine(XMAX / 2, 20, YMAX); // vertical divider all the way down
        LCD.DrawHorizontalLine(YMAX * 2 / 3, 0, XMAX); // horizontal divider

        // Wait for touch
        LCD.WaitForTouchToStart();
        while (!LCD.Touch(&x, &y)) {
            Sleep(0.1);
        }
        Sleep(TOUCH_BUFFER);

        // Determine which region was touched
        if (y < YMAX * 2 / 3) {
            if (x < XMAX / 2) {
                // Run Course selected
                LCD.Clear();
                run();
            } else {
                // Motor Control selected
                motorControlGUI();
            }
        } else {
            if (x < XMAX / 2) {
                // Task Test selected
                taskGUI();
            } else {
                // Calibrate Light Sensor
                LCD.Clear();
                LCD.WriteLine("Starting light sensor calibration");
                Sleep(0.5);
                calibrateLightSensor();
            }
        }
    }
}