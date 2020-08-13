

#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include "rgb_lcd.h"

/**************** WEB SERVER SYSTEM ****************************/
// MAC address from Ethernet shield sticker on board
byte mac[] = { 0x98, 0x4F, 0xEE, 0x00, 0x2C, 0xDF };
IPAddress ip(169,254,8,55); // IP address of the board
EthernetServer server(80);  // create a server at port 80, it's the default

String HTTP_req;          // stores the HTTP request
boolean LED_status = 0;   // state of LED, off by default


/**************** MORSE CODE SYSTEM ***************************/
//hardware constants
rgb_lcd lcd;
const int colorR = 66;
const int colorG = 241;
const int colorB = 244;
const int buttonPin = 3;

// Set the timer system for the button presses
long buttonPressTime = 0;
long longPressTime = 1000;                                            //a long button press is one second, in this morse code implementation
unsigned long previousMillis = 0;                                     // will store last time LED was updated
const long interval = 10000;                                          // interval at which to decode a new morse code character (milliseconds)
long syncInterval = 0;                                                // interval for the synchronization of the server

//variables for the decoding algorithm
String morseVals = String("");                                          //where I will store each morse character that the user enters, will be passed to the loop statement
char englishChar = ' ';                                                 

//Text state of the LCD
int morseColumn = 0;
int englishColumn = 0;
const int MORSE_ROW = 0;
const int ENGLISH_ROW = 1;

// Set the state system of the button presses
boolean isButtonActive = false;                                                                   // for telling if the button is being pressed so that we only have to use digitalRead() once
boolean isLongPressActive = false;



void setup()
{
    Ethernet.begin(mac, ip);  // initialize Ethernet device
    server.begin();           // start to listen for clients
    Serial.begin(9600);       // for diagnostics
    
    pinMode(LED_BUILTIN, OUTPUT);       // On board LED 

    // set up the LCD
    lcd.begin(16, 2);
    lcd.setRGB(colorR, colorG, colorB);
    //lcd.cursor();                                                                                       //makes the cursor where the next character will be printed at, but only useful for debugging as it looks too much like a dah
    lcd.setCursor(0, 0);
}

void loop()
{
    EthernetClient client = server.available();  // try to get client

    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read                                                TODO: CHECK THE SYNCHING INTERVAL HERE
                char c = client.read(); // read 1 byte (character) from client
                HTTP_req += c;  // save the HTTP request 1 char at a time
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");
                    client.println();
                    // send web page
                    client.println("<!DOCTYPE html>");
                    client.println("<html>");
                    client.println("<head>");
                    client.println("<title>Arduino Morse Code Control</title>");
                    client.println("<meta name='viewport' content='user-scalable=no'/>");
                    client.println("</head>");
                    client.println("<body>");
                    client.println("<h1>Morse Code Decoding</h1>");
                    client.println("<p>Click to send Morse Code</p>");
                    client.println("<form action='' method='get'>");
                    client.println("<input style='height:20%;width:40%;font-size:70px' name='_' type='submit' value='_' />");
                    client.println("<input style='height:20%;width:40%;font-size:70px' name='.' type='submit' value='.' />");
                    webMorseDecoder(client);
                    client.println("</form>");
                    client.println("</body>");
                    client.println("</html>");
                    Serial.print(HTTP_req);
                    HTTP_req = "";    // finished with request, empty string
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)

        
  //enable sending dits and dahs with the pushbuttons, separate from the web page also
  morseDecoder();

}


