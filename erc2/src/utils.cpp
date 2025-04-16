#include <FEH.h>
#include <FEHLCD.h>
#include <FEHUtility.h>

#define MAX_VOLTAGE 11.5 // Actually closer to 11.52-11.55
#define EMPTY_VOLTAGE 9.0 // Estimated

#define YMAX 240
#define XMAX 320

#define READING_COUNT 100 // Max value due to memory constraints: ~1500
#define READING_INTERVAL 0.05 // Time in seconds between readings

#define TOUCH_BUFFER 0.2
#define UPDATE_INTERVAL 0.5
#define CHAR_HEIGHT 20

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
float getVoltages(float* voltages, AnalogInputPin& sensor, int len)
{
    float sum = 0;
    LCD.Clear();
    LCD.WriteLine("Reading voltages...");
    float lastUpdate = TimeNow();

    for (int i = 0; i < len; i++) {
        voltages[i] = sensor.Value();
        sum += voltages[i];

        // Update progress on screen every 10 seconds
        if (TimeNow() - lastUpdate >= UPDATE_INTERVAL) {
            lastUpdate = TimeNow();
            LCD.DrawRectangle(0, (YMAX / 2) - CHAR_HEIGHT, XMAX, CHAR_HEIGHT * 2);
            LCD.WriteAt("Progress: ", 0, YMAX / 2);
            LCD.Write((i + 1) * 100 / len);
            LCD.WriteLine("%");
        }

        Sleep(READING_INTERVAL);
    }

    float average = sum / len;

    LCD.WriteLine(average);

    return average;
}

// Calculates the standard deviation of all values in voltages, but could be used to find the standard deviation of any float[]
// @param voltages
//      array of voltages
// @param avg
//      average of voltages
// @param len
//      length of voltages (so dynamic arrays can be used)
// @return standard deviation of voltages
float standardDeviationOfVoltages(float* voltages, float avg, int len)
{
    float stddev = 0;

    for (int i = 0; i < len; i++) {
        float diff = voltages[i] - avg;
        stddev += diff * diff;
    }

    stddev = sqrt(stddev / len);

    return stddev;
}

// Clears the screen, shows a message, and waits for touch to continue
// @param message
//      a message to display
void waitForTouch(const char* message)
{
    LCD.Clear();
    LCD.WriteLine(message);
    LCD.WaitForTouchToStart();
    Sleep(TOUCH_BUFFER);
}

void waitForTouchNoClear(const char* message)
{
    LCD.WriteLine(message);
    LCD.WaitForTouchToStart();
    Sleep(TOUCH_BUFFER);
}