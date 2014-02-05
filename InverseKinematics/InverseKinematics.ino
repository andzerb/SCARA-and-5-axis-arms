/*
This program controls a robot arm like that discussed at the link below.
http://projectsfromtech.blogspot.com/2013/09/simple-arduino-robot-arm-from-9-gram.html

Position is calculated using simple inverse kinematics based on high school trig

Setup:
* Change arm constants to match physical setup (distance between pivot points in cm)
* Change servo correction factors to match physical setup
* Change origin correction factors to match desired origin

Use:
* Enter (x,y) coordinates in FixCoordinate(x,y). Distances are in cm and are measured from origin
* Call CalculateServoAngles()
* Call MoveArm()

Author: Matthew
http://projectsfromtech.blogspot.com/
9/2/2013

*/


#include <Servo.h>
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

Servo ServoS_1;      // Joint at base
Servo ServoS_2;      // Elbow between a and b

// Servo Angles
float ServoS_1_Angle = 90;
float ServoS_2_Angle = 90;

// Define arm Constants
const float a = 14.5;      // lower joint length (cm)
const float b = 11.5;      // upper joint length (cm)

// Correction factors to align servo values with their respective axis
const float S_1_CorrectionFactor = -10;     // Align arm "a" with the horizontal when at 0 degrees
const float S_2_CorrectionFactor = -77;     // Align arm "b" with arm "a" when at 0 degrees

// Correction factor to shift origin out to edge of the mount
const float X_CorrectionFactor = 6.5;       // X direction correction factor (cm)
const float Y_CorrectionFactor = -4;       // Y direction correction factor (cm)

// Angle Variables
float A;            //Angle oppposite side a (between b and c)
float B;            //Angle oppposite side b
float C;            //Angle oppposite side c
float theta;        //Angle formed between line from origin to (x,y) and the horizontal

// Distance variables
float x;            // x position (cm)
float y;            // y position (cm)
float c;            // Hypotenuse legngth in cm
const float pi = M_PI;  //Store pi in a less annoying format

//===================================================================================

void setup()
{
  ServoS_1.attach(9);             // Attach servos
  ServoS_2.attach(10);
  Serial.begin(9600);             // -For debugging
}
//--------------------------------------------------------------


void loop()
{
  if (stringComplete) {
    Serial.println(inputString);
    String strData = inputString;
    int nComma = strData.indexOf(',');
    // Get the characters up to, but not including the comma
    String xString = strData.substring(0, nComma);
    // Get the characters after the comma
    String yString = strData.substring(nComma+1);
    // clear the string:
    int xdeg = xString.toInt();
    int ydeg = yString.toInt();
   }
  FixCoordinates(xdeg, ydeg);           // Enter coordinates of point.
  CalculateServoAngles();           // Calculate necessary angles of servos
  MoveArm();                        // Move arm to new position
  delay(30);
}

//====================================================================================

// Get x and y measured from the bottom of the base. Function corrects for offset
void FixCoordinates(float x_input, float y_input)
{
 x = x_input + X_CorrectionFactor;
 y = y_input + Y_CorrectionFactor;
}

// Calculate necessary servo angles to move arm to desired points
void CalculateServoAngles()
{
  c = sqrt( sq(x) + sq(y) );                                            // pythagorean theorem
  B = (acos( (sq(b) - sq(a) - sq(c))/(-2*a*c) )) * (180/pi);            // Law of cosines: Angle opposite upper arm section
  C = (acos( (sq(c) - sq(b) - sq(a))/(-2*a*b) )) * (180/pi);            // Law of cosines: Angle opposite hypotenuse
  theta = (asin( y / c )) * (180/pi);                                   // Solve for theta to correct for lower joint's impact on upper joint's angle
  ServoS_1_Angle = B + theta + S_1_CorrectionFactor;                    // Find necessary angle. Add Correction
  ServoS_2_Angle = C + S_2_CorrectionFactor;                            // Find neceesary angle. Add Correction

}

// Update the servos
void MoveArm()
{
  ServoS_1.write(ServoS_1_Angle);              // Move joint to desired position
  ServoS_2.write(ServoS_2_Angle);              // Move joint to desired position
}
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

