
#include <SoftwareSerial.h>  //Biblioteca de comunicação do módulo
#include <EBYTE.h>           //Biblioteca do Kris

#define M0_LoRa 12
#define M1_LoRa 11
#define RX_LoRa 9   // Vai no TXD do módulo
#define TX_LoRa 10  // Vai no RXD do módulo
#define AUX_LoRa 8

#define TXM_FIXED 0b1

SoftwareSerial lora(RX_LoRa, TX_LoRa);          //Definição dos pinos para criar a comunicação serial
EBYTE LoRa(&lora, M0_LoRa, M1_LoRa, AUX_LoRa);  //Criar objeto de Transmissão, passando na comunicação serial e pinos

void setup() {

  Serial.begin(9600);  //Definir taxa de trabalho em 9600
  lora.begin(9600);
  LoRa.init();  // Inicializa a comunicação e obtem todos os parâmetros do módulo

  //LoRa.Reset();                                      // Reseta parâmetros para os de fábrica
  LoRa.SetAirDataRate(ADR_2400);   // Estabelece a taxa de dados de transmissão
  LoRa.SetAddress(0);              // Estabelece endereço da rede
                                   // Estabelece canal como 23
  LoRa.SaveParameters(TEMPORARY);  // Salva todas as definições de forma temporária

  LoRa.PrintParameters();     // Imprime todos os parâmetros (configurações) obtidos do módulo
  LoRa.SetMode(MODE_NORMAL);  // Define modo de operação como normal
  LoRa.SetTransmissionMode(TXM_FIXED);
}


bool getBusy() {
  if (digitalRead(AUX_LoRa) == HIGH) {
    return false;
  } else return true;
}

bool sendTarget(EBYTE LoRa, uint16_t targetAddres, uint8_t targetChannel) {
  if (LoRa.GetTransmissionMode() != TXM_FIXED) return false;
  if (!getBusy()) {
    uint8_t aH = ((targetAddres & 0xFFFF) >> 8);
    uint8_t aL = (targetAddres & 0xFF);
    LoRa.SetChannel(targetChannel);
    LoRa.SendByte(aH);
    LoRa.SendByte(aL);
    LoRa.SendByte(targetChannel);
    return true;
  } else {
    return false;
  }
}

void loop() {
  if (Serial.available() > 0) {
    String msg = Serial.readString();
    if (sendTarget(LoRa, 2, 16)) {
      for (int i = 0; i < msg.length(); i++) {
        LoRa.SendByte(msg[i]);
      }
      Serial.println(msg);
    }
  }
}
