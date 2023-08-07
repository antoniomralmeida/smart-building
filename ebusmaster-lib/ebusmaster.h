#include <TimeLib.h>
#include <EthernetClient.h>
#include <avr/wdt.h>  // Include the ATmel library

#define RXPIN  10
#define TXPIN  11
#define SensorPIN1 12
#define SensorPIN2 13
#define SensorPIN3_A A2
#define SensorPIN4_A A3
#define BOUNDRATE_SERIAL 115200

enum busType
{
    I2C,
    RS485,
    LORA,
    WIFI
};

String busTypeStr[] ={"I2C","RS485","LORA","WIFI"};

enum sensorType
{
    DHT11T,
    DHT11H,
    DHT22T,
    DHT22H,
    QDY30A_RS485
};

String sensorTypeStr[] ={"DHT11T","DHT11H","DHT22T","DHT22H","QDY30A_RS485"};


enum calcType
{
    ABS,
    ACCU,
    DIFF
};

String calcTypeStr[] ={"ABS","ACCU","DIFF"};

typedef struct
{
  String name;
  String unit;
  byte calcType;
  byte channelType;
  byte channelAddr;
  byte registerAddr;
  byte sensorType;
  byte decimalFix;
  float LastValue;
} sensorData;


byte findBusTypeStr(String type) {
  for (int i=0;i<sizeof(busTypeStr);i++) {
    if (type == busTypeStr[i]) {
      return i;
    }
    return -1;
  }
}


byte findSensorTypeStr(String type) {
  for (int i=0;i<sizeof(sensorTypeStr);i++) {
    if (type == sensorTypeStr[i]) {
      return i;
    }
    return -1;
  }
}


byte findCalcTypeStr(String type) {
  for (int i=0;i<sizeof(calcTypeStr);i++) {
    if (type == calcTypeStr[i]) {
      return i;
    }
    return -1;
  }
}



void wd_reboot() {
  delay(1000);
  wdt_disable();        // Deactivates the function while configuring the time in which it will be reset
  wdt_enable(WDTO_2S);  // We set the timer to restart in 2s
  while (true) {
    delay(1);
  }
}

String version = "MULT-BUS V1.0.0 ";

void splash() {
  Serial.begin(BOUNDRATE_SERIAL);
  Serial.println();
  Serial.print(version);
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.print(__TIME__);
  Serial.println();
}