void morseDecoder() {
  if (digitalRead(buttonPin) == HIGH) {                                                               // if the button is being pressed ...
    if (isButtonActive == false) {                                                                    // if the last saved state of the button was set to false i.e it's the first run of the program ...
      isButtonActive = true;                                                                          // set it to true
      buttonPressTime = millis();                                                                     // save the time when the button was pressed  
    }
    if ((millis() - buttonPressTime > longPressTime) && (isLongPressActive == false)) {               // if the current time - the time the button was pressed is the long button press time
                                                                                                      // AND the last saved state of the Long button press is set to false (to avoid recording two long presses)
      isLongPressActive = true;                                                                       // set the state of isLongPressActive to true
      lcd.setCursor(morseColumn++,MORSE_ROW);                                                                             // specify the next column in the "morse row"
      lcd.print('_');                                                                                 // print a dah
      morseVals = morseVals + "_";
    }
  } 
  
  else {                                                                                                // happens if the long press case failed
    if (isButtonActive == true) {                                                                       // if the button is being pressed
      if (isLongPressActive == true) {                                                                  // if the long press state is set to true ...
        isLongPressActive = false;                                                                      // set it to false
      } 
      else {
        lcd.setCursor(morseColumn++,MORSE_ROW);
        lcd.print('.');                                                                                 // print a dit
        morseVals = morseVals + ".";
      }
      isButtonActive = false;                                                                           // set button active to false (we don't check how short a short press is)
    }
  }


  //it is my understanding that the character timer does not depend on whether or not a button was presses, just time so save the time for each interval
  unsigned long currentMillis = millis();

  if ((currentMillis - previousMillis >= interval) || (morseVals.length() >= 5) ) {                   //TODO: this if statement works as a delay right now, but I should I should actually reset the timer whenever the user presses the button
    // save the last time you printed an english character
    previousMillis = currentMillis;

    // print the character now and move up one row once you finish
    lcd.setCursor(morseColumn, MORSE_ROW);                                                                                  
    //declare the big switch case here
    if(morseVals.equals("._")){
      englishChar = 'A';
    }
     else if(morseVals.equals("_...")) {
      englishChar = 'B';
     }
     else if(morseVals.equals("_._.")) {
      englishChar = 'C';
     }
     else if(morseVals.equals("_..")) {
      englishChar = 'D';
     }
     else if(morseVals.equals(".")) {
      englishChar = 'E';
     }
     else if(morseVals.equals(".._.")) {
      englishChar = 'F';
     }
     else if(morseVals.equals("__.")) {
      englishChar = 'G';
     }
     else if(morseVals.equals("....")) {
      englishChar = 'H';
     }
     else if(morseVals.equals("..")) {
      englishChar = 'I';
     }
     else if(morseVals.equals(".___")) {
      englishChar = 'J';
     }
     else if(morseVals.equals("_._")) {
      englishChar = 'K';
     }
     else if(morseVals.equals("._..")) {
      englishChar = 'L';
     }
     else if(morseVals.equals("__")) {
      englishChar = 'M';
     }
     else if(morseVals.equals("_.")) {
      englishChar = 'N';
     }
     else if(morseVals.equals("___")) {
      englishChar = 'O';
     }
     else if(morseVals.equals(".__.")) {
      englishChar = 'P';
     }
     else if(morseVals.equals("__._")) {
      englishChar = 'Q';
     }
     else if(morseVals.equals("._.")) {
      englishChar = 'R';
     }
     else if(morseVals.equals("...")) {
      englishChar = 'S';
     }
     else if(morseVals.equals("_")) {
      englishChar = 'T';
     }
     else if(morseVals.equals(".._")) {
      englishChar = 'U';
     }
     else if(morseVals.equals("..._")) {
      englishChar = 'V';
     }
     else if(morseVals.equals(".__")) {
      englishChar = 'W';
     }
     else if(morseVals.equals("_.._")) {
      englishChar = 'X';
     }
     else if(morseVals.equals("_.__")) {
      englishChar = 'Y';
     }
     else if(morseVals.equals("__..")) {
      englishChar = 'Z';
     }
     else if(morseVals.equals(".____")) {
      englishChar = '1';
     }
     else if(morseVals.equals("..___")) {
      englishChar = '2';
     }
     else if(morseVals.equals("...__")) {
      englishChar = '3';
     }
     else if(morseVals.equals("...._")) {
      englishChar = '4';
     }
     else if(morseVals.equals(".....")) {
      englishChar = '5';
     }
     else if(morseVals.equals("_....")) {
      englishChar = '6';
     }
     else if(morseVals.equals("__...")) {
      englishChar = '7';
     }
     else if(morseVals.equals("____..")) {
      englishChar = '8';
     }
     else if(morseVals.equals("____.")) {
      englishChar = '9';
     }
     else if(morseVals.equals("_____")) {
      englishChar = '0';
     }    
    else {
      // in this implementation of a morse code device, no valid input within the 10 second counter means that we must print a space character. 
      englishChar = ' ';
    }
    //Reset the decoding algorithm dependencies and print the English character
    morseVals = "";
    lcd.setCursor(englishColumn++,ENGLISH_ROW);
    lcd.print(englishChar);
    resetMorseRow();    
  }

}


// MORSE CODE SYSTEM INDEPENDENT:helper function for reseting the first row
void resetMorseRow() {
  for(int i = 0; i < 5; i++){
    lcd.setCursor(i, MORSE_ROW);
    lcd.print(" ");
  }
  morseColumn = 0;
  lcd.setCursor(morseColumn, MORSE_ROW);
}

