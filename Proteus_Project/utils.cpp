
#include <FEH.h>

// Clears LCD and writes battery percentage
void WriteBatteryPercent()
{
    // show battery percent
    LCD.Clear();
    LCD.Write("Battery: ");
    LCD.Write((1 - (Battery.Voltage() / 11.5)) * 100);
    LCD.WriteLine("%");
}