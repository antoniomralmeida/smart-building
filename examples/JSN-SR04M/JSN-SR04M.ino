#include <JSNSR04.h>
#define trig 9  //Pino 9 do Arduino será a saída de trigger
#define echo 8  //Pino 8 do Arduino será a entrada de echo

JSNSR04 ultrasom = JSNSR04(trig,echo);

void setup() {

  Serial.begin(9600);  //Inicia comunicação serial
}

void loop() {

  Serial.println(ultrasom.getDistance());
  delay(200);  //Taxa de atualização
}
