//E-BUSMASTER-LIB
#include <EthernetClient.h>
#include <avr/wdt.h>  // Include the ATmel library
	
#define SERIALX4_RX 6
#define SERIALX4_TX 7
#define SERIALX5_RX 8
#define SERIALX5_TX 9

#define SENSOR_D1 10
#define SENSOR_D2 11
#define SENSOR_D3 12
#define SENSOR_D4 13
#define SENSOR_A1 A2
#define SENSOR_A2 A3

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
    QDY30A_RS485, //Sensor de nivel (presão)
    JSN_SR04M //Módulo sensor ultrassonico Water Proof
};

#define NsensorType 6
String sensorTypeStr[NsensorType] ={"DHT11T","DHT11H","DHT22T","DHT22H","QDY30A_RS485", "JSN_SR04T"};

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
  byte busType;
  byte channelAddr;
  byte registerAddr;
  byte sensorType;
  byte decimalFix;
  float lastValue;
  unsigned long lastTime;
} sensorData;


typedef struct
{
  char token[38];
  char variable[16];
  char unit[16];
  float lastValue;
  unsigned long lastTime;
} dataFileType;





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

