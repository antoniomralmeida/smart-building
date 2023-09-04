

//Libraries
#include <DHT.h>;
#include "Wire.h"

//Constants
#define DHTPIN 7           // what pin we're connected to
#define DHTTYPE DHT22      // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);  //// Initialize DHT sensor for normal 16mhz Arduino

// endereco do modulo slave que pode ser um valor de 0 a 255
#define myAdress 0x08

#define DHT22T 0x01
#define DHT22H 0x02


char buff[100];
float data;

void setup() {
  Serial.begin(9600);
  dht.begin();
  // ingressa ao barramento I2C com o endereço definido no myAdress (0x08)
  Wire.begin(myAdress);
  //Registra um evento para ser chamado quando chegar algum dado via I2C
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void receiveEvent(int howMany) {
  // verifica se existem dados para serem lidos no barramento I2C
  if (Wire.available()) {
    for (int i = 0; i < howMany; i++) {
      byte register_address = Wire.read();
      if (register_address == DHT22T) {
        data = dht.readTemperature();
        Serial.print("DT22T: ");
      } else if (register_address == DHT22H) {
        data = dht.readHumidity();
        Serial.print("DT22H: ");
      } else {
        Serial.println("Registrador desconhecido!");
        continue;
      }
      Serial.print(data, 10);
      Serial.println();
      delay(100);
    }
  }
}

void requestEvent() {
    Wire.write((byte *)&data, 4);
}
  

void loop() {
}
