#include <FEHRCS.h>
#include <FEHLCD.h>

int main(void)
{
    RCS.InitializeTouchMenu("0410H5NPE"); // Run menu to select region (ABCDEFGH)
    // after this we need to find the red kill switch for our region and pull it

    Sleep(1.0);

    LCD.Write("Current region: ");
    LCD.WriteLine(RCS.CurrentRegionLetter());

    LCD.Write("Current course: ");
    LCD.WriteLine(RCS.CurrentCourse());

}