using namespace std;
#include <FEH.h>
#include <FEHIO.h>
#include <FEHMotor.h>
// Cmath and algorithm have been killed by ERC2
#include "utils.h"
#include <math.h>

// All perspectives (r/m/l) are from the back of the robot

DigitalEncoder rightEncoder(FEHIO::Pin13); // Encoder for the right Igwan motor
DigitalEncoder leftEncoder(FEHIO::Pin14); // Encoder for the left Igwan motor
FEHMotor rightMotor(FEHMotor::Motor0, 9.0); // Right Igwan motor
FEHMotor leftMotor(FEHMotor::Motor1, 9.0); // Left Igwan motor
FEHMotor composter(FEHMotor::Motor2, 5.0); // Composter motor

FEHServo leverArm2(FEHServo::Servo7); // Fitec high torque servo that controls the arm. Black wire on top

// Declarations for optosensors
AnalogInputPin rightOpto(FEHIO::Pin2); // Right optosensor
AnalogInputPin middleOpto(FEHIO::Pin1); // Middle optosensor
AnalogInputPin leftOpto(FEHIO::Pin0); // Left optosensor

// Convert desired degrees to number of counts required to rotate said number of degrees
#define DEGREES_TO_COUNTS(degrees) (int)(degrees * COUNTS_IN_90_DEGREES / 90)
#define INCHES_TO_COUNTS(inches) (int)(inches * COUNTS_IN_1_INCH)

// Motors have less power as battery decreases
#define ACTUAL_PERCENTAGE_POWER(percent) min((int)((MAX_VOLTAGE / Battery.Voltage()) * percent), 100)

#define COUNTS_IN_90_DEGREES 223 // Old: 215
#define COUNTS_IN_1_INCH 32 // Old: 32

#define LEFT_MODIFIER 1 // Added to left motor speed. Set to 0 if it's going straight right now

#define MOTOR_DOWNTIME 0.2 // Time in seconds motors stop after a drivetrain method is called
#define SERVO_ADJUSTMENT_INTERVAL 0.005 // Time in seconds between servo movements
#define OPTOSENSOR_INTERVAL 0.005 // Time in seconds between optosensor readings

#define LEVER_ARM_MIN 500
#define LEVER_ARM_MAX 2500
#define LEVER_ARM_DEFAULT 120

int lastDegree = LEVER_ARM_DEFAULT;

void turnRight(int percent, int degrees)
{
    // Convert degrees to counts
    int counts = DEGREES_TO_COUNTS(degrees);
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);

    // Reset counts
    rightEncoder.ResetCounts();
    leftEncoder.ResetCounts();

    // Set motors to desired percent
    rightMotor.SetPercent(actualPercent * -1);
    leftMotor.SetPercent(actualPercent + LEFT_MODIFIER);

    // Run motors until avg of left and right encoder equals counts
    while ((leftEncoder.Counts() + rightEncoder.Counts()) / 2.0 < counts) {
        Sleep(0);
    }

    // Turn off motors
    rightMotor.Stop();
    leftMotor.Stop();
    Sleep(MOTOR_DOWNTIME);
}

void turnLeft(int percent, int degrees)
{
    // Convert degrees to counts
    int counts = DEGREES_TO_COUNTS(degrees);
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);

    // Reset counts
    rightEncoder.ResetCounts();
    leftEncoder.ResetCounts();

    // Set motors to desired percent
    rightMotor.SetPercent(actualPercent + LEFT_MODIFIER);
    leftMotor.SetPercent(actualPercent * -1);

    // Run motors until avg of left and right encoder equals counts
    while ((leftEncoder.Counts() + rightEncoder.Counts()) / 2.0 < counts) {
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
    rightMotor.SetPercent(actualPercent);
    leftMotor.SetPercent(actualPercent + LEFT_MODIFIER);

    // Run motors until avg of left and right encoder equals counts
    while ((leftEncoder.Counts() + rightEncoder.Counts()) / 2.0 < counts) {
        // For some reason empty while loops don't work on arduino. This is a workaround
        Sleep(0);
    }

    // Turn off motors
    rightMotor.Stop();
    leftMotor.Stop();
    Sleep(MOTOR_DOWNTIME);
}

// Motors go forward at percent power for seconds
void goForwardTimed(int percent, float seconds)
{
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);

    rightMotor.SetPercent(actualPercent);
    leftMotor.SetPercent(actualPercent + LEFT_MODIFIER);

    Sleep(seconds);

    rightMotor.Stop();
    leftMotor.Stop();
    Sleep(MOTOR_DOWNTIME);
}

void goForward(int percent)
{
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);

    rightMotor.SetPercent(actualPercent);
    leftMotor.SetPercent(actualPercent + LEFT_MODIFIER);
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

float getOptoReading(AnalogInputPin& opto, int samples)
{
    float sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += opto.Value();
        Sleep(OPTOSENSOR_INTERVAL);
    }
    return sum / samples;
}

void calibrateOptosensors()
{
    LCD.Clear();
    // Black line
    LCD.WriteLine("Place LEFT sensor over BLACK line and touch the screen");
    LCD.WaitForTouchToStart();
    Sleep(TOUCH_BUFFER);
    float avgBlack = getOptoReading(leftOpto, 10);
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

void turnComposter(int percent) {
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);
    composter.SetPercent(actualPercent);
}

void stopComposter() {
    composter.Stop();
    Sleep(MOTOR_DOWNTIME);
}

void turnComposter(int percent, float seconds) {
    int actualPercent = ACTUAL_PERCENTAGE_POWER(percent);
    composter.SetPercent(actualPercent);
    Sleep(seconds);
    composter.Stop();
    Sleep(MOTOR_DOWNTIME);
}

// Touch GUI for manually controlling motors. It exits after a command is given, so wrap it in a while (true) to keep doing it
void motorControlGUI()
{
    LCD.Clear();
    LCD.SetFontColor(WHITE);
    LCD.WriteLine("Touch a region:");
    LCD.WriteAt("Left", XMAX / 4, YMAX / 4); // left region
    LCD.WriteAt("Right", XMAX / 2 + 10, YMAX / 4); // right region
    LCD.WriteAt("Forward", XMAX / 4 - 20, YMAX / 2 + 10); // bottom left region
    LCD.WriteAt("LevArm", XMAX / 2 + 10, YMAX / 2 + 10); // top part of bottom right region
    LCD.WriteAt("Compost", XMAX / 2 + 10, YMAX * 0.75 - 20); // bottom part of bottom right region

    int x, y;

    LCD.DrawVerticalLine(XMAX / 2, 20, YMAX); // goes halfway down
    LCD.DrawHorizontalLine(YMAX / 2, 0, XMAX); // goes all the way across

    // Wait for touch
    while (!LCD.Touch(&x, &y)) {
        Sleep(0.1);
    }

    int motorPower = 25;

    // Determine which region was touched
    if (x < XMAX / 2 && y < YMAX / 2) { // Left
        int deg = getValueTouch("Set degrees to turn left", 0, 360, 15, 90);
        turnLeft(motorPower, deg);
    } else if (x >= XMAX / 2 && y < YMAX / 2) { // Right
        int deg = getValueTouch("Set degrees to turn right", 0, 360, 15, 90);
        turnRight(motorPower, deg);
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
            int power = getValueTouch("Set composter power", -100, 100, 5, 25);
            turnComposter(power);
            LCD.WriteLine("Touch to stop composter");
            LCD.WaitForTouchToStart();
            Sleep(TOUCH_BUFFER);
            stopComposter();
        }
    }
}

