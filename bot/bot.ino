
#include <Servo.h>
#include <SPI.h>
#include "RF24.h"

RF24 radio(9, 10);

#define MOTOR_OFF 90
#define TURNING_RATE 74

#define MOTOR_A_PIN 6
#define MOTOR_B_PIN 5

int servo_last_a = -1;
int servo_last_b = -1;
int servo_requested_b = MOTOR_OFF;
int servo_requested_a = MOTOR_OFF;

Servo motorA;
Servo motorB;

void setup() {

  // start serial debug outpu
  pinMode(MOTOR_A_PIN, OUTPUT);
  pinMode(MOTOR_B_PIN, OUTPUT);

  motorA.attach(MOTOR_A_PIN);
  motorB.attach(MOTOR_B_PIN);

  motorA.write(MOTOR_OFF);
  motorB.write(MOTOR_OFF);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setAutoAck(0);
  radio.setPayloadSize(1);    //StationID  In1 In2 In3 In4  Out1 Out2 Out3 Out4
  radio.setDataRate(RF24_250KBPS);
  uint8_t address[] = { 0xCC, 0xCE, 0xCC, 0xCE, 0xCC };
  radio.openReadingPipe(0, address);
  radio.startListening();
}

uint32_t last_radio_time = 0;

void loop(void) {

  uint8_t pipeNo;
  uint8_t payload[9];
  
  while ( radio.available(&pipeNo)) {

    radio.read( &payload, 1 );

    servo_requested_b = MOTOR_OFF;
    servo_requested_a = MOTOR_OFF;

    if (bitRead(payload[0], 3) == 1 && bitRead(payload[0], 2) == 1) {
      //lever is up
      if (bitRead(payload[0], 0) == 0 &&  bitRead(payload[0], 1) == 0) {
        servo_requested_b = 0; //forwards
        servo_requested_a = 180; //forwards
      } else     if (bitRead(payload[0], 0) == 0) { //left button
        servo_requested_a = 180; //forwards
        servo_requested_b = 180; //backwards
      }
      else if (bitRead(payload[0], 1) == 0) { //right button
        servo_requested_b = 0; //forwards
        servo_requested_a = 0; //backwards
      }
    } else if (bitRead(payload[0], 3) == 1 && bitRead(payload[0], 2) == 0) {
      //lever fwd
      if (bitRead(payload[0], 0) == 0 &&  bitRead(payload[0], 1) == 0) {
        servo_requested_a = 180; //forwards
        servo_requested_b = 0; //forwards
      } else if (bitRead(payload[0], 0) == 0) { //left button
        servo_requested_a = 180; //forwards
        servo_requested_b = 90 - 4; //forwards
      }
      else if (bitRead(payload[0], 1) == 0) { //right button.
        servo_requested_a = 90 + 16; //forwards
        servo_requested_b = 0; //forwards
      }
    } else if (bitRead(payload[0], 3) == 0 && bitRead(payload[0], 2) == 1) {
      //lever rwd
      if (bitRead(payload[0], 0) == 0 &&  bitRead(payload[0], 1) == 0) {
        servo_requested_a = 0; //backwards
        servo_requested_b = 180; //backwards
      } else if (bitRead(payload[0], 0) == 0) { //left button
        servo_requested_b = 90 + 16; //backwards
        servo_requested_a = 0; //backwards
      }
      else if (bitRead(payload[0], 1) == 0) { //right button
        servo_requested_a = 90 -4; //backwards
        servo_requested_b = 180; //backwards
      }
    }
    last_radio_time = millis();
  }


  //deadmans  switch
  if (millis() - last_radio_time > 100) {
    servo_requested_b = MOTOR_OFF;
    servo_requested_a = MOTOR_OFF;
  }
  
  //do the updates
  if (servo_last_a != servo_requested_a) {
    if (servo_last_a == MOTOR_OFF) { //reattach
      motorA.attach(MOTOR_A_PIN);
    }
    if (servo_requested_a == MOTOR_OFF) {  //detatch if off to prevent jitter
      motorA.detach();
    }
    motorA.write(servo_requested_a);  
    servo_last_a = servo_requested_a;
  }
  
  if (servo_last_b != servo_requested_b) {
    if (servo_last_b == MOTOR_OFF) { //reattach
      motorB.attach(MOTOR_B_PIN);
    }
    if (servo_requested_b == MOTOR_OFF) { //detatch if off to prevent jitter
      motorB.detach();
    }
    motorB.write(servo_requested_b);
    servo_last_b = servo_requested_b;
  }

}


