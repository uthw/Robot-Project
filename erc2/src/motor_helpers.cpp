using namespace std;
#include <FEH.h>
#include <FEHIO.h>
#include <FEHMotor.h>
// Cmath and algorithm have been killed by ERC2
#include "motor_helpers.h"
#include "motor_pid.h"
#include "utils.h"
#include <math.h>

// All perspectives (r/m/l) are from the back of the robot

DigitalEncoder rightEncoder(FEHIO::Pin12); // Encoder for the right Igwan motor
DigitalEncoder leftEncoder(FEHIO::Pin14); // Encoder for the left Igwan motor
FEHMotor rightMotor(FEHMotor::Motor0, 9.0); // Right Igwan motor
FEHMotor leftMotor(FEHMotor::Motor1, 9.0); // Left Igwan motor
FEHMotor composter(FEHMotor::Motor2, 5.0); // Composter motor

FEHServo leverArm2(FEHServo::Servo7); // Fitec high torque servo that controls the arm. Black wire on top

DigitalInputPin bumpSwitch(FEHIO::Pin6); // Bump switch on the back of the robot

// Declarations for optosensors
AnalogInputPin middleOpto(FEHIO::Pin0); // Middle optosensor

// Convert desired degrees to number of counts required to rotate said number of degrees
#define DEGREES_TO_COUNTS(degrees) (int)(degrees * COUNTS_IN_90_DEGREES / 90)
#define INCHES_TO_COUNTS(inches) (int)(inches * COUNTS_IN_1_INCH)

// Motors have less power as battery decreases
#define ACTUAL_PERCENTAGE_POWER(percent) min((int)((MAX_VOLTAGE / Battery.Voltage()) * percent), 100)

#define COUNTS_IN_90_DEGREES 225 // Old: 215
#define COUNTS_IN_1_INCH 32 // Old: 32

#define LEFT_FACTOR 1 // Applied to left motor
#define RIGHT_FACTOR -1 // Applied to right motor
#define TURN_FACTOR 0.75

#define MOTOR_DOWNTIME 0.2 // Time in seconds motors stop after a drivetrain method is called
#define SERVO_ADJUSTMENT_INTERVAL 0.005 // Time in seconds between servo movements
#define OPTOSENSOR_INTERVAL 0.05 // Time in seconds between optosensor readings

#define LEVER_ARM_MIN 500
#define LEVER_ARM_MAX 2500
#define LEVER_ARM_DEFAULT 120

#define MAX_TIMEOUT 10 // Maximum time in seconds for goForward

int lastDegree = LEVER_ARM_DEFAULT;

enum bumpSwitchState {
    NOT_ENGAGED = 1,
    ENGAGED = 0
};

// Writes encoder counts to the bottom half of the screen with a 0.2 second timeout, which can be useful for seeing which one is disconnected, if any
// TODO add adjusted percent and battery lvl to the display
void writeDebugMotor()
{
    LCD.SetFontColor(BLACK);
    LCD.FillRectangle(0, YMAX / 2, XMAX, YMAX);
    Sleep(0.2);
    LCD.SetFontColor(WHITE);
    LCD.WriteAt("left counts: ", 0, YMAX / 2);
    LCD.WriteLine(leftEncoder.Counts());
    LCD.WriteAt("right counts: ", 0, YMAX / 2 + 20);
    LCD.WriteLine(rightEncoder.Counts());
}

// Runs the right motor at -power and left motor at power to have the robot turn right in place.
// @param percent
//      percentage of power to run the motors at
// @param degrees
//      degrees to turn
void turnRight(int percent, int degrees)
{
    // Convert degrees to counts
    int counts = DEGREES_TO_COUNTS(degrees);
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);

    // Reset counts
    rightEncoder.ResetCounts();
    leftEncoder.ResetCounts();

    // Set motors to desired percent
    int rightPercent = actualPercent * -1 * RIGHT_FACTOR * TURN_FACTOR;
    int leftPercent = actualPercent * LEFT_FACTOR * TURN_FACTOR;
    rightMotor.SetPercent(rightPercent);
    leftMotor.SetPercent(leftPercent);

    // Run motors until avg of left and right encoder equals counts
    while ((leftEncoder.Counts() + rightEncoder.Counts()) / 2.0 < counts) {
        // writeDebugMotor();
        Sleep(0);
    }

    // Turn off motors
    rightMotor.Stop();
    leftMotor.Stop();
    Sleep(MOTOR_DOWNTIME);
}

