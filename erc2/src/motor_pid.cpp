#include "motor_pid.h"
#include <FEH.h>

#define MAX_SPEED 50 // Limits maximum dynamic speed of motors (max: 100)
#define MAX_CORRECTION 0.4
#define HIGH_SPEED_SCALING 1.25 // Scales up proportional correction for higher speeds

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
    if (accumulatedError > MAX_SPEED / ki) {
        accumulatedError = MAX_SPEED / ki;
    } else if (accumulatedError < -MAX_SPEED / ki) {
        accumulatedError = -MAX_SPEED / ki;
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
bool runPID(int targetCounts, FEHMotor& rightMotor, FEHMotor& leftMotor, DigitalEncoder& rightEncoder, DigitalEncoder& leftEncoder, float baseRightPower, float baseLeftPower)
{
    float tolerance = 1.0;
    float timeout = 10.0;
    float baseMagnitude = fabs(baseRightPower); // Magnitude of base power for scaling
    float speedAmplifier = min(1.0, baseMagnitude / 15);
    float kp = 0.8 + (speedAmplifier * HIGH_SPEED_SCALING); // Proportional gain

    PID pid(kp, 0.0, 0.1); // Needs to be adjusted fs
    pid.reset();

    float startTime = TimeNow();
    float currentTime = 0.0;
    float lastTime = startTime;

    // Stall detection
    float lastPosition = 0;
    float stallStartTime = 0;
    bool isStalled = false;

    while (true) {
        // Break early if the timeout threshold is broken
        currentTime = TimeNow();

        if (currentTime - startTime > timeout) {
            rightMotor.Stop();
            leftMotor.Stop();
            return false;
        }

        // Update current counts based on driving straight or turning
        bool shouldPivot = (baseRightPower * baseLeftPower > 0); // True if R and L have opposite signs
        float currentCounts;
        if (shouldPivot) {
            // Absolute value for pivots
            currentCounts = (fabs(leftEncoder.Counts()) + fabs(rightEncoder.Counts())) / 2.0;
        } else {
            // Regular average
            currentCounts = (leftEncoder.Counts() + rightEncoder.Counts()) / 2.0;
        }

        // Break early if target counts is reached
        // Also summon the wrath of SW1
        if (fabs(targetCounts - currentCounts) <= tolerance) {
            rightMotor.Stop();
            leftMotor.Stop();
            return true;
        }

        // STALL DETECTION - Secondary exit condition
        // Check if we're still moving
        if (fabs(currentCounts - lastPosition) < 3) {
            // Not moving or moving very slowly
            if (!isStalled) {
                // Start stall timer
                stallStartTime = currentTime;
                isStalled = true;
            } else if (currentTime - stallStartTime > 0.5) {
                // Stalled for over 0.5 seconds - we're done
                rightMotor.Stop();
                leftMotor.Stop();
                return false;
            }
        } else {
            // We're moving - reset stall detection
            isStalled = false;
            lastPosition = currentCounts;
        }

        // Calculate time step
        float dt = currentTime - lastTime;
        if (dt < 0.01) {
            Sleep(0.01 - dt);
            dt = 0.01;
        }

        float correction = pid.update(targetCounts, currentCounts, dt); // Get PID output (correction value)
        float maxCorrection = 0.0;

        if (shouldPivot) {
            // For turns, we need a minimum correction capability regardless of speed
            maxCorrection = max(10.0f, baseMagnitude * HIGH_SPEED_SCALING); // At least 10% or HSS of base power
        } else {
            // For straight driving, keep the proportional limit
            maxCorrection = baseMagnitude * MAX_CORRECTION;
        }

        if (correction > maxCorrection) {
            correction = maxCorrection;
        } else if (correction < -maxCorrection) {
            correction = -maxCorrection;
        }

        float rightPower = 0.0, leftPower = 0.0;

        // Correct motors
        if (shouldPivot) { // apply full correction if it should spin in place
            if (baseRightPower < 0) { // Right turn
                rightPower = baseRightPower - correction;
                leftPower = baseLeftPower - correction;
            } else { // Left turn
                rightPower = baseRightPower + correction;
                leftPower = baseLeftPower + correction;
            }
        } else { // split correction between motors if it should go straight
            // LCD.WriteLine("should go straight");
            rightPower = baseRightPower - correction / 2;
            leftPower = baseLeftPower + correction / 2;
        }

        

        // Bound motor powers
        if (rightPower > MAX_SPEED) {
            rightPower = MAX_SPEED;
        } else if (rightPower < -MAX_SPEED) {
            rightPower = -MAX_SPEED;
        }
        if (leftPower > MAX_SPEED) {
            leftPower = MAX_SPEED;
        } else if (leftPower < -MAX_SPEED) {
            leftPower = -MAX_SPEED;
        }

        rightMotor.SetPercent(rightPower);
        leftMotor.SetPercent(leftPower);

        lastTime = currentTime;
    }
}