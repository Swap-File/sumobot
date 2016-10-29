
#include <SPI.h>
#include "RF24.h"
#include "printf.h"
RF24 radio(9, 10);

void setup() {
  Serial.begin(115200);
  Serial.print("1");

  pinMode(8, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  printf_begin();
  radio.begin();
  radio.setPALevel(RF24_PA_HIGH); //lowest power mode
  Serial.print("2");
  radio.setAutoAck(0);
  radio.setDataRate(RF24_250KBPS); //slowest speed
  radio.setPayloadSize(1);  //StationID     motor1 motor2
  Serial.print("3");
  uint8_t address[] = { 0xCC, 0xCE, 0xCC, 0xCE, 0xCC };
  radio.openWritingPipe(address);
  Serial.print("4");
  radio.printDetails();
}

void loop() {

  delay(20);

  uint8_t payload[9];
  payload[0] = 0x00;
  
  if (digitalRead(8)) bitSet(payload[0], 0);
  if (digitalRead(7)) bitSet(payload[0], 1);
  if (digitalRead(6)) bitSet(payload[0], 2);
  if (digitalRead(5)) bitSet(payload[0], 3);
  radio.write( &payload, 1 );

}
