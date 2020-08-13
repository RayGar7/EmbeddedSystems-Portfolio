//Note: this code was written using the header file library
//and the github repository's code samples. Both credits to 
//this belong to Seeed and their employees.

#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

const int colorR = 60;
const int colorG = 255;
const int colorB = 80;


void setup()
{
   // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  
  lcd.setRGB(colorR, colorG, colorB);
  
  lcd.print("Hello world");
}

void loop()
{


 
}



