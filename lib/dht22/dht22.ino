

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

byte register_address;




void setup() {
  Serial.begin(9600);
  dht.begin();
  // ingressa ao barramento I2C com o endereço definido no myAdress (0x08)
  Wire.begin(myAdress);
  //Registra um evento para ser chamado quando chegar algum dado via I2C
  Wire.onReceive(receiveEvent);
  //Wire.onRequest(requestEvent);
}

// vide "void setup()"
void receiveEvent(int howMany) {
  // verifica se existem dados para serem lidos no barramento I2C
  if (Wire.available()) {
    // le o byte recebido
    char received = Wire.read();
       Serial.println(received);
   
  }
}


void receiveEvent2(int howMany) {
  Serial.println("receiveEvent...");
  // verifica se existem dados para serem lidos no barramento I2C
  if (Wire.available()) {
    // le o byte recebido
    if (howMany > 0)  // for safety, check if some data was received.
    {
      register_address = Wire.read();
      Serial.println(register_address);
    }
  }
}

void requestEvent() {
    Serial.println("requestEvent...");
  if (register_address == DHT22T) {
    float data = dht.readTemperature();
    Wire.write((byte *)&data, 4);
  }
  if (register_address == DHT22H) {
    float data = dht.readHumidity();
    Wire.write((byte *)&data, 4);
  }
}

void loop() {
}
