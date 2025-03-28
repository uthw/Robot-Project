#include <FEHLCD.h>
#include <FEHUtility.h>

#define MAX_VOLTAGE 11.5 // Actually closer to 11.52-11.55
#define EMPTY_VOLTAGE 9.0 // Estimated

#define YMAX 240
#define XMAX 320

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