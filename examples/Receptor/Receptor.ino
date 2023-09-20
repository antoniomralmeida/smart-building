#include <SoftwareSerial.h>                               //Biblioteca de comunicação do módulo
#include <EBYTE.h>                                        //Biblioteca do Kris
  
#define M0_LoRa 12
#define M1_LoRa 11
#define RX_LoRa 9   // Vai no TXD do módulo
#define TX_LoRa 10  // Vai no RXD do módulo
#define AUX_LoRa 8
                            
SoftwareSerial lora(RX_LoRa, TX_LoRa);                   //Definição dos pinos para criar a comunicação serial
EBYTE LoRa(&lora, M0_LoRa, M1_LoRa, AUX_LoRa);           //Criar objeto de Transmissão, passando na comunicação serial e pinos

void setup() {
  
Serial.begin (9600);                                    //Definir taxa de trabalho em 9600
lora.begin(9600);
  LoRa.init();                                         // Inicializa a comunicação e obtem todos os parâmetros do módulo

  //LoRa.Reset();                                       // Reseta parâmetros para os de fábrica
  LoRa.SetAirDataRate(ADR_2400);                          // Estabelece a taxa de dados de transmissão
  LoRa.SetAddress(0);                                   // Estabelece o endereço da rede
  LoRa.SetChannel(16);                                  // Estabelece canal como 23
  LoRa.SaveParameters(TEMPORARY);                       // Salva todas as definições de forma temporária

  LoRa.PrintParameters();                               // Imprime todos os parâmetros (configurações) obtidos do módulo 
  LoRa.SetMode(MODE_NORMAL);                            // Define modo de operação como normal
}
 


void loop() {
  
if (lora.available()>0){
  String input = lora.readString();
  Serial.print(" A Mensagem do Lora é: "); 
  for (int i=0;i<input.length();i++) {
    Serial.print(input[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  
}
delay(1500);
}
