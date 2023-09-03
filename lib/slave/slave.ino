#include <Wire.h> // Biblioteca nativa do core Arduino
// Variáveis globais
const int myAddress = 0x08; // armazena o endereço deste dispositivo (slave)
void setup() {
  Serial.begin(9600);  // Configura a taxa de transferência em bits por
  //                        segundo (baud rate) para transmissão serial.
  Serial.println();
  Wire.begin(myAddress); // inicia o dispositivo com o endereço definido anteriormente
  Wire.onReceive(receiveEvent); //registra o evento de recebimento de mensagem
  Serial.println("Software do arduino MEGA");
}
void loop() {}
void receiveEvent(int howMany) {
  String message = readString();
  Serial.print(message); // imprime a mensagem recebida
}
String readString() {
  String retorno;  
  while (Wire.available()) { // Enquanto houver bytes disponíveis para leitura, ...
    char c = Wire.read(); // recebe o byte como caractere
    retorno += c;
  }
  return retorno;
}