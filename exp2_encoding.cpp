#include <FEHAccel.h>
#include <FEHBattery.h>
#include <FEHBuzzer.h>
#include <FEHIO.h>
#include <FEHLCD.h>
#include <FEHMotor.h>
#include <FEHSD.h>
#include <FEHServo.h>
#include <FEHUtility.h>
#include <stdio.h>
#include <string.h>

// Declarations for encoders & motors
DigitalEncoder rightencoder(FEHIO::P0_0);
DigitalEncoder leftencoder(FEHIO::P0_1);
FEHMotor leftmotor(FEHMotor::Motor0, 9.0);
FEHMotor rightmotor(FEHMotor::Motor1, 9.0);

void move_forward(int percent, int counts) // using encoders
{
    // Reset encoder counts
    rightencoder.ResetCounts();
    leftencoder.ResetCounts();

    // Set both motors to desired percent
    rightmotor.SetPercent(percent);
    leftmotor.SetPercent(percent);

    // While the average of the left and right encoder is less than counts,
    // keep running motors
    while ((leftencoder.Counts() + rightencoder.Counts()) / 2. < counts)
        ;

    // Turn off motors
    rightmotor.Stop();
    leftmotor.Stop();
}

void turn_right(int percent, int counts) // using encoders
{
    // Reset encoder counts
    rightencoder.ResetCounts();
    leftencoder.ResetCounts();

    rightmotor.SetPercent(-percent);
    leftmotor.SetPercent(percent);
    // While the average of the left and right encoder is less than counts,
    // keep running motors
    while ((leftencoder.Counts() + rightencoder.Counts()) / 2. < counts)
        ;
    // Turn off motors
    rightmotor.Stop();
    leftmotor.Stop();
}
void turn_left(int percent, int counts) // using encoders
{
    // Reset encoder counts
    rightencoder.ResetCounts();
    leftencoder.ResetCounts();
    // Set both motors to desired percent
    rightmotor.SetPercent(percent);
    leftmotor.SetPercent(-percent);
    // While the average of the left and right encoder is less than counts,
    // keep running motors
    while ((leftencoder.Counts() + rightencoder.Counts()) / 2. < counts)
        ;
    // Turn off motors
    rightmotor.Stop();
    leftmotor.Stop();
}

int main(void)
{
    int motor_percent = 25; // Input power level here
    int expected_counts = 227; // Input theoretical counts here
    int sleep25 = 3, sleep40 = 2.5, sleep60 = 1.5;

    float x, y; // for touch screen
    int i = 0;
    // Initialize the screen
    for (i; i < 3; i++) {
        switch (i) {
        case 0:
            motor_percent = 25;
            break;

        case 1:
            motor_percent = 40;
            break;

        case 2:
            motor_percent = 60;
            break;
        }
        LCD.Clear(BLACK);
        LCD.SetFontColor(WHITE);
        Sleep(500);

        LCD.WriteLine("Shaft Encoder Exploration Test");
        LCD.WriteLine("Touch the screen");
        while (LCD.Touch(&x, &y))
            ; // Wait for screen to be unpressed
        while (!LCD.Touch(&x, &y))
            ; // Wait for screen to be pressed
        while (LCD.Touch(&x, &y))
            ; // Wait for screen to be unpressed

        move_forward(motor_percent, 280); // see function
        Sleep(500);

        turn_left(motor_percent, 110);
        Sleep(500);

        move_forward(motor_percent, 200);
        Sleep(500);

        turn_right(motor_percent, 110);
        Sleep(500);

        move_forward(motor_percent, 80);
        Sleep(500);
    }

    Sleep(2.0); // Wait for counts to stabilize
    // Print out data
    LCD.Write("Theoretical Counts: ");
    LCD.WriteLine(expected_counts);
    LCD.Write("Motor Percent: ");
    LCD.WriteLine(motor_percent);
    LCD.Write("Actual LE Counts: ");
    LCD.WriteLine(leftencoder.Counts());
    LCD.Write("Actual RE Counts: ");
    LCD.WriteLine(rightencoder.Counts());

    return 0;
}