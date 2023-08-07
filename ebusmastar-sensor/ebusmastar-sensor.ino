//E-BUSMASTER

//Libraries
#include <DHT.h>;
#include <EEPROM.h>
#include "Wire.h"
#include "ebusmaster.h"
#include <SoftwareSerial.h>
#include <ModbusRTUSlave.h>


// endereco do modulo slave que pode ser um valor de 0 a 255
byte myAdress;

float data;
SoftwareSerial mySerial(RXPIN, TXPIN);
ModbusRTUSlave modbus(mySerial); // serial port, driver enable pin for rs-485 (optional)
uint16_t holdingRegisters[1];


void setup() {
  splash();

  myAdress = SetupBusAdrress(false);
  Serial.print("BUS ADDRESS: ");
  Serial.println(myAdress, HEX);
  Serial.println("Type 'X' in serial to force setup");
  // ingressa ao barramento I2C com o endereço definido no myAdress (0x08)
  Wire.begin(myAdress);
  //Registra um evento para ser chamado quando chegar algum dado via I2C
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  pinMode(LED_BUILTIN, OUTPUT);
  
  
  modbus.configureHoldingRegisters(holdingRegisters, 1); // unsigned 16 bit integer array of holding register values, number of holding registers
  modbus.begin(myAdress, 9600, SERIAL_8N1);                                // slave id, baud rate, config (optional)
  
  
  Serial.print("Listening...");
}

void receiveEvent(int howMany) {
  // verifica se existem dados para serem lidos no barramento I2C
  if (Wire.available()) {
    for (int i = 0; i < howMany; i++) {
      byte sensorType = Wire.read();
      data = readSensor(sensorType);
      delay(100);
    }
  }
}


float readSensor(byte sensorType) {
  float data;
  if (sensorType == DHT22T || sensorType == DHT11T) {
    byte type = DHT22;
    if (sensorType == DHT11T) {
      type = DHT11;
    }
    DHT dht(SensorPIN1, type);  //// Initialize DHT sensor for normal 16mhz Arduino
    dht.begin();
    data = dht.readTemperature();
    Serial.print("DT22T: ");
  } else if (sensorType == DHT22H || sensorType == DHT11H) {
    byte type = DHT22;
    if (sensorType == DHT11T) {
      type = DHT11;
    }
    DHT dht(SensorPIN1, type);  //// Initialize DHT sensor for normal 16mhz Arduino
    dht.begin();
    data = dht.readHumidity();
    Serial.print("DT22H: ");
  } else {
    Serial.println("Sensor unknow!");
    return NAN;
  }
  Serial.println(data);
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);
  return data;
}

byte SetupBusAdrress(bool force) {
  byte address = EEPROM.read(2);
  // Random MAC address stored in EEPROM
  if (EEPROM.read(1) != '#' || force || address <= 0) {
    while (1) {
      Serial.println("Enter Bus Address:");
      String addr = Serial.readString();
      address = addr.toInt();
      if (address > 0) {
        EEPROM.write(1, '#');
        EEPROM.write(2, address);
        return address;
      }
      delay(500);
    }
  }
  return address;
}


void requestEvent() {
  Wire.write((byte *)&data, 4);
}


void loop() {
  if (Serial.available()) {
    String cmd = Serial.readString();
    if (cmd[0] == 'X') {
      SetupBusAdrress(true);
      wd_reboot();
    }
  }

  modbus.poll();
  if (holdingRegisters[0] > 0) {
    byte sensorType = holdingRegisters[0];
    holdingRegisters[0] = readSensor(sensorType) * 100;
  }
  delay(100);
}
