

//Libraries
#include <DHT.h>;
#include "Wire.h"
#include "smart-building.h"


// endereco do modulo slave que pode ser um valor de 0 a 255
#define myAdress 0x08


float data;

void setup() {
  Serial.begin(9600);

  // ingressa ao barramento I2C com o endereço definido no myAdress (0x08)
  Wire.begin(myAdress);
  //Registra um evento para ser chamado quando chegar algum dado via I2C
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  pinMode(LED_BUILTIN, OUTPUT);
}

void receiveEvent(int howMany) {
  // verifica se existem dados para serem lidos no barramento I2C
  if (Wire.available()) {
    for (int i = 0; i < howMany; i++) {
      byte register_address = Wire.read()
      if (register_address == DHT22T || register_address == DHT11T) {
        u_int8t type = DHT22;
        if (register_address == DHT11T) {
          type = DHT11;
        }
        DHT dht(SensorPIN1, type);  //// Initialize DHT sensor for normal 16mhz Arduino
        dht.begin();
        data = dht.readTemperature();
        Serial.print("DT22T: ");
      } else if (register_address == DHT22H || register_address == DHT11H) {
        u_int8t type = DHT22;
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
      delay(100);                      // wait for a second
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}

void requestEvent() {
  Wire.write((byte *)&data, 4);
}


void loop() {
}
