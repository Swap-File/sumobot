
#include <Servo.h>
#include <Wire.h>
#include <ArduinoNunchuk.h>

//pin defines

#define MOTOR_A_PIN 6
#define MOTOR_B_PIN 9

ArduinoNunchuk nunchuk = ArduinoNunchuk();

Servo motorA;
Servo motorB;

int servo_last_a = -1;
int servo_last_b = -1;

//default motor speeds

#define MOTOR_A_OFF 91 // 90 is twitchy
#define MOTOR_B_OFF 90

void setup() {

  // start serial debug outpu
  pinMode(MOTOR_A_PIN, OUTPUT);
  pinMode(MOTOR_B_PIN, OUTPUT);

  motorA.attach(MOTOR_A_PIN);
  motorB.attach(MOTOR_B_PIN);

  motorA.write(MOTOR_A_OFF);
  motorB.write(MOTOR_B_OFF);

  nunchuk.init();

}

void loop() {
  //delay(10);
  nunchuk.update();

  int servo_requested_b = servo_last_b;
  int servo_requested_a = servo_last_a;

  if (nunchuk.zButton && !nunchuk.cButton) {

    if (nunchuk.analogY > 64 && nunchuk.analogY < 192 ) {
      servo_requested_a = 180;
      servo_requested_b = 180;
    } else if (nunchuk.analogY <= 64 ) {
      servo_requested_a = 0;
      servo_requested_b = MOTOR_B_OFF + 16;

    } else if (nunchuk.analogY >= 192 ) {
      servo_requested_a = 180;
      servo_requested_b = MOTOR_B_OFF - 16;

    }
  }
  else if (nunchuk.cButton && !nunchuk.zButton ) {
    if (nunchuk.analogY > 64 && nunchuk.analogY < 192 ) {
      servo_requested_a = 0;
      servo_requested_b = 0;
    } else if (nunchuk.analogY <= 64) {
    servo_requested_a =  MOTOR_B_OFF - 16;
      servo_requested_b = 180 ;
    } else if (nunchuk.analogY >= 192) {

            servo_requested_a = MOTOR_A_OFF + 16;
      servo_requested_b =  0  ;
  
    }
  }
  else if (nunchuk.cButton && nunchuk.zButton ) {
    if (nunchuk.analogY > 64 && nunchuk.analogY < 192 ) {
      //default fwd
      servo_requested_a = 180;
      servo_requested_b = 0 ;
    } else if (nunchuk.analogY <= 64 ) {
      servo_requested_a = 0;
      servo_requested_b = 180 ;
    } else if (nunchuk.analogY >= 192) {
      servo_requested_a = 180;
      servo_requested_b = 0 ;
    }
  } else {
    servo_requested_a = MOTOR_A_OFF;
    servo_requested_b = MOTOR_B_OFF ;
  }

  if (servo_last_a != servo_requested_a) {
    motorA.write(servo_requested_a);
    servo_last_a = servo_requested_a;
  }
  if (servo_last_b != servo_requested_b) {
    motorB.write(servo_requested_b);
    servo_last_b = servo_requested_b;
  }

}
