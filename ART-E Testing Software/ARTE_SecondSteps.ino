
#include <MeMegaPi.h>
#include <Wire.h>
#include <SoftwareSerial.h>

/* define directions for readability */
#define FORWARD   1
#define BACKWARD  2
#define LEFT      3
#define RIGHT     4

/* encoder motor on slot 1 */
MeEncoderOnBoard Encoder_1(SLOT_1);
MeEncoderOnBoard Encoder_2(SLOT_2);

MeUltrasonicSensor ultraSensor(PORT_7); /* Ultrasonic module can ONLY be connected to port 3, 4, 6, 7, 8 of base shield. */

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
  Serial.begin(9600);
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
  Encoder_1.updateSpeed();
  Encoder_2.updateSpeed();
}


void loop() {

  move(FORWARD, 50/100.0*255);
  Serial.println(ultraSensor.distanceCm());
  if(ultraSensor.distanceCm() <= 3){
    move(FORWARD, 0);
    delay(1000);
    move(BACKWARD, 50/100.0*255);
    delay(1000);
    move(LEFT, 50/100.0*255);
    delay(2000);
  }

}
