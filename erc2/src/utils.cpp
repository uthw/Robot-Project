#include <FEHLCD.h>
#include <FEHUtility.h>
#include <FEH.h>

#define MAX_VOLTAGE 11.5 // Actually closer to 11.52-11.55
#define EMPTY_VOLTAGE 9.0 // Estimated

#define YMAX 240
#define XMAX 320

#define READING_COUNT 200 // Max value due to memory constraints: ~1500
#define READING_INTERVAL 0.05 // Time in seconds between readings

// Clears the screen and displays the battery percentage
void DisplayBatteryPercent()
{
    float voltage = Battery.Voltage();
    float percentage = ((voltage - EMPTY_VOLTAGE) / (MAX_VOLTAGE - EMPTY_VOLTAGE)) * 100;
    // Clamp percentage to 0-100
    if (percentage < 0)
        percentage = 0;
    else if (percentage > 100)
        percentage = 100;

    // Display battery
    LCD.Clear();
    LCD.Write("Battery: ");
    LCD.Write(percentage);
    LCD.Write("%");
}

// Reads the average of [array length] readings into the array
// @param voltages
//      array to store the readings
// @param sensor
//      AnalogInputPin to read from
// @return average of readings
float getVoltages(float* voltages, AnalogInputPin& sensor)
{ // IDK if the * is gonna work
    float sum = 0;
    int len = sizeof(voltages) / sizeof(voltages[0]);

    for (int i = 0; i < len; i++) {
        voltages[i] = sensor.Value();
        sum += voltages[i];
        Sleep(READING_INTERVAL);
    }

    float average = sum / len;

    return average;
}

// Calculates the standard deviation of all values in voltages, but could be used to find the standard deviation of any float[]
// @param voltages
//      array of voltages
// @param avg
//      average of voltages
// @return standard deviation of voltages
float standardDeviationOfVoltages(float* voltages, float avg)
{
    int len = sizeof(voltages) / sizeof(voltages[0]);
    float stddev = 0;
    for (int i = 0; i < len; i++) {
        stddev += (voltages[i] - avg) * (voltages[i] - avg); // sigma of (x - mu)^2
    }
    stddev = sqrt(stddev / len); // sqrt(sigma / N)
    return stddev;
}