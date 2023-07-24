#include <ModbusMaster.h>

/*!
We're using a MAX485-compatible RS485 Transceiver.
Rx/Tx is hooked up to the hardware serial port at 'Serial'.
The Data Enable and Receiver Enable pins are hooked up as follows:
*/
#define MAX485_DE_RE 4


// instantiate ModbusMaster object
ModbusMaster node;

void preTransmission() {
  digitalWrite(MAX485_DE_RE, 1);
}

void postTransmission() {
  digitalWrite(MAX485_DE_RE, 0);
}

void setup() {
  pinMode(MAX485_DE_RE, OUTPUT);
  // Init in receive mode
  digitalWrite(MAX485_DE_RE, 0);

  // Modbus communication runs at 115200 baud
  Serial.begin(9600);

  Serial.print("Got here1");

  // Modbus slave ID 1
  Serial1.begin(9600, SERIAL_8N1);
  node.begin(1, Serial1);
  // Callbacks allow us to configure the RS485 transceiver correctly
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
  Serial.print("Got here2");
}


void loop() {
  uint8_t result;
  uint16_t data[6];

  result = node.writeSingleRegister(0x00, 0x01);

  // Read 16 registers starting at 0x3100)
  result = node.readInputRegisters(0x00, 2);
  Serial.println(result, HEX);
  
  if (result == node.ku8MBSuccess) {
    Serial.print("Vbatt: ");
    Serial.println(node.getResponseBuffer(0x04) / 100.0f);
    Serial.print("Vload: ");
    Serial.println(node.getResponseBuffer(0xC0) / 100.0f);
    Serial.print("Pload: ");
    Serial.println((node.getResponseBuffer(0x0D) + node.getResponseBuffer(0x0E) << 16) / 100.0f);
  }

  delay(1000);
}