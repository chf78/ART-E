/*
 *    Control servo using joystick input
 */


#include <Servo.h>

Servo servo;

#define joyX A0
#define joyY A1

int xValue, yValue, stickPos;
 
void setup() {
  servo.attach(22);   // attach servo to digital pin 22
  Serial.begin(9600);
}
 
void loop() {
  
  xValue = analogRead(joyX);
  yValue = analogRead(joyY);

  stickPos = map(xValue, 0, 1023, 10, 180);     // scale it to use it with the servo (value between 0 and 180)
  servo.write(stickPos);                  // sets the servo position according to the scaled value
  
  Serial.print(xValue);
  Serial.print("\t");
  Serial.println(stickPos);
  
  delay(15);                           // waits for the servo to get there
 
}
