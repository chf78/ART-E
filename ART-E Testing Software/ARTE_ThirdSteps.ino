#include <MeMegaPi.h>
#include <Wire.h>
#include <SoftwareSerial.h>

MeEncoderOnBoard encoders[3];

MeUltrasonicSensor ultraSensor(PORT_7);

void isr_process_encoder1(void)
{
  if(digitalRead(encoders[0].getPortB()) == 0)
  {
    encoders[0].pulsePosMinus();
  }
  else
  {
    encoders[0].pulsePosPlus();;
  }
}

void isr_process_encoder2(void)
{
  if(digitalRead(encoders[1].getPortB()) == 0)
  {
    encoders[1].pulsePosMinus();
  }
  else
  {
    encoders[1].pulsePosPlus();
  }
}

void isr_process_encoder3(void)
{
  if(digitalRead(encoders[2].getPortB()) == 0)
  {
    encoders[2].pulsePosMinus();
  }
  else
  {
    encoders[2].pulsePosPlus();
  }
}

void PWM_init() {

  //Set Pwm 970Hz
  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(CS11) | _BV(CS10) | _BV(WGM12);
  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS22);
  TCCR3A = _BV(WGM30);
  TCCR3B = _BV(CS31) | _BV(CS30) | _BV(WGM32);
  
}

/**************** Encoder Motor Functions ************************/
void Forward(int moveSpeed){
  
  encoders[0].setMotorPwm(moveSpeed);
  encoders[1].setMotorPwm(-moveSpeed);
  
}

void Backward(int moveSpeed){
  
  encoders[0].setMotorPwm(-moveSpeed);
  encoders[1].setMotorPwm(moveSpeed);
  
}

void TurnLeft(int moveSpeed){
  
  encoders[0].setMotorPwm(moveSpeed);
  encoders[1].setMotorPwm(-moveSpeed/2);
  
}

void TurnRight(int moveSpeed){
  
  encoders[0].setMotorPwm(moveSpeed/2);
  encoders[1].setMotorPwm(-moveSpeed);
  
}

void Stop(void){
  
  encoders[0].setMotorPwm(0);
  encoders[1].setMotorPwm(0);
  
}
/****************************************************/

void checkLeftRight(void){

  int leftDist, rightDist;

  // look left, then look right
  TurnLeft(25);
  delay(500);
  leftDist = ultraSensor.distanceCm();
  TurnRight(25);
  delay(1000);
  rightDist = ultraSensor.distanceCm();
  TurnLeft(25);
  delay(500);

  if((leftDist < 40) && (rightDist < 40)){
    Backward(50);
    delay(3000);
    TurnLeft(50);
    delay(2000);
  }
  if(leftDist < rightDist){
    TurnRight(50);
    delay(1000);
  }
  if(rightDist < leftDist){
    TurnLeft(50);
    delay(1000);
  }
}

void setup()
{

  for(int i=0;i<3;i++){
    encoders[i].reset(i+1);
  }
  
  attachInterrupt(encoders[0].getIntNum(), isr_process_encoder1, RISING);
  attachInterrupt(encoders[1].getIntNum(), isr_process_encoder2, RISING);
  attachInterrupt(encoders[2].getIntNum(), isr_process_encoder3, RISING);
  Serial.begin(115200);

  PWM_init();

  for(uint8_t i = 0; i < 3; i++){
    
      encoders[i].setPulse(7);
      encoders[i].setRatio(26.9);
      encoders[i].setPosPid(1.8,0,1.2);
      encoders[i].setSpeedPid(0.18,0,0);
    
  }

}

void loop() {

  for(int i=0;i<4;i++){
    encoders[i].loop();
  }

  if(ultraSensor.distanceCm() < 40){

    Stop();
    delay(1000);
    encoders[2].moveTo(1500,300);   // lower arm
    delay(3000);
    encoders[2].moveTo(0,300);  // raise arm
    delay(500);
    checkLeftRight();
    
  }
  else{

    Forward(50);
    
  }
}
