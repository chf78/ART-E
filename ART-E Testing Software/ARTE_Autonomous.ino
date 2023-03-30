
#include <MeMegaPi.h>
//#include <Wire.h>
//#include <SoftwareSerial.h>

/* directions */
#define FORWARD   1
#define BACKWARD  2
#define LEFT      3
#define RIGHT     4

/* state names for autonomous driving */
#define CLEAR       0
#define OBSTACLE    1
#define CHECK_LEFT  2
#define CHECK_RIGHT 3
#define RECENTER    4
#define GO_LEFT     5
#define GO_RIGHT    6
#define TURN_AROUND 7

uint8_t pathState;
volatile uint8_t leftDist, rightDist;

/* encoder motor on slot 1 */
MeEncoderOnBoard Encoder_1(SLOT_1);
MeEncoderOnBoard Encoder_2(SLOT_2);

MeUltrasonicSensor ultraSensor(PORT_7);

void isr_process_encoder1(){
  if(digitalRead(Encoder_1.getPortB()) == 0){
    Encoder_1.pulsePosMinus();
  }else{
    Encoder_1.pulsePosPlus();
  }
}

void isr_process_encoder2(){
  if(digitalRead(Encoder_2.getPortB()) == 0){
    Encoder_2.pulsePosMinus();
  }else{
    Encoder_2.pulsePosPlus();
  }
}

void TimerPWM_init() {

  //Set PWM 8KHz
  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(CS11) | _BV(WGM12);

  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS21);
  
}

void setup() {

  // encoder motor setup
  attachInterrupt(Encoder_1.getIntNum(), isr_process_encoder1, RISING);
  attachInterrupt(Encoder_2.getIntNum(), isr_process_encoder2, RISING);
  TimerPWM_init();
  Serial.begin(115200);

  pathState = CLEAR;
}

void move(int direction, int speed){
  int leftSpeed = 0;
  int rightSpeed = 0;
  speed = (speed/100.0)*255;
  if(direction == FORWARD){
    leftSpeed = -1 * speed;
    rightSpeed = speed;
  }else if(direction == BACKWARD){
    leftSpeed = speed;
    rightSpeed = -1 * speed;
  }else if(direction == LEFT){
    leftSpeed = speed;
    rightSpeed = speed;
  }else if(direction == RIGHT){
    leftSpeed = -1 * speed;
    rightSpeed = -1 * speed;
  }
  Encoder_1.setMotorPwm(rightSpeed);
  Encoder_2.setMotorPwm(leftSpeed);
}

void Stop(){
  Encoder_1.setMotorPwm(0);
  Encoder_2.setMotorPwm(0);
}

void checkLeft(){
  move(LEFT, 50);
  delay(1000);
  Stop();
  delay(50);
  leftDist = ultraSensor.distanceCm();
}

void checkRight(){
  move(RIGHT, 50);
  delay(2100);
  Stop();
  delay(50);
  rightDist = ultraSensor.distanceCm();
}

void Center(){
  move(LEFT, 50);
  delay(900);
  Stop();
  //delay(2500);
}

void turnAround(){
  move(LEFT, 50);
  delay(2500);
  Stop();
  delay(2500);
}

void autoDrive(){
  int minDistance = 20; // minimum distance in cm for detecting obstacles
  switch(pathState){
    case CLEAR:
      move(FORWARD, 50);
      if(ultraSensor.distanceCm() <= minDistance){pathState = OBSTACLE;}
    break;
    case OBSTACLE:
      Stop();
      delay(250);
      move(BACKWARD, 50);
      delay(500);
      Stop();
      pathState = CHECK_LEFT;
    break;
    case CHECK_LEFT:
      checkLeft();
      pathState = CHECK_RIGHT;
    break;
    case CHECK_RIGHT:
      checkRight();
      pathState = RECENTER;
    break;
    case RECENTER:
      Center();
      if((leftDist <= minDistance) && (rightDist <= minDistance)){pathState = TURN_AROUND;}
      if(leftDist >= rightDist){pathState = GO_LEFT;}
      if(rightDist > leftDist){pathState = GO_RIGHT;}
    break;
    case GO_LEFT:
      checkLeft();
      pathState = CLEAR;
    break;
    case GO_RIGHT:
      checkRight();
      pathState = CLEAR;
    break;
    case TURN_AROUND:
      turnAround();
      pathState = CLEAR;
    break;
  }
  
}

void loop() {

  autoDrive();
  
  Encoder_1.loop();
  Encoder_2.loop();

}
