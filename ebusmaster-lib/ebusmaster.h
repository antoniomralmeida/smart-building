//E-BUSMASTER-LIB
#include <EthernetClient.h>
#include <avr/wdt.h>  // Include the ATmel library

#define RXPIN  10
#define TXPIN  11
#define SensorPIN1 12
#define SensorPIN2 13
#define SensorPIN3_A A2
#define SensorPIN4_A A3
#define BOUNDRATE_SERIAL 115200

String version = "E-BUSMASTER V1.0 ";

enum busType
{
    I2C,
    RS485,
    LORA,
    WIFI
};
#define NbusType 4
String busTypeStr[NbusType] ={"I2C","RS485","LORA","WIFI"};

enum sensorType
{
    DHT11T,
    DHT11H,
    DHT22T,
    DHT22H,
    QDY30A_RS485
};

#define NsensorType 5
String sensorTypeStr[NsensorType] ={"DHT11T","DHT11H","DHT22T","DHT22H","QDY30A_RS485"};

enum calcType
{
    ABS,
    ACCU,
    DIFF
};

#define NcalcType 3
String calcTypeStr[NcalcType] ={"ABS","ACCU","DIFF"};

typedef struct
{
  String token;
  String variable;
  String unit;
  byte calcType;
  byte channelType;
  byte channelAddr;
  byte registerAddr;
  byte sensorType;
  byte decimalFix;
  float lastValue;
  unsigned long lastTime;
} sensorData;


byte findBusTypeStr(String type) {
  for (int i=0;i<NbusType;i++) {
    if (type.equals(busTypeStr[i])) {
      return i;
    }
  }
  return -1;
}


byte findSensorTypeStr(String type) {
  for (int i=0;i<NsensorType;i++) {
    if (type.equals(sensorTypeStr[i])) {
      return i;
    }
  }
  return -1;
}


byte findCalcTypeStr(String type) {
  for (int i=0;i<NcalcType;i++) {
    if (type.equals(calcTypeStr[i])) {
      return i;
    }
  }
  return -1;
}



void wd_reboot() {
  delay(1000);
  wdt_disable();        // Deactivates the function while configuring the time in which it will be reset
  wdt_enable(WDTO_2S);  // We set the timer to restart in 2s
  while (true) {
    delay(1);
  }
}


void splash() {
  Serial.begin(BOUNDRATE_SERIAL);
  Serial.println();
  Serial.print(version);
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.print(__TIME__);
  Serial.println();
}

