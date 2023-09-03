
#include <ModbusMaster.h>

#define MAX485_DE 3
#define MAX485_RE_NEG 2

// instantiate ModbusMaster object
ModbusMaster node;

void preTransmission() {
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}

void postTransmission() {
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}

void setup() {
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  // Init in receive mode
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);

  Serial.begin(9600);

  // Modbus slave ID 1
  Serial1.begin(9600, SERIAL_8N1);
  node.begin(1, Serial1);
  // Callbacks allow us to configure the RS485 transceiver correctly
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}

typedef struct {
  byte address;
  byte function;
  uint16_t reg;
  uint16_t data;
  uint16_t crc;
} REQ_MODBUS;

typedef struct {
  byte address;
  byte function;
  byte length;
  byte data[2];
  uint16_t crc;
} RET_MODBUS;

void loop() {
  Serial.println("Reading req_QDY30A");
  /*REQ_MODBUS req_QDY30A = { 0x01, 0x03, 0x00, 0x01, 0x840A };
  RET_MODBUS ret_QDY30A;
  byte buffer[sizeof(ret_QDY30A)];
  //Serial1.write((byte *)&req_QDY30A, sizeof(req_QDY30A));
  for (int i=0;i<sizeof(ret_QDY30A);i++) {
    if (Serial1.available()) {
      buffer[i] = Serial1.read();
    }
    Serial.println(i);
  } 
  ret_QDY30A = *((RET_MODBUS *)&buffer);
  Serial.println(ret_QDY30A.data[1], HEX);
  Serial1.flush();
  */

  node.getResponseBuffer(uint8_t)
  delay(1000);
}
