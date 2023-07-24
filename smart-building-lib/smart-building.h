
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

enum SensorType
{
    DHT11T,
    DHT11H,
    DHT22T,
    DHT22H,
    QDY30A_RS485
};



typedef struct
{
  byte channelType;
  byte channelAddr;
  byte SensorType;
  int dataLength;
} SensorData;