
#define SensorPIN1 7
#define SensorPIN2 8
#define SensorPINA3 A3
#define SensorPINA4 A4


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

typedef struct
{
  byte channelType;
  byte channelAddr;
  byte sensorType;
  int dataLength;
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
