// This program can consistently complete milestone 4 with 23/20 points. Commented out code is untested but might allow us to earn the last 3 for only interacting with the correct lever. 
// It shouldn't matter anymore, but battery was around 88% when it was tested.

#include <Arduino.h>
#include <FEH.h>
#include <FEHRCS.h>
#include "motor_helpers.h"
#include "utils.h"
#include "tasks.h"

FEHServo leverArm(FEHServo::Servo7); // black on top

void ERCMain()
{
    leverArm.SetMax(2500);
    leverArm.SetMin(500);

     //while (true) {
       //MotorControlGUI();
     //}
    
    // RCS.InitializeTouchMenu();
    //detectStart();

    // default (extending out towards robot) = ~165~ 120
    leverArm.SetDegree(100);
    // DisplayBatteryPercent();
     Sleep(1.0);
    goForward(-25, 2);
    // Sleep(2.3);
    turnRight(25, 90);
    Sleep(0.5);
    // Sleep(2.3);
     goForward(-25, 3);
     Sleep(0.5);
    // Sleep(2.3);
    // // pick up
    // //leverArm.SetDegree(90); // experimental
    turnLeft(25, 30);
    Sleep(0.5);
    // Sleep(2.3);
    goForward(-25, 3);
    Sleep(0.5);
    // Sleep(2.3);
    turnLeft(25, 15);
    Sleep(0.5);
    // Sleep(2.3);
    goForward(-25, 2);
    Sleep(0.5);
    // Sleep(2.3);
    //turnCompostTimed(25,2.0);
    leverArm.SetDegree(155);
    Sleep(1.0);
    goForward(25, 2);
    Sleep(1.2);
    leverArm.SetDegree(100);
    Sleep(1.2);
    goForward(-25, 2);
    Sleep(1.2);
    leverArm.SetDegree(155);
    Sleep(1.2);
     goForward(25, 2);
     Sleep(1.2);
    leverArm.SetDegree(100);
    Sleep(1.2);
    goForward(-25, 2);
    Sleep(1.2);
    leverArm.SetDegree(155);
    Sleep(2.0);
    //prepare for going back around
    leverArm.SetDegree(100);
    Sleep(1.2);
     goForward(25, 2);
     Sleep(1.2);
     leverArm.SetDegree(155);
     Sleep(1.2);
     goForward(-25, 2);
     Sleep(1.2);
     leverArm.SetDegree(100);
     Sleep(1.2);
     goForward(25, 2);
     Sleep(1.2);
     leverArm.SetDegree(155);
     Sleep(1.2);
     goForward(-25, 2);
     Sleep(1.2);
     leverArm.SetDegree(100);
     Sleep(1.2);
     goForward(25, 10);
     Sleep(.2);
     turnLeft(25, 45);
     Sleep(1.2);
     goForward(25, 2);







    //goForward(25, 4);
    //turnRight(25, 90);
    //goForward(35, 25);
    //turnLeft(25, 45);
    //goForward(25, 10);
    //turnRight(25, 47);
    //goForward(25, 15.5);
    // drop bucket off
    //leverArm.SetDegree(140);
    //Sleep(0.2);

    // levers    
    //goForward(-25, 4);
    //leverArm.SetDegree(90);
    //Sleep(0.2);

    // RCS experimental
    // int correctLever = RCS.GetCorrectLever();
    // // 0: left, 1: middle, 2: right
    // if (correctLever == 0) {
    //     turnLeft(25, 135);
    //     Sleep(0.2);
    //     // perpendicular to levers now. levers 4.5 inches away from e/o
    //     goForward(25, 9);
    //     Sleep(0.2);
    //     turnRight(25, 90);
    //     Sleep(0.2);
    //     goForward(25, 8);
    // } else if (correctLever == 1) {
    //     turnLeft(25, 135);
    //     Sleep(0.2);
    //     // perpendicular
    //     goForward(25, 4.5);
    //     // in front of lever
    //     Sleep(0.2);
    //     turnRight(25, 90);
    //     // facing and in front of lever
    //     Sleep(0.2);
    //     goForward(25, 8);

    // } else if (correctLever == 2) {
    //     turnLeft(25, 45);
    //     Sleep(0.2);
    //     goForward(25, 8);
    // }

    //turnLeft(25, 45);
   // goForward(25, 8);

   // leverArm.SetDegree(160); // lower the lever
   // Sleep(0.2);
    //goForward(-25, 5);
    //leverArm.SetDegree(180); // lower position to raise lever after
    //Sleep(5.0); // Bonus points: wait at least 5 seconds before raising lever
   // turnLeft(25, 5);
    //goForward(25, 5);
    //leverArm.SetDegree(110); // raise lever back up
}
