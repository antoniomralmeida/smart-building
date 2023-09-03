
#include <SoftwareSerial.h>
#include <ModbusRTUSlave.h>

const uint8_t rxPin = 10;
const uint8_t txPin = 11;

SoftwareSerial mySerial(rxPin, txPin);
ModbusRTUSlave modbus(mySerial); // serial port, driver enable pin for rs-485 (optional)

uint16_t holdingRegisters[2];


void setup() {
  Serial.begin(9600);

  modbus.configureHoldingRegisters(holdingRegisters, 1); // unsigned 16 bit integer array of holding register values, number of holding registers
  modbus.begin(1, 9600, SERIAL_8N1);                                // slave id, baud rate, config (optional)
  Serial.println("ModbusRTUSlave");
}

void loop() {
  modbus.poll();
  if (holdingRegisters[0] == 0x01) {
    holdingRegisters[0] = random(1000); 
    Serial.println(holdingRegisters[0]);   
  }
  
  delay(100);
}
