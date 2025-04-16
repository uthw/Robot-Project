#include "motor_pid.h"
#include <FEH.h>

// Constructor with default gains
PID::PID(float p, float i, float d)
{
    kp = p;
    ki = i;
    kd = d;
    reset();
}

// Reset controller state
void PID::reset()
{
    accumulatedError = 0.0;
    lastError = 0.0;
    lastTime = TimeNow();
}

// Set new parameters in case you want to try tuning them
void PID::setParameters(float p, float i, float d)
{
    kp = p;
    ki = i;
    kd = d;
}

// Calculate PID output based on error
// @param targetCounts
//  desired number of counts after which the motors should stop
// @param current
//  current number of counts
// @param dt
//  time difference between the last and current call to update
// @return
//  PID output value
float PID::update(float targetCounts, float current, float dt)
{
    // Calculate error
    float error = targetCounts - current;

    // Get P term: proportional to current error
    float pTerm = kp * error;

    // Get I term: integral (accumulated error over time)
    accumulatedError += error * dt;
    // Anti windup: bound accumulatedError between -100/ki and 100/ki to prevent motors from going insane
    if (accumulatedError > 100.0 / ki) {
        accumulatedError = 100.0 / ki;
    } else if (accumulatedError < -100.0 / ki) {
        accumulatedError = -100.0 / ki;
    }
    float iTerm = ki * accumulatedError; // Integral term

    // Get D term: change in error over time
    float errorRate = (error - lastError) / dt;
    float dTerm = kd * errorRate; // Derivative term

    lastError = error; // Update last error for next call

    return pTerm + iTerm + dTerm; // Return total PID output
}

// Run motors using PID loop until the target counts is reached or the loop times out
// @return
//  true if the target counts was reached, false if the timeout was reached first
bool runPID(int targetCounts, FEHMotor& rightMotor, FEHMotor& leftMotor, DigitalEncoder& rightEncoder, DigitalEncoder& leftEncoder, float baseRightPower, float baseLeftPower, float timeout = 10.0, float tolerance = 2.0)
{
    PID pid(0.5, 0.1, 0.05); // Needs to be adjusted fs
    pid.reset();

    float startTime = TimeNow();
    float currentTime = 0.0;
    float lastTime = startTime;

    while (true) {
        float currentCounts = (leftEncoder.Counts() + rightEncoder.Counts()) / 2.0;

        // Break early if target counts is reached
        // Also summon the wrath of SW1
        if (fabs(targetCounts - currentCounts) <= tolerance) {
            return true;
        }

        // Break early if the timeout threshold is broken
        currentTime = TimeNow();
        if (currentTime - startTime > timeout) {
            return false;
        }

        // Find dt (t2 - t1)
        float dt = currentTime - lastTime;

        // Correct dt if it's very small to prevent excessive correction/updates
        if (dt < 0.01) {
            Sleep(0.01 - dt);
            dt = 0.01;
        }

        // Get PID output (correction value)
        float correction = pid.update(targetCounts, currentCounts, dt);

        // Correct motors
        float rightPower = baseRightPower + correction / 2;
        float leftPower = baseLeftPower - correction / 2;

        // Bound motor powers between -100 to 100
        if (rightPower > 100) {
            rightPower = 100;
        } else if (rightPower < -100) {
            rightPower = -100;
        }
        if (leftPower > 100) {
            leftPower = 100;
        } else if (leftPower < -100) {
            leftPower = -100;
        }

        rightMotor.SetPercent(rightPower);
        leftMotor.SetPercent(leftPower);

        lastTime = currentTime;
    }
}