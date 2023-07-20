// Inclusão da biblioteca
#include <Wire.h> // Biblioteca nativa do core Arduino
// Variáveis globais
const int slaveAddress = 0x08; // constante armazena o endereço do dispositivo slave
int x = 0; // variável de armazenagem do valor para envio
void setup() {
  Wire.begin(); // inicia a comunicação I2C
}
void loop() {
  Wire.beginTransmission(slaveAddress); // transmite para o dispositivo slave
  // Cria e formata a mensagem para envio
  String message = "O valor de x é ";
  message.concat(x);
  message.concat("\n");
  Wire.write(message.c_str());// envia a mensagem
  Wire.endTransmission();  //para de transmitir
  x++; // incremento da variável
  delay(500); // pausa de 500 milissegundos
}