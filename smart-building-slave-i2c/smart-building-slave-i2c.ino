//Libraries
#include <DHT.h>;
#include <EEPROM.h>
#include "Wire.h"
#include "smart-building.h"


// endereco do modulo slave que pode ser um valor de 0 a 255
byte myAdress;


float data;

void setup() {
  String version = "SMART-BUILDING I2C V 1.0.0";
  Serial.begin(BOUNDRATE_SERIAL);
  Serial.println();
  Serial.print(version);
  Serial.print(__DATE__);
  Serial.print(__TIME__);
  Serial.println();

  myAdress = SetupBusAdrress(false);
  Serial.print("BUS ADDRESS: ");
  Serial.println(myAdress, HEX);
  Serial.println("Type 'X' in serial to force setup");
  // ingressa ao barramento I2C com o endereço definido no myAdress (0x08)
  Wire.begin(myAdress);
  //Registra um evento para ser chamado quando chegar algum dado via I2C
  //Wire.onReceive(receiveEvent);
  //Wire.onRequest(requestEvent);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.print("Listening...");
}

void receiveEvent(int howMany) {
  // verifica se existem dados para serem lidos no barramento I2C
  if (Wire.available()) {
    for (int i = 0; i < howMany; i++) {
      byte register_address = Wire.read();
      if (register_address == DHT22T || register_address == DHT11T) {
        byte type = DHT22;
        if (register_address == DHT11T) {
          type = DHT11;
        }
        DHT dht(SensorPIN1, type);  //// Initialize DHT sensor for normal 16mhz Arduino
        dht.begin();
        data = dht.readTemperature();
        Serial.print("DT22T: ");
      } else if (register_address == DHT22H || register_address == DHT11H) {
        byte type = DHT22;
        if (register_address == DHT11T) {
          type = DHT11;
        }
        DHT dht(SensorPIN1, type);  //// Initialize DHT sensor for normal 16mhz Arduino
        dht.begin();
        data = dht.readHumidity();
        Serial.print("DT22H: ");
      } else {
        Serial.println("Registrador desconhecido!");
        continue;
      }
      Serial.print(data, 10);
      Serial.println();
      digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
      delay(100);                       // wait for a second
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
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

  DHT dht(13, DHT22);  //// Initialize DHT sensor for normal 16mhz Arduino
  dht.begin();
  data = dht.readHumidity();
  Serial.println(data);
  delay(2000);
}
