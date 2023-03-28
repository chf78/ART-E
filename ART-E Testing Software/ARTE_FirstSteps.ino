
#include <MeMegaPi.h>
#include <Wire.h>
#include <SoftwareSerial.h>


/* encoder motor on slot 1 */
MeEncoderOnBoard Encoder_1(SLOT_1);
MeEncoderOnBoard Encoder_2(SLOT_2);

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

void TimerPWM_init() {

  // set up 8 kHz PWM
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

}

void move(int direction, int speed){
  int leftSpeed = 0;
  int rightSpeed = 0;
  if(direction == 1){
    leftSpeed = -1 * speed;
    rightSpeed = speed;
  }else if(direction == 2){
    leftSpeed = speed;
    rightSpeed = -1 * speed;
  }else if(direction == 3){
    leftSpeed = speed;
    rightSpeed = speed;
  }else if(direction == 4){
    leftSpeed = -1 * speed;
    rightSpeed = -1 * speed;
  }
  Encoder_1.setMotorPwm(rightSpeed);
  Encoder_2.setMotorPwm(leftSpeed);
  Encoder_1.updateSpeed();
  Encoder_2.updateSpeed();
  //Encoder_1.loop();
  //Encoder_2.loop();
}

void loop() {

  // forward = 1, backward = 2, left = 3, right = 4
  move(1, 50 / 100.0 * 255);
  delay(3000);
  move(1, 0);
  delay(500);
  
  move(2, 50 / 100.0 * 255);
  delay(3000);
  move(1, 0);
  delay(500);
  
  move(3, 50 / 100.0 * 255);
  delay(3000);
  move(1, 0);
  delay(500);
  
  move(4, 50 / 100.0 * 255);
  delay(3);
  move(1, 0);
  delay(500);

  //Encoder_1.loop();
  //Encoder_2.loop();

}