//MORSE CODE SYSTEM AND WEB SERVER dependent: use handle Morse Code decoding from the pushbutton on the Intel Galieleo board and display the current morse code characters onto the page
void webMorseDecoder(EthernetClient cl) {
  

   if (HTTP_req.indexOf("_=_") > -1){
      lcd.setCursor(morseColumn++,MORSE_ROW);                                                                             // specify the next column in the "morse row"
      lcd.print('_');                                                                                 // print a dah
      morseVals = morseVals + "_";
      //webpage.send(morseVals);
    } 
  else if(HTTP_req.indexOf(".=.") > -1) {    
    lcd.setCursor(morseColumn++,MORSE_ROW);
    lcd.print('.');                                                                                 // print a dit
     morseVals = morseVals + ".";
     //webpage.send(morseVals);
  }
  

  unsigned long currentMillis = millis();

  if ((currentMillis - previousMillis >= interval) || (morseVals.length() >= 5) ) {                   //TODO: this if statement works as a delay right now, but I should I should actually reset the timer whenever the user presses the button
    // save the last time you printed an english character
    previousMillis = currentMillis;
 

    lcd.setCursor(morseColumn, MORSE_ROW);  
    //TODO: display the morse code string into the web page here
    //cl.println(morseVals);                                                                                     
    //declare the big switch case here
    if(morseVals.equals("._")){
      englishChar = 'A';
    }
     else if(morseVals.equals("_...")) {
      englishChar = 'B';
     }
     else if(morseVals.equals("_._.")) {
      englishChar = 'C';
     }
     else if(morseVals.equals("_..")) {
      englishChar = 'D';
     }
     else if(morseVals.equals(".")) {
      englishChar = 'E';
     }
     else if(morseVals.equals(".._.")) {
      englishChar = 'F';
     }
     else if(morseVals.equals("__.")) {
      englishChar = 'G';
     }
     else if(morseVals.equals("....")) {
      englishChar = 'H';
     }
     else if(morseVals.equals("..")) {
      englishChar = 'I';
     }
     else if(morseVals.equals(".___")) {
      englishChar = 'J';
     }
     else if(morseVals.equals("_._")) {
      englishChar = 'K';
     }
     else if(morseVals.equals("._..")) {
      englishChar = 'L';
     }
     else if(morseVals.equals("__")) {
      englishChar = 'M';
     }
     else if(morseVals.equals("_.")) {
      englishChar = 'N';
     }
     else if(morseVals.equals("___")) {
      englishChar = 'O';
     }
     else if(morseVals.equals(".__.")) {
      englishChar = 'P';
     }
     else if(morseVals.equals("__._")) {
      englishChar = 'Q';
     }
     else if(morseVals.equals("._.")) {
      englishChar = 'R';
     }
     else if(morseVals.equals("...")) {
      englishChar = 'S';
     }
     else if(morseVals.equals("_")) {
      englishChar = 'T';
     }
     else if(morseVals.equals(".._")) {
      englishChar = 'U';
     }
     else if(morseVals.equals("..._")) {
      englishChar = 'V';
     }
     else if(morseVals.equals(".__")) {
      englishChar = 'W';
     }
     else if(morseVals.equals("_.._")) {
      englishChar = 'X';
     }
     else if(morseVals.equals("_.__")) {
      englishChar = 'Y';
     }
     else if(morseVals.equals("__..")) {
      englishChar = 'Z';
     }
     else if(morseVals.equals(".____")) {
      englishChar = '1';
     }
     else if(morseVals.equals("..___")) {
      englishChar = '2';
     }
     else if(morseVals.equals("...__")) {
      englishChar = '3';
     }
     else if(morseVals.equals("...._")) {
      englishChar = '4';
     }
     else if(morseVals.equals(".....")) {
      englishChar = '5';
     }
     else if(morseVals.equals("_....")) {
      englishChar = '6';
     }
     else if(morseVals.equals("__...")) {
      englishChar = '7';
     }
     else if(morseVals.equals("____..")) {
      englishChar = '8';
     }
     else if(morseVals.equals("____.")) {
      englishChar = '9';
     }
     else if(morseVals.equals("_____")) {
      englishChar = '0';
     }    
    else {
      // in this implementation of a morse code device, no valid input within the 10 second counter means that we must print a space character. 
      englishChar = ' ';
    }
    //TODO: display the english character onto the web page here.

    
    //Reset the decoding algorithm dependencies and print the English character
    morseVals = "";
    lcd.setCursor(englishColumn++,ENGLISH_ROW);
    lcd.print(englishChar);
    cl.print("<h1>");
    cl.print(englishChar);
    cl.print("</h1>");
    resetMorseRow();
  }    
}




