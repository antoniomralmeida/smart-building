

#include <SoftwareSerial.h>
#include <ModbusRTUMaster.h>

// https://joy-it.net/files/files/Produkte/COM-TTL-RS485/COM-TTL-RS485_Manual_2021-05-07.pdf
const uint8_t rxPin = 10;
const uint8_t txPin = 11;

SoftwareSerial mySerial(rxPin, txPin);
ModbusRTUMaster modbus(mySerial);  // serial port, driver enable pin for rs-485 (optional)

uint16_t holdingRegisters[2];

void setup() {
  Serial.begin(9600);

  modbus.begin(9600, SERIAL_8N1);  // baud rate, config (optional)
  modbus.setTimeout(1000);
  Serial.println("ModbusRTUMaster");
}

int addr = 1;

void loop() {
  Serial.print("Reading req_QDY30A ");
  Serial.println(addr);
  /*
  if (modbus.writeSingleHoldingRegister(addr++, 0, 0x01)) {
    Serial.println("reading...");
    delay(1000);

  if (  modbus.readHoldingRegisters(1, 0, holdingRegisters, 1)) {
      Serial.println(holdingRegisters[0]);
    };

  } 
*/

  byte req_QDY30A[] = { addr++, 0X03, 0X00, 0X00, 0X00, 0X01, 0X84, 0X0A };
  byte buffer[sizeof(req_QDY30A)];
  mySerial.write((byte *)&req_QDY30A, sizeof(req_QDY30A));
  delay(500);
  for (int i = 0; i < sizeof(req_QDY30A); i++) {
    if (mySerial.available()) {
      buffer[i] = mySerial.read();
      Serial.print(buffer[i], HEX);
      Serial.print(" ");
      Serial.print(buffer[i], DEC);
      Serial.print(" ");
    }
  }
  Serial.println();
  delay(1000);
}
