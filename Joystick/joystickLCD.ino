/*
 *    Operate LCD display with joystick replacing buttons
 */

#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

#define OFF 0x0
#define ON 0x1
#define joyX A0
#define joyY A1
#define joyPress 22

int xVal, yVal, button;

void setup() {
  
    Serial.begin(9600);

    pinMode(joyPress, INPUT_PULLUP);  // activate pull-up resistor for digital pin 22 (joystick select button)
     
    lcd.begin(16, 2); // set up the LCD's number of columns and rows
    lcd.print("Tree health:");
    lcd.setCursor(0,1);
    lcd.print("Wait for input..");
    lcd.setBacklight(ON);
  
}

void loop() {   // main

    xVal = analogRead(joyX);
    yVal = analogRead(joyY);
    
    button = digitalRead(joyPress);

    stickSelect(xVal, yVal, button);

}

/* function to select output using joystick */
void stickSelect(int xVal, int yVal, int button){

    lcd.setCursor(0,1); // set cursor position to start of lower line

    if(!button){  // stick press, flashes the text at 250 ms intervals
      lcd.clear();
      delay(250);
      lcd.home();
      lcd.print("Tree health:");
      lcd.setCursor(0,1);
      lcd.print("Wait for input..");
    }    
    if( ((xVal > 300) && (xVal < 700)) && ((yVal > 300) && (yVal < 700)) ){
      lcd.print("Wait for input..");
    }
    if(xVal < 300){ // stick left
      lcd.print("Tree is dead :( ");
    }
    if(xVal > 700){ // stick right
      lcd.print("Need nutrients  ");
    }
    if(yVal < 300){ // stick up
      lcd.print("Need water      ");
    }
    if(yVal > 700){ // stick down
      lcd.print("Looks good      ");
    }
    
}
