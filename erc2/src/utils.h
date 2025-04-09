#ifndef UTILS_H
#define UTILS_H

#define MAX_VOLTAGE 11.5
#define EMPTY_VOLTAGE 9.0 // Estimated

#define YMAX 240
#define XMAX 320

#define TOUCH_BUFFER 0.2
#define READING_COUNT 200

void DisplayBatteryPercent();
float getVoltages(float* voltages, AnalogInputPin& sensor);
float standardDeviationOfVoltages(float* voltages, float avg);

#endif