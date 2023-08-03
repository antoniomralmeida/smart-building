//    FILE: RS485_simple_master.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: simple master
//     URL: https://github.com/RobTillaart/RS485

//  this is the code of a simple master  (needs simple slave)
//  it send one of 3 (single char) commands to the slave
//  '0' == set LED LOW
//  '1' == set LED HIGH
//  '2' == request status.
//
//  print debug messages SEND and RECV with data.
//  Note that one needs a 2nd Serial port for nice debugging.
//  (or an LCD screen whatever).


#include "Arduino.h"
#include "RS485.h"

const uint8_t sendPin = 4;
const uint8_t deviceIDslave = 1;
#define LED 13



uint32_t lastCommand = 0;


void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1);
}


void loop() {
   digitalWrite(LED, HIGH);
    delay(50);
    digitalWrite(LED, LOW);
  String cmd = "TESTE DE RS485";
  Serial1.print(cmd);
  //rs485.send(deviceIDslave, cmd, sizeof(cmd));
  delay(1000);
  Serial.print("READ:");
  while (Serial1.available() > 0) {
    char c = Serial1.read();
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    Serial.print(c);
  }
  Serial.println();
  delay(1000);
}


//  -- END OF FILE --