// Runs the right motor at a power and left motor at -power to have the robot turn left in place.
// @param percent
//      percentage of power to run the motors at
// @param degrees
//      degrees to turn
void turnLeft(int percent, int degrees)
{
    // Convert degrees to counts
    int counts = DEGREES_TO_COUNTS(degrees);
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);

    // Reset counts
    rightEncoder.ResetCounts();
    leftEncoder.ResetCounts();

    // Set motors to desired percent
    int rightPercent = actualPercent * RIGHT_FACTOR * TURN_FACTOR;
    int leftPercent = actualPercent * -1 * LEFT_FACTOR * TURN_FACTOR;
    rightMotor.SetPercent(rightPercent);
    leftMotor.SetPercent(leftPercent);

    // Run motors until avg of left and right encoder equals counts
    while ((leftEncoder.Counts() + rightEncoder.Counts()) / 2.0 < counts) {
        // writeDebugMotor();
        Sleep(0);
    }

    // Turn off motors
    rightMotor.Stop();
    leftMotor.Stop();
    Sleep(MOTOR_DOWNTIME);
}

// Motors go forward at percent power for inches
void goForward(int percent, float inches)
{
    // Convert inches to counts
    // Could change this to mm later if more precision needed
    int counts = INCHES_TO_COUNTS(inches);
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);

    // Reset counts
    rightEncoder.ResetCounts();
    leftEncoder.ResetCounts();

    // Set motors to desired percent
    int rightPercent = actualPercent * RIGHT_FACTOR;
    int leftPercent = actualPercent * LEFT_FACTOR;
    rightMotor.SetPercent(rightPercent);
    leftMotor.SetPercent(leftPercent);

    float startTime = TimeNow();

    // Run motors until avg of left and right encoder equals counts
    while ((leftEncoder.Counts() + rightEncoder.Counts()) / 2.0 < counts &&
           TimeNow() - startTime < MAX_TIMEOUT) {
        // For some reason empty while loops don't work on arduino. This is a workaround
        // writeDebugMotor();
        Sleep(0);
    }

    // Turn off motors
    rightMotor.Stop();
    leftMotor.Stop();
    Sleep(MOTOR_DOWNTIME);
}

// Motors go forward at percent power for seconds before stopping
void goForwardTimed(int percent, float seconds)
{
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);

    rightMotor.SetPercent(actualPercent * RIGHT_FACTOR);
    leftMotor.SetPercent(LEFT_FACTOR * actualPercent);

    Sleep(seconds);

    rightMotor.Stop();
    leftMotor.Stop();
    Sleep(MOTOR_DOWNTIME);
}

// Motors go forward. They won't stop until you call a timed/distance based version of goForward or stopMotors()
void goForward(int percent)
{
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);

    rightMotor.SetPercent(LEFT_FACTOR * actualPercent * RIGHT_FACTOR);
    leftMotor.SetPercent(actualPercent);
}

// Overload of goForward that allows to customize the downtime
void goForward(int percent, float inches, float downtime)
{
    goForward(percent, inches);
    Sleep(downtime - MOTOR_DOWNTIME);
}

void stopMotors()
{
    rightMotor.Stop();
    leftMotor.Stop();
    Sleep(MOTOR_DOWNTIME);
}

void goBackward(int percent, float inches)
{
    goForward(percent * -1, inches);
}

void goBackward(int percent)
{
    goForward(percent * -1);
}

