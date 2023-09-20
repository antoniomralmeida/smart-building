

#ifndef _JSNSR04_H
#define _JSNSR04_H

class JSNSR04T{
  private:
    uint8_t _pinTrigger;
    uint8_t _pinEcho;

  public:

    JSNSR04T(uint8_t pinTrigger, uint8_t pinEcho){
      _pinTrigger = pinTrigger;
      _pinEcho = pinEcho;
  	pinMode(_pinTrigger, OUTPUT);   //Pino de trigger será saída digital
  	pinMode(_pinEcho, INPUT);    //Pino de echo será entrada digital
  
  	digitalWrite(_pinTrigger, LOW); //Saída trigger inicia em nível baixo
	};

    float getDistance() {

	byte retray=10;
	while (retray-->0) {
		float dist = trigPulse();
		if (dist> 0 && dist < 500.0) {
			return dist;
		}
	}
	return NAN;
};


private:
float trigPulse()
{
  digitalWrite(_pinTrigger, HIGH);  //Pulso de trigger em nível alto
  delayMicroseconds(10);     //duração de 10 micro segundos
  digitalWrite(_pinTrigger, LOW);   //Pulso de trigge em nível baixo

  float pulse = pulseIn(_pinEcho, HIGH, 200000); //Mede o tempo em que o pino de echo fica em nível alto
   
  float  dist_cm = pulse/58.82;       //Valor da distância em centímetros
	return dist_cm;
};

};

#endif



