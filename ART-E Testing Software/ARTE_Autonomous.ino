
#include <MeMegaPi.h>

/* driving directions */
#define FORWARD   1
#define BACKWARD  2
#define LEFT      3
#define RIGHT     4

/* arm directions */
#define STOP  0
#define UP    1
#define DOWN  2

/* fixed arm speed */
const uint8_t armSpeed = (50/100.0)*255;

/* state names for autonomous driving */
#define CLEAR       0
#define OBSTACLE    1
#define SCAN        2
#define CHECK_LEFT  3
#define CHECK_RIGHT 4
#define TURN_AROUND 5

uint8_t pathState;
volatile uint8_t leftDist, rightDist;

/* encoder motors on slots 1, 2, and 3 */
MeEncoderOnBoard Encoder_1(SLOT_1);
MeEncoderOnBoard Encoder_2(SLOT_2);
MeEncoderOnBoard Encoder_3(SLOT_3);

MeUltrasonicSensor ultraSensor(PORT_7);

/********* interrupt handlers for encoder motors ***********/
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

void isr_process_encoder3(){
  if(digitalRead(Encoder_3.getPortB()) == 0){
    Encoder_3.pulsePosMinus();
  }else{
    Encoder_3.pulsePosPlus();
  }
}
/******************************************************/

/******** pulse width modulation setup ****************/
void TimerPWM_init() {

  //Set PWM 8KHz
  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(CS11) | _BV(WGM12);

  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS21);
  
}
/**************************************************/

void setup() {

  // encoder motor setup
  attachInterrupt(Encoder_1.getIntNum(), isr_process_encoder1, RISING);
  attachInterrupt(Encoder_2.getIntNum(), isr_process_encoder2, RISING);
  attachInterrupt(Encoder_3.getIntNum(), isr_process_encoder3, RISING);
  
  TimerPWM_init();
  
  Serial.begin(115200);

  pathState = CLEAR;
  
}

/************* driving functions ******************/
void move(int direction, int speed){
  int leftSpeed = 0;
  int rightSpeed = 0;
  speed = (speed/100.0)*255;
  if(direction == FORWARD){
    leftSpeed = -1*speed;
    rightSpeed = speed;
  }else if(direction == BACKWARD){
    leftSpeed = speed;
    rightSpeed = -1*speed;
  }else if(direction == LEFT){
    leftSpeed = speed;
    rightSpeed = speed;
  }else if(direction == RIGHT){
    leftSpeed = -1*speed;
    rightSpeed = -1*speed;
  }
  Encoder_1.setMotorPwm(rightSpeed);
  Encoder_2.setMotorPwm(leftSpeed);
}

void Stop(){
  Encoder_1.setMotorPwm(0);
  Encoder_2.setMotorPwm(0);
}

void armMove(uint8_t direction){
  
  switch(direction){
    case STOP:
      Encoder_3.setMotorPwm(STOP);
    break;
    case DOWN:
      Encoder_3.setMotorPwm(-1*armSpeed);
    break;
    case UP:
      Encoder_3.setMotorPwm(armSpeed);
    break;
  }
  
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

void turnAround(){
  move(LEFT, 50);
  delay(2500);
  Stop();
  delay(2500);
}

/***************** finite state machine **********************/
void autoDrive(){
  int minDistance = 15; // minimum distance in cm for detecting obstacles
  switch(pathState){
    case CLEAR:
      move(FORWARD, 50);
      if(ultraSensor.distanceCm() <= minDistance){pathState = OBSTACLE;}
    break;
    case OBSTACLE:
      Stop();
      delay(250);
      move(BACKWARD, 50);
      delay(250);
      Stop();
      pathState = SCAN;
    break;
    case SCAN:
      armMove(DOWN);
      delay(2500);
      armMove(STOP);
      delay(500);
      armMove(UP);
      delay(2500);
      armMove(STOP);
      pathState = CHECK_LEFT;
    break;
    case CHECK_LEFT:
      checkLeft();
      if(leftDist <= 20){pathState = CHECK_RIGHT;}
      if(leftDist > 20){pathState = CLEAR;}
    break;
    case CHECK_RIGHT:
      checkRight();
      if(rightDist <= 20){pathState = TURN_AROUND;}
      if(rightDist > 20){pathState = CLEAR;}
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
