

#include <SoftwareSerial.h>
#include <ModbusRTUMaster.h>


const uint8_t rxPin = 10;
const uint8_t txPin = 11;

SoftwareSerial mySerial(rxPin, txPin);
ModbusRTUMaster modbus(mySerial); // serial port, driver enable pin for rs-485 (optional)

uint16_t holdingRegisters[2];

void setup() {
  Serial.begin(9600);
  
  modbus.begin(9600, SERIAL_8N1); // baud rate, config (optional)
  modbus.setTimeout(1000);
  Serial.println("ModbusRTUMaster");
}

void loop() {
  if (modbus.writeSingleHoldingRegister(1, 0, 0x01)) {
    Serial.println("reading...");
    delay(1000);

  if (  modbus.readHoldingRegisters(1, 0, holdingRegisters, 1)) {
      Serial.println(holdingRegisters[0]);
    };

  } 
  delay(5000);
}