int getValueTouch(const char* message, int min, int max, int increment, int initialValue)
{
    LCD.Clear();
    LCD.SetFontColor(WHITE);
    LCD.DrawVerticalLine(XMAX / 2, 20, YMAX * 0.8); // goes all the way down
    LCD.DrawHorizontalLine(YMAX * 0.8, 0, XMAX); // goes all the way across

    LCD.WriteLine(message);

    LCD.WriteAt("Down", XMAX / 4, YMAX * 0.8 - 20);
    LCD.WriteAt("Up", XMAX / 2 + 10, YMAX * 0.8 - 20);
    LCD.WriteAt("Confirm", XMAX / 2 + 10, YMAX * 0.8 + 20);

    int value = initialValue;
    while (true) {
        // Redraw old value
        LCD.SetFontColor(BLACK);
        LCD.FillRectangle(XMAX / 4 - 20, YMAX * 0.8 + 10, 60, 20);
        LCD.SetFontColor(WHITE);
        LCD.WriteAt(value, XMAX / 4 - 20, YMAX * 0.8 + 10);

        // Wait for touch
        LCD.WaitForTouchToStart();
        // Check where the touch was
        int x, y;
        LCD.Touch(&x, &y);
        Sleep(TOUCH_BUFFER);

        if (x < XMAX / 2 && y < YMAX * 0.8) {
            // Left region, decrease value
            value -= increment;
            if (value < min) {
                value = min;
            }
        } else if (x >= XMAX / 2 && y < YMAX * 0.8) {
            // Right region, increase value
            value += increment;
            if (value > max) {
                value = max;
            }
        } else if (y >= YMAX * 0.8) {
            // Bottom region, return value
            LCD.Clear();
            LCD.WriteAt("Confirmed value: ", 0, 0);
            LCD.WriteLine(value);
            Sleep(0.25);
            return value;
        }
    }
}

// Takes a number of readings from the optosensor and returns the average
float getOptoReading(AnalogInputPin& opto, int samples)
{
    float sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += opto.Value();
        Sleep(OPTOSENSOR_INTERVAL);
    }
    return sum / samples;
}

// Takes 4 readings of the middle optosensor and shows the average reflectiveness on screen
// The readings are for on black line, off black line, on blue line, and off blue line
void calibrateOptosensors()
{
    LCD.Clear();
    // Black line
    LCD.WriteLine("Place middle sensor on black line and touch the screen");
    LCD.WaitForTouchToStart();
    Sleep(TOUCH_BUFFER);
    float avgBlack = getOptoReading(middleOpto, 10);
    LCD.WriteLine("Place middle sensor completely off black line and touch the screen");
    LCD.WaitForTouchToStart();
    Sleep(TOUCH_BUFFER);
    float avgOffBlack = getOptoReading(middleOpto, 10);

    LCD.Clear();

    LCD.WriteLine("Place middle sensor on blue line and touch the screen");
    LCD.WaitForTouchToStart();
    Sleep(TOUCH_BUFFER);
    float avgBlue = getOptoReading(middleOpto, 10);
    LCD.WriteLine("Place middle sensor completely off blue line and touch the screen");
    LCD.WaitForTouchToStart();
    Sleep(TOUCH_BUFFER);
    float avgOffBlue = getOptoReading(middleOpto, 10);
    LCD.Clear();
    LCD.Write("On black line: ");
    LCD.WriteLine(avgBlack);
    LCD.Write("Off black line: ");
    LCD.WriteLine(avgOffBlack);
    LCD.Write("On blue line: ");
    LCD.WriteLine(avgBlue);
    LCD.Write("Off blue line: ");
    LCD.WriteLine(avgOffBlue);
    LCD.WriteLine("Touch to continue");
    LCD.WaitForTouchToStart();
    Sleep(TOUCH_BUFFER);
    LCD.Clear();
}

// Gradually moves the lever arm to the desired degree, one degree per SERVO_ADJUSTMENT_INTERVAL seconds. Moving the servo too fast might be helping it fall off
void setLeverArmDegree(int degree)
{
    leverArm2.SetMin(LEVER_ARM_MIN);
    leverArm2.SetMax(LEVER_ARM_MAX);
    int increment = degree > lastDegree ? 1 : -1;
    for (int i = lastDegree; i != degree; i += increment) {
        leverArm2.SetDegree(i);
        Sleep(SERVO_ADJUSTMENT_INTERVAL);
    }
    lastDegree = degree;
    Sleep(MOTOR_DOWNTIME);
}

// Instantly sets the lever arm to the desired degree. This exists so we don't have to set min and max in the main method; it does the same thing as leverArm.SetDegree
void setLeverArmDegreeInstant(int degree)
{
    leverArm2.SetMin(LEVER_ARM_MIN);
    leverArm2.SetMax(LEVER_ARM_MAX);
    leverArm2.SetDegree(degree);
    lastDegree = degree;
    Sleep(MOTOR_DOWNTIME);
}

