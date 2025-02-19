#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHSD.h>

// Declarations for analog optosensors
AnalogInputPin right_opto(FEHIO::P1_0);
AnalogInputPin middle_opto(FEHIO::P2_1);
AnalogInputPin left_opto(FEHIO::P2_2);

int main(void)
{
    FEHFile *outfile = SD.FOpen("Output.txt", "w");

    float x, y; //for touch screen

    // Open output file and prepare for writing values to it
    // <ADD CODE HERE>

    //Initialize the screen
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);

    LCD.WriteLine("Analog Optosensor Testing");
    LCD.WriteLine("Touch the screen");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed

    // Record values for optosensors on and off of the straight line
    // Left Optosensor on straight line
    LCD.Clear(BLACK);
    LCD.WriteLine("Place left optosensor on straight line");
    Sleep(0.25); // Wait to avoid double input
    LCD.WriteLine("Touch screen to record value (1/12)");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed
    // Write the value returned by the optosensor to your output file
    SD.FPrintf(outfile, "Left optosensor on straight line: %f\n", left_opto.Value());

    // Left Optosensor off straight line
    LCD.Clear(BLACK);
    LCD.WriteLine("Place left optosensor off straight line");
    Sleep(0.25); // Wait to avoid double input
    LCD.WriteLine("Touch screen to record value (2/12)");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed
    // Write the value returned by the optosensor to your output file
    SD.FPrintf(outfile, "Left optosensor off straight line: %f\n", left_opto.Value());

    // Middle Optosensor on straight line
    LCD.Clear(BLACK);
    LCD.WriteLine("Place middle optosensor on straight line");
    Sleep(0.25); // Wait to avoid double input
    LCD.WriteLine("Touch screen to record value (3/12)");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed
    // Write the value returned by the optosensor to your output file
    SD.FPrintf(outfile, "Middle optosensor on straight line: %f\n", middle_opto.Value());

    // Middle Optosensor off straight line
    LCD.Clear(BLACK);
    LCD.WriteLine("Place middle optosensor off straight line");
    Sleep(0.25); // Wait to avoid double input
    LCD.WriteLine("Touch screen to record value (4/12)");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed
    // Write the value returned by the optosensor to your output file
    SD.FPrintf(outfile, "Middle optosensor off straight line: %f\n", middle_opto.Value());

    // right Optosensor on straight line
    LCD.Clear(BLACK);
    LCD.WriteLine("Place right optosensor on straight line");
    Sleep(0.25); // Wait to avoid double input
    LCD.WriteLine("Touch screen to record value (5/12)");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed
    // Write the value returned by the optosensor to your output file
    SD.FPrintf(outfile, "Right optosensor on straight line: %f\n", right_opto.Value());
    
    // right Optosensor off straight line
    LCD.Clear(BLACK);
    LCD.WriteLine("Place right optosensor off straight line");
    Sleep(0.25); // Wait to avoid double input
    LCD.WriteLine("Touch screen to record value (6/12)");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed
    // Write the value returned by the optosensor to your output file
    SD.FPrintf(outfile, "Right optosensor off straight line: %f\n", right_opto.Value());

    // Curved lines
    // Left Optosensor on curved line
    LCD.Clear(BLACK);
    LCD.WriteLine("Place left optosensor on curved line");
    Sleep(0.25); // Wait to avoid double input
    LCD.WriteLine("Touch screen to record value (7/12)");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed
    SD.FPrintf(outfile, "Left optosensor on curved line: %f\n", left_opto.Value());

    // Left Optosensor off curved line
    LCD.Clear(BLACK);
    LCD.WriteLine("Place left optosensor off curved line");
    Sleep(0.25); // Wait to avoid double input
    LCD.WriteLine("Touch screen to record value (8/12)");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed
    SD.FPrintf(outfile, "Left optosensor off curved line: %f\n", left_opto.Value());

    // Middle Optosensor on curved line
    LCD.Clear(BLACK);
    LCD.WriteLine("Place middle optosensor on curved line");
    Sleep(0.25); // Wait to avoid double input
    LCD.WriteLine("Touch screen to record value (9/12)");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed
    SD.FPrintf(outfile, "Middle optosensor on curved line: %f\n", middle_opto.Value());

    // Middle Optosensor off curved line
    LCD.Clear(BLACK);
    LCD.WriteLine("Place middle optosensor off curved line");
    Sleep(0.25); // Wait to avoid double input
    LCD.WriteLine("Touch screen to record value (10/12)");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed
    SD.FPrintf(outfile, "Middle optosensor off curved line: %f\n", middle_opto.Value());

    // Right Optosensor on curved line
    LCD.Clear(BLACK);
    LCD.WriteLine("Place right optosensor on curved line!");
    Sleep(0.25); // Wait to avoid double input
    LCD.WriteLine("Touch screen to record value (11/12)");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed
    SD.FPrintf(outfile, "Right optosensor on curved line: %f\n", right_opto.Value());

    // Right Optosensor on curved line
    LCD.Clear(BLACK);
    LCD.WriteLine("Place right optosensor off curved line!");
    Sleep(0.25); // Wait to avoid double input
    LCD.WriteLine("Touch screen to record value (12/12)");
    while(!LCD.Touch(&x,&y)); //Wait for screen to be pressed
    while(LCD.Touch(&x,&y)); //Wait for screen to be unpressed
    SD.FPrintf(outfile, "Right optosensor off curved line: %f\n", right_opto.Value());

    // Close output file
    SD.FClose(outfile);

    // Print end message to screen
    LCD.Clear(BLACK);
    LCD.WriteLine("Test Finished");

    return 0;
}