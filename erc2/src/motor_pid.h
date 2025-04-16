#ifndef MOTOR_PID_H
#define MOTOR_PID_H

#include <FEH.h>

class PID {
    private:
        float kp; // Proportional gain
        float ki; // Integral gain
        float kd; // Derivative gain

        float accumulatedError; // For integral
        float lastError; // For derivative (dx/dt)
        float lastTime; // For time difference

    public:
        PID(float p = 1.0, float i = 0.0, float d = 0.0);
        void reset();
        void setParameters(float p, float i, float d);
        float update(float targetCounts, float current, float dt);
};

bool runPID(int targetCounts, FEHMotor& rightMotor, FEHMotor& leftMotor, DigitalEncoder& rightEncoder, DigitalEncoder& leftEncoder, float baseRightPower, float baseLeftPower, float timeout = 10.0, float tolerance = 2.0);

#endif