// Turns the composter servo at the desired percent power until stopComposter is called
void turnComposter(int percent)
{
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);
    composter.SetPercent(actualPercent);
}

// Stops the composter servo
void stopComposter()
{
    composter.Stop();
    Sleep(MOTOR_DOWNTIME);
}

// Turns the composter servo at the desired percent power and then stops it after the desired number of seconds
void turnComposter(int percent, float seconds)
{
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);
    composter.SetPercent(actualPercent);
    Sleep(seconds);
    composter.Stop();
    Sleep(MOTOR_DOWNTIME);
}

void goForwardPID(int percent, float inches)
{
    // Convert inches to counts
    // Could change this to mm later if more precision needed
    int counts = INCHES_TO_COUNTS(inches);
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);

    // Reset counts
    rightEncoder.ResetCounts();
    leftEncoder.ResetCounts();

    // Set motors to desired percent
    int rightPercent = actualPercent * RIGHT_FACTOR;
    int leftPercent = actualPercent * LEFT_FACTOR;

    runPID(counts, rightMotor, leftMotor, rightEncoder, leftEncoder, rightPercent, leftPercent);
}

void turnLeftPID(int percent, int degrees)
{
    // Convert degrees to counts
    int counts = DEGREES_TO_COUNTS(degrees);
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);

    // Reset counts
    rightEncoder.ResetCounts();
    leftEncoder.ResetCounts();

    // Set motors to desired percent
    int rightPercent = actualPercent * RIGHT_FACTOR;
    int leftPercent = actualPercent * -1 * LEFT_FACTOR;

    runPID(counts, rightMotor, leftMotor, rightEncoder, leftEncoder, rightPercent, leftPercent);
}

void turnRightPID(int percent, int degrees)
{
    float scale = 1.1;
    // Convert degrees to counts
    int counts = DEGREES_TO_COUNTS(degrees * scale);
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);

    // Reset counts
    rightEncoder.ResetCounts();
    leftEncoder.ResetCounts();

    // Set motors to desired percent
    int rightPercent = actualPercent * -1 * RIGHT_FACTOR;
    int leftPercent = actualPercent * LEFT_FACTOR;

    runPID(counts, rightMotor, leftMotor, rightEncoder, leftEncoder, rightPercent, leftPercent);
}

// Touch GUI for manually controlling motors. It exits after a command is given, so wrap it in a while (true) to keep doing it
void motorControlGUI()
{
    // leftMotor.SetPercent(25);
    // Sleep(5.0);
    // leftMotor.Stop();
    // rightMotor.SetPercent(25);
    // Sleep(5.0);
    // rightMotor.Stop();

    LCD.Clear();
    LCD.SetFontColor(WHITE);
    LCD.WriteLine("Touch a region:");
    LCD.WriteAt("Left", XMAX / 4, YMAX / 4); // left region
    LCD.WriteAt("Right", XMAX / 2 + 10, YMAX / 4); // right region
    LCD.WriteAt("Forward", XMAX / 4 - 20, YMAX / 2 + 10); // bottom left region
    LCD.WriteAt("Arm", XMAX / 2 + 10, YMAX / 2 + 10); // top part of bottom right region
    LCD.WriteAt("Compost", XMAX / 2 + 10, YMAX * 0.75 + 20); // bottom part of bottom right region

    int x, y;

    LCD.DrawVerticalLine(XMAX / 2, 20, YMAX); // goes halfway down
    LCD.DrawHorizontalLine(YMAX / 2, 0, XMAX); // goes all the way across
    LCD.DrawHorizontalLine(YMAX * 0.75, XMAX / 2, XMAX);

    // Wait for touch
    while (!LCD.Touch(&x, &y)) {
        Sleep(0.1);
    }

    int motorPower = 35;

    // Determine which region was touched
    if (x < XMAX / 2 && y < YMAX / 2) { // Left
        int deg = getValueTouch("Set degrees to turn left", 0, 360, 5, 90);
        turnLeftPID(motorPower, deg);
    } else if (x >= XMAX / 2 && y < YMAX / 2) { // Right
        int deg = getValueTouch("Set degrees to turn right", 0, 360, 5, 90);
        turnRightPID(motorPower, deg);
    } else if (x < XMAX / 2 && y >= YMAX / 2) { // Forward
        int inches = getValueTouch("Set inches to go forward", -60, 60, 1, 6);

        if (inches < 0) {
            inches *= -1;
            motorPower *= -1;
        }
        goForward(motorPower, inches);
    } else if (x >= XMAX / 2 && y >= YMAX / 2) { // Servos
        if (y < YMAX * 0.75) { // Lever arm
            int angle = getValueTouch("Set servo angle", 0, 180, 5, 90);
            setLeverArmDegree(angle);
        } else if (y >= YMAX * 0.75) { // Compost mechanism
            int power = getValueTouch("Set composter power", -100, 100, 25, 75);
            turnComposter(power);
            LCD.WriteLine("Touch to stop composter");
            LCD.WaitForTouchToStart();
            Sleep(TOUCH_BUFFER);
            stopComposter();
        }
    }
}

