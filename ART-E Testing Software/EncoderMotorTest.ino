/**
 * \par Copyright (C), 2012-2016, MakeBlock
 * @file    EncoderMotorTestRunSpeed.ino
 * @author  MakeBlock
 * @version V1.0.0
 * @date    2015/11/19
 * @brief   Description: this file is sample code for Encoder Motor  device.
 *
 * Function List:
 *
 *    1. void MeEncoderMotor::begin();
 *    2. boolean MeEncoderMotor::runSpeed(float speed);
 *
 * \par History:
 * <pre>
 * <Author>     <Time>        <Version>      <Descr>
 * forfish      2015/11/19    1.0.0          add some descriptions
 * </pre>
 */

#include <MeMegaPi.h>

/* define directions for readability */
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

/* timing variables */
unsigned long startTime, currentTime, duration = 6000;

const uint8_t StickPin = 2;
volatile uint8_t PressFlag = 0;

uint8_t speedInput;
uint16_t stickPos, posX, posY;

/* encoder motor on slot 1 */
MeEncoderOnBoard Encoder_1(SLOT_1);
MeEncoderOnBoard Encoder_2(SLOT_2);
MeEncoderOnBoard Encoder_3(SLOT_3);

void isr_process_encoder1(void){
  if(digitalRead(Encoder_1.getPortB()) == 0){
    Encoder_1.pulsePosMinus();
  }else{
    Encoder_1.pulsePosPlus();
  }
}

void isr_process_encoder2(void){
  if(digitalRead(Encoder_2.getPortB()) == 0){
    Encoder_2.pulsePosMinus();
  }else{
    Encoder_2.pulsePosPlus();
  }
}

void isr_process_encoder3(void){
  if(digitalRead(Encoder_3.getPortB()) == 0){
    Encoder_3.pulsePosMinus();
  }else{
    Encoder_3.pulsePosPlus();
  }
}

void TimerPWM_init() {

  // set up 8 kHz PWM
  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(CS11) | _BV(WGM12);
  
  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS21);  
  
}

void move(int direction, int speed){
  int leftSpeed = 0;
  int rightSpeed = 0;
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
  Serial.println(direction);
  
}

void setup()
{
//  // external interrupt setup for joystick press
//  pinMode(StickPin, INPUT_PULLUP);
//  attachInterrupt(digitalPinToInterrupt(StickPin), ISR_Press, RISING);

  // encoder motor setup
  attachInterrupt(Encoder_1.getIntNum(), isr_process_encoder1, RISING);
  attachInterrupt(Encoder_2.getIntNum(), isr_process_encoder2, RISING);
  attachInterrupt(Encoder_3.getIntNum(), isr_process_encoder3, RISING);
  
  TimerPWM_init();
  
  Serial.begin(115200);
}

void loop()
{
  // do i need to add some flags???
  /*
  currentTime = millis();
  if(currentTime - startTime >= duration){
    direction = STOP;
  }

  armMove(direction);
  */

  armMove(DOWN);
  delay(2500);
  armMove(STOP);
  delay(2000);
  armMove(UP);
  delay(2500);
  armMove(STOP);
  Serial.println("Cycle complete.");
  delay(5000);
  

//  if(Serial.available()){
//    char armDirection = Serial.read();
//    armMove(armDirection);
//  }
  
  Encoder_3.loop();
    
}

void EncoderSweep() {

  uint8_t pwm;
  for(pwm = 10; pwm <= 90; pwm++) {
    Encoder_1.setMotorPwm(pwm/100.0*255);
    Encoder_2.setMotorPwm(pwm/100.0*255);
    Encoder_1.updateSpeed();
    Encoder_2.updateSpeed();
    delay(50);
  }
  for(pwm = 90; pwm >= 10; pwm--) {
    Encoder_1.setMotorPwm(pwm/100.0*255);
    Encoder_2.setMotorPwm(pwm/100.0*255);
    Encoder_1.updateSpeed();
    Encoder_2.updateSpeed();
    delay(50);
  }
  Encoder_1.setMotorPwm(0);
  Encoder_2.setMotorPwm(0);
  Encoder_1.updateSpeed();
  Encoder_2.updateSpeed();
}



void ISR_Press(void){

  static unsigned long lastIntTime = 0;
  unsigned long IntTime = millis();
  if (IntTime - lastIntTime > 200)  // debouncing for switch 
  {
    PressFlag = 1;
  }
  lastIntTime = IntTime;
  
}
