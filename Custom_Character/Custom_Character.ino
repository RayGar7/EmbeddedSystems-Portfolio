/*
  CustomCharacter.ino
  2013 Copyright (c) Seeed Technology Inc.  All right reserved.
  Author:Loovee
  2013-9-18
  Grove - Serial LCD RGB Backlight demo.
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

// make some custom characters:
byte heart[8] = {
    0b00000,
    0b01010,
    0b11111,
    0b11111,
    0b11111,
    0b01110,
    0b00100,
    0b00000
};

byte smiley[8] = {
    0b00000,
    0b00000,
    0b01010,
    0b00000,
    0b00000,
    0b10001,
    0b01110,
    0b00000
};

byte frownie[8] = {
    0b00000,
    0b00000,
    0b01010,
    0b00000,
    0b00000,
    0b00000,
    0b01110,
    0b10001
};

byte armsDown[8] = {
    0b00100,
    0b01010,
    0b00100,
    0b00100,
    0b01110,
    0b10101,
    0b00100,
    0b01010
};

byte armsUp[8] = {
    0b00100,
    0b01010,
    0b00100,
    0b10101,
    0b01110,
    0b00100,
    0b00100,
    0b01010
};

byte orb[8] = {
    0b00100,
    0b01010,
    0b11011,
    0b10101,
    0b10101,
    0b11011,
    0b01010,
    0b00100
};


byte leftMouth[8] = {
    0b00000,
    0b00000,
    0b00100,
    0b00110,
    0b00011,
    0b00000,
    0b00000,
    0b00000
};

byte rightMouth[8] = {
    0b00000,
    0b00000,
    0b00100,
    0b01100,
    0b11000,
    0b00000,
    0b00000,
    0b00000
};

byte bigR[8] = {
    0b11111,
    0b10001,
    0b10001,
    0b11111,
    0b11000,
    0b10100,
    0b10010,
    0b10001
};

byte bigE[8] = {
    0b11111,
    0b10000,
    0b10000,
    0b11111,
    0b10000,
    0b10000,
    0b10000,
    0b11111
};

byte bigA[8] = {
    0b11111,
    0b10001,
    0b10001,
    0b11111,
    0b10001,
    0b10001,
    0b10001,
    0b10001
};

void setup() 
{

    lcd.begin(16, 2);
 #if 1   
    // create a new character
    lcd.createChar(0, orb);
    // create a new character
    lcd.createChar(1, smiley);
    // create a new character
    lcd.createChar(2, bigR);
    // create a new character
    lcd.createChar(3, bigA);
    // create a new character
    lcd.createChar(4, bigE);
    //create a mouth
    lcd.createChar(5, leftMouth);
    lcd.createChar(6, rightMouth);
    //lcd.createChar(7, bigR);
    //lcd.createChar(8, bigE);
    //lcd.createChar(9, bigA);
    
    
#endif 
    // set up the lcd's number of columns and rows:
    
    
    lcd.setCursor(0, 0);
    // Print a message to the lcd.
    lcd.write((unsigned char)0);
    lcd.write((unsigned char)0);
    lcd.print(" Arduino! ");
    lcd.write(1);
    lcd.setCursor(0,1);
    lcd.write((unsigned char)5);
    lcd.write((unsigned char)6);
    
    
    lcd.write((unsigned char)2);
    lcd.write((unsigned char)3);
    lcd.write((unsigned char)4);
    //lcd.write((unsigned char)10);
    
}

void loop() 
{
    // read the potentiometer on A0:
    int sensorReading = analogRead(A0);
    // map the result to 200 - 1000:
    int delayTime = map(sensorReading, 0, 1023, 200, 1000);
    // set the cursor to the bottom row, 8th position:
    lcd.setCursor(7, 1);
    // draw the first frame of the animation, an A
    lcd.write(3);
    delay(delayTime);
    lcd.setCursor(7, 1);
    // draw animation
    lcd.write(4);
    delay(delayTime);
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