// Goes backwards for seconds and then goes forward the same number of counts it just went backwards
void goBackwardsAndReturn(int percent, float seconds)
{
    rightEncoder.ResetCounts();
    leftEncoder.ResetCounts();

    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);

    rightMotor.SetPercent(actualPercent * -1 * RIGHT_FACTOR);
    leftMotor.SetPercent(actualPercent * -1);
    Sleep(1.0);
    int countsForReturn = (leftEncoder.Counts() + rightEncoder.Counts()) / 2.0;

    rightMotor.Stop();
    leftMotor.Stop();
    Sleep(MOTOR_DOWNTIME);

    rightEncoder.ResetCounts();
    leftEncoder.ResetCounts();
    rightMotor.SetPercent(actualPercent * RIGHT_FACTOR);
    leftMotor.SetPercent(actualPercent);
    while ((leftEncoder.Counts() + rightEncoder.Counts()) / 2.0 < countsForReturn) {
        Sleep(0);
    }
    rightMotor.Stop();
    leftMotor.Stop();

    Sleep(MOTOR_DOWNTIME);
}

// Currently unused alternative that uses a bump switch instead of time
void goBackwardsUntilBumpAndReturn(int percent)
{
    rightEncoder.ResetCounts();
    leftEncoder.ResetCounts();

    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);

    rightMotor.SetPercent(actualPercent * -1);
    leftMotor.SetPercent(actualPercent * -1);
    while (bumpSwitch.Value() == NOT_ENGAGED) {
        Sleep(0);
    }
    int countsForReturn = (leftEncoder.Counts() + rightEncoder.Counts()) / 2.0;
    Sleep(0.3);

    rightMotor.Stop();
    leftMotor.Stop();
    Sleep(MOTOR_DOWNTIME);

    rightEncoder.ResetCounts();
    leftEncoder.ResetCounts();
    rightMotor.SetPercent(actualPercent);
    leftMotor.SetPercent(actualPercent);
    while ((leftEncoder.Counts() + rightEncoder.Counts()) / 2.0 < countsForReturn) {
        Sleep(0);
    }
    rightMotor.Stop();
    leftMotor.Stop();

    Sleep(MOTOR_DOWNTIME);
}

void goForward(int percent, float inches, float downtime, float timeout)
{
    // Convert inches to counts
    // Could change this to mm later if more precision needed
    int counts = INCHES_TO_COUNTS(inches);
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);

    // Reset counts
    rightEncoder.ResetCounts();
    leftEncoder.ResetCounts();

    // Set motors to desired percent
    int rightPercent = actualPercent * RIGHT_FACTOR;
    int leftPercent = LEFT_FACTOR * actualPercent;
    rightMotor.SetPercent(rightPercent);
    leftMotor.SetPercent(leftPercent);

    float startTime = TimeNow();

    // Run motors until avg of left and right encoder equals counts
    while ((leftEncoder.Counts() + rightEncoder.Counts()) / 2.0 < counts &&
           TimeNow() - startTime < timeout) {
        // For some reason empty while loops don't work on arduino. This is a workaround
        // writeDebugMotor();
        Sleep(0);
    }

    // Turn off motors
    rightMotor.Stop();
    leftMotor.Stop();
    Sleep(downtime);
}

