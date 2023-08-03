//    FILE: RS485_simple_slave.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: simple listening slave
//     URL: https://github.com/RobTillaart/RS485

//  this is the code of a simple slave  (needs simple master)
//  it receives one of 3 (single char) commands to the slave
//  '0' == set LED LOW.
//  '1' == set LED HIGH.
//  '2' == return status to master.
//
//  print debug messages SEND and RECV with data.
//  Note that one needs a 2nd Serial port for nice debugging.
//  (or an LCD screen whatever).


#include "Arduino.h"
#include "RS485.h"


const uint8_t sendPin  = 4;
const uint8_t deviceID = 0X01;


RS485 rs485(&Serial, sendPin, deviceID);


const uint8_t LED = 13;
uint8_t status = LOW;


void setup()
{
  Serial.begin(9600);
  rs485.setMicrosPerByte(9600);
  
  pinMode(LED, OUTPUT);

  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);
  rs485.setRXmode();
}


void loop()
{
  while (rs485.available() > 0)
  {
    int c = rs485.read();    
    Serial.println(c, HEX);
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
   
  }
}


//  -- END OF FILE --

