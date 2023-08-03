
#include <SoftwareSerial.h>
#include <ModbusRTUSlave.h>

#define SLAVE_ADDR  1
#define REGISTER  0X00

#define rxPin 10
#define txPin 11

// Set up a new SoftwareSerial object
SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);
ModbusRTUSlave modbus(mySerial); // serial port, driver enable pin for rs-485 (optional)
uint16_t Registers[1];

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("ModbusRTUSlave");
  
  modbus.configureInputRegisters(Registers, 1);     // unsigned 16 bit integer array of input register values, number of input registers
  modbus.begin(SLAVE_ADDR, 9600, SERIAL_8N1);   
}

void loop() {
  modbus.poll();
  Serial.println(Registers[0]);

  // wait for 1 second
  delay(1000);

}
