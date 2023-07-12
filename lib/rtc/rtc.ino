#include <ThreeWire.h> //INCLUSÃO DA BIBLIOTECA
#include <RtcDS1302.h> //INCLUSÃO DA BIBLIOTECA


ThreeWire myWire(4, 5, 2); //OBJETO DO TIPO ThreeWire
RtcDS1302<ThreeWire> Rtc(myWire); //OBJETO DO TIPO RtcDS1302

void setup () {
    Serial.begin(9600); //INICIALIZA A SERIAL
    Rtc.Begin(); //INICIALIZA O RTC
    Serial.print("Compilado em: "); //IMPRIME O TEXTO NO MONITOR SERIAL
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__); //VARIÁVEL RECEBE DATA E HORA DE COMPILAÇÃO
    printDateTime(compiled); //PASSA OS PARÂMETROS PARA A FUNÇÃO printDateTime


    if(Rtc.GetIsWriteProtected()){ //SE O RTC ESTIVER PROTEGIDO CONTRA GRAVAÇÃO, FAZ
        Serial.println("RTC está protegido contra gravação. Habilitando a gravação agora..."); //IMPRIME O TEXTO NO MONITOR SERIAL
        Rtc.SetIsWriteProtected(false); //HABILITA GRAVAÇÃO NO RTC
        Serial.println(); //QUEBRA DE LINHA NA SERIAL
    }

    if(!Rtc.GetIsRunning()){ //SE RTC NÃO ESTIVER SENDO EXECUTADO, FAZ
        Serial.println("RTC não está funcionando de forma contínua. Iniciando agora..."); //IMPRIME O TEXTO NO MONITOR SERIAL
        Rtc.SetIsRunning(true); //INICIALIZA O RTC
        Serial.println(); //QUEBRA DE LINHA NA SERIAL
    }
    
    RtcDateTime now = Rtc.GetDateTime(); //VARIÁVEL RECEBE INFORMAÇÕES
    Serial.println(compiled);
    Serial.println(now);
    Serial.println(now < compiled);
    int comp = compareDateTime(compiled , now);

    if (comp == 1) { //SE A INFORMAÇÃO REGISTRADA FOR MENOR QUE A INFORMAÇÃO COMPILADA, FAZ
        Serial.println("As informações atuais do RTC estão desatualizadas. Atualizando informações..."); //IMPRIME O TEXTO NO MONITOR SERIAL
        Rtc.SetDateTime(compiled); //INFORMAÇÕES COMPILADAS SUBSTITUEM AS INFORMAÇÕES ANTERIORES
        Serial.println(); //QUEBRA DE LINHA NA SERIAL
    }
    else if (comp == -1) { //SENÃO, SE A INFORMAÇÃO REGISTRADA FOR MAIOR QUE A INFORMAÇÃO COMPILADA, FAZ
        Serial.println("As informações atuais do RTC são mais recentes que as de compilação. Isso é o esperado."); //IMPRIME O TEXTO NO MONITOR SERIAL
        Serial.println(); //QUEBRA DE LINHA NA SERIAL
    }
    else  { //SENÃO, SE A INFORMAÇÃO REGISTRADA FOR IGUAL A INFORMAÇÃO COMPILADA, FAZ
        Serial.println("As informações atuais do RTC são iguais as de compilação! Não é o esperado, mas está tudo OK."); //IMPRIME O TEXTO NO MONITOR SERIAL
        Serial.println(); //QUEBRA DE LINHA NA SERIAL
    }
}

void loop () {
    RtcDateTime now = GetDateTime(); //VARIÁVEL RECEBE INFORMAÇÕESc.
    printDateTime(now); //PASSA OS PARÂMETROS PARA A FUNÇÃO printDateTime
    delay(1000); //INTERVALO DE 1 SEGUNDO
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

RtcDateTime GetDateTime() {
  RtcDateTime now;
  while (now.Hour() == 0 && now.Minute() == 0 && now.Second() == 0) {
    now = Rtc.GetDateTime();
    delay(1);
  }
  return now;
}

void printDateTime(const RtcDateTime& dt){
    char datestring[20]; //VARIÁVEL ARMAZENA AS INFORMAÇÕES DE DATA E HORA

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"), //FORMATO DE EXIBIÇÃO DAS INFORMAÇÕES
            dt.Day(), //DIA
            dt.Month(), //MÊS
            dt.Year(), //ANO
            dt.Hour(), //HORA
            dt.Minute(), //MINUTOS
            dt.Second() ); //SEGUNDOS
    Serial.print(datestring); //IMPRIME NO MONITOR SERIAL AS INFORMAÇÕES
    Serial.println(); //QUEBRA DE LINHA NA SERIAL
}

int compareDateTime(const RtcDateTime& dt1, const RtcDateTime& dt2){

  long udt1 = dt1.Year() * 365 * 30 * 24 * 60 * 60 + dt1.Month() * 30 * 24 * 60 * 60  + dt1.Day() * 24 * 60 * 60  + dt1.Hour() * 60 * 60 + dt1.Minute() * 60 + dt1.Second();
  long udt2 = dt2.Year() * 365 * 30 * 24 * 60 * 60 + dt2.Month() * 30 * 24 * 60 * 60  + dt2.Day() * 24 * 60 * 60  + dt2.Hour() * 60 * 60 + dt2.Minute() * 60 + dt2.Second();

  if (udt1 > udt2) {
    return 1;
  } else if (udt1 < udt2) {
    return -1;
  }    else {
    return 0;
  }
}
