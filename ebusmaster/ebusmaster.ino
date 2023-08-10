//E-BUSMASTER
#include <TimeLib.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <LiquidCrystal_I2C.h>  // Biblioteca utilizada para fazer a comunicação com o display 20x4
#include <EEPROM.h>
#include <SPI.h>
#include <RtcDS1302.h>
#include <Wire.h>  // Biblioteca utilizada para fazer a comunicação com o I2C
#include <ebusmaster.h>
#include <SD.h>
#include <NTPClient.h>
#include <SoftwareSerial.h>
#include <ModbusRTUMaster.h>
#include <avr/wdt.h>  // Include the ATmel library


#define COLS 16  // Serve para definir o numero de colunas do display utilizado
#define ROWS 2   // Serve para definir o numero de linhas do display utilizado
#define STATUS_COL 13
#define LCDADDR 0x27  // Módulo I2C para Display LCD.
#define APISERVER "api.tago.io"

#define LED_ERR 13
#define SDCARD_SS_PIN 4
#define MAX_SENSORS 64
#define CE 2
#define IO 4
#define SCLK 5

sensorData sensors[MAX_SENSORS];
byte nSensors = 0;

LiquidCrystal_I2C lcd(LCDADDR, COLS, ROWS);  // Chamada da funcação LiquidCrystal para ser usada com o I2C
EthernetClient client;
byte MACAddress[8];
char buff[128];

File myFile;

ThreeWire myWire(IO, SCLK, CE);  // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

SoftwareSerial mySerial(RXPIN, TXPIN);
ModbusRTUMaster modbus(mySerial);  // serial port, driver enable pin for rs-485 (optional)

EthernetUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void setup() {
  splash();

  setupMAC(false);
  lcd.init();       // Serve para iniciar a comunicação com o display já conectado
  lcd.backlight();  // Serve para ligar a luz do display
  lcd.clear();      // Serve para limpar a tela do display

  lcd.setCursor(0, 0);  // Coloca o cursor do display na coluna 1 e linha 1
  lcd.print(version);
  delay(1000);

  lcd.clear();              // Serve para limpar a tela do display
  lcd.setCursor(0, 0);      // Coloca o cursor do display na coluna 1 e linha 1
  lcd.print("Booting...");  // Comando de saída com a mensagem que deve aparecer na coluna 2 e linha 1.

  logMsg("S01 - Wait Serial Port");

  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  logMsg("S02 - Initialize Ethernet with DHCP");
  lcd.setCursor(0, 1);  // Coloca o cursor do display na coluna 1 e linha 1
  lcd.print(MAC2String());


  // start the Ethernet connection:
  if (Ethernet.begin(MACAddress) == 0) {
    logMsg("E01 - Failed to configure Ethernet using DHCP");

    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      logMsg("E02 - Ethernet shield was not found.");

    } else if (Ethernet.linkStatus() == LinkOFF) {
      logMsg("E03 - Ethernet cable is not connected.");
    }
    reboot();
  }

  // print your local IP address:
  Serial.print("My IP: ");
  Serial.println(Ethernet.localIP());

  logMsg("S03 - Setup NTP");
  timeClient.begin();
  timeClient.setTimeOffset(-3 * 3600);
  if (!timeClient.update()) {
    logMsg("E04 - NTP Setup error!");
    reboot();
  }
  if (!timeClient.isTimeSet()) {
    logMsg("E05 - NTP Server not found!");
    reboot();
  }
  time_t t = timeClient.getEpochTime();
  sprintf(buff, "%02d.%02d.%02d %02d:%02d:%02d", day(t), month(t), year(t), hour(t), minute(t), second(t));
  Serial.println(buff);

  logMsg("S04 - Setup RTC");
  Rtc.SetIsWriteProtected(false);
  Rtc.SetIsRunning(true);
  time_t now = timeClient.getEpochTime();
  Rtc.SetDateTime(RtcDateTime(now));
  time_t rtc = getTime();
  sprintf(buff, "%02d.%02d.%02d %02d:%02d:%02d", day(rtc), month(rtc), year(rtc), hour(rtc), minute(rtc), second(rtc));
  Serial.println(buff);


  logMsg("S05 - SD Card setup");
  if (!SD.begin(SDCARD_SS_PIN)) {
    logMsg("E06 - SD card initialization failed!");
    reboot();
  }

  logMsg("S06 - Read Data Setup");
  myFile = SD.open("SETUP.CSV");
  if (!myFile) {
    logMsg("E07 - SETUP.CSV not found!");
    reboot();
  }

  char inputChar = myFile.read();  //get one byte from file
  String field = "";
  while (myFile.available()) {
    byte column = 0;
    byte pt = 0;
    while (myFile.available()) {
      if (inputChar == '\n' || inputChar == ',') {
        switch (column) {
          case 0:
            {
              sensors[nSensors].token = field;
              break;
            }
          case 1:
            {
              sensors[nSensors].variable = field;
              break;
            }
          case 2:
            {
              sensors[nSensors].unit = field;
              break;
            }
          case 3:
            {
              sensors[nSensors].calcType = findCalcTypeStr(field);
              break;
            }
          case 4:
            {
              sensors[nSensors].busType = findBusTypeStr(field);
              break;
            }
          case 5:
            {
              sensors[nSensors].channelAddr = field.toInt();
              break;
            }
          case 6:
            {
              sensors[nSensors].decimalFix = field.toInt();
              break;
            }

          case 7:
            {

              sensors[nSensors].sensorType = findSensorTypeStr(field);
              if (sensors[nSensors].sensorType == 255) {
                logMsg("E08 - Sensor type unknow in SETUP.CSV!");
                Serial.println(field);
                reboot();
              }
              break;
            }
          case 8:
            {
              sensors[nSensors].decimalFix = field.toInt();
              break;
            }
        }
        column++;
        field = "";
      } else {
        field += inputChar;
      }
      bool breakLine = (inputChar == '\n');
      if (myFile.available()) {
        inputChar = myFile.read();
      }
      if (breakLine) {
        break;
      }
    }
    nSensors++;
  }
  myFile.close();

  logMsg("S07 - Connect API SERVER");
  if (!client.connect(APISERVER, 80)) {
    Serial.println("E07 - API SERVER not online!");
    reboot();
  }

  lcd.clear();                    // Serve para limpar a tela do display
  lcd.setCursor(0, 0);            // Coloca o cursor do display na coluna 1 e linha 1
  lcd.print(Ethernet.localIP());  // Comando de saída com a mensagem que deve aparecer na coluna 2 e linha 1.
  Serial.println("OK.");
  Wire.begin();
}

int oneminute = 0;
void loop() {
  time_t rtc = getTime();
  if (now != 0) {
    if (oneminute == 0) {
      Serial.println();
      sprintf(buff, "%02d.%02d.%02d %02d:%02d:%02d", day(rtc), month(rtc), year(rtc), hour(rtc), minute(rtc), second(rtc));
      Serial.print(buff);
      ProcessChannelRead();
      Serial.println("Wait time...");
    }
    sprintf(buff, "%02d:%02d:%02d", hour(rtc), minute(rtc), second(rtc));
    lcd.setCursor(0, 1);
    lcd.print(buff);
  }
  oneminute = (oneminute + 1) % 60;
  delay(1000);
}

void ProcessChannelRead() {
  Serial.println(", processChannelRead");
  for (int i = 0; i < nSensors; i++) {
    Serial.print("Read busType=" + busTypeStr[sensors[i].busType] + " channelAddr=" + String(sensors[i].channelAddr, HEX) + " sensorType=" + sensorTypeStr[sensors[i].sensorType]);
    switch (sensors[i].busType) {
      case I2C:
        Wire.beginTransmission(sensors[i].channelAddr);
        Wire.write(sensors[i].sensorType);
        Wire.endTransmission();
        delay(500);
        Wire.requestFrom((int)sensors[i].channelAddr, sizeof(sensors[i].lastValue));
        int timeOut = 0;
        while (Wire.available() == 0 && timeOut++ < 10) {
          delay(100);
        }
        if (Wire.available()) {
          Serial.print(" Received ");
          byte buffer[sizeof(sensors[i].lastValue)];
          for (int i = 0; i < sizeof(sensors[i].lastValue); i++) {
            buffer[i] = Wire.read();
          }
          float data = *((float *)&buffer[0]);
          if (sensors[i].decimalFix != 0) {
            data = data * pow(10, sensors[i].decimalFix);
          }
          sensors[i].lastValue = data;
          Serial.print(data);
          logFile(sensors[i].token, sensors[i].variable, sensors[i].lastTime, sensors[i].lastValue, sensors[i].unit);
          if (!IOTSendTagoIO(sensors[i].token, sensors[i].variable, sensors[i].lastTime, sensors[i].lastValue, sensors[i].unit)) {
            saveData(i, sensors[i].token, sensors[i].variable, sensors[i].lastTime, sensors[i].lastValue, sensors[i].unit);
          }
        } else {
          Serial.print(" receive error ");
        }
        break;
      case RS485:
        if (modbus.writeSingleHoldingRegister(sensors[i].channelAddr, sensors[i].registerAddr, sensors[i].sensorType)) {
          delay(1000);
          uint16_t data;
          if (modbus.readHoldingRegisters(sensors[i].channelAddr, sensors[i].registerAddr, data, 1)) {
            sensors[i].lastTime = getTime();
            sensors[i].lastValue = data;
            if (sensors[i].decimalFix != 0) {
              sensors[i].lastValue = sensors[i].lastValue * pow(10, sensors[i].decimalFix);
            }
            bool saved = IOTSendTagoIO(sensors[i].token, sensors[i].variable, sensors[i].lastTime, sensors[i].lastValue, sensors[i].unit);
          };
        }
        break;
    }
    Serial.println();
  }
}


time_t getTime() {
  time_t time = 0;
  while (!time) {
    time = Rtc.GetDateTime().TotalSeconds();
    delay(5);
  }
  return time;
}

bool saveData(byte sensorID, String token, String variable, time_t time, float value, String unit) {
  File logFile;
  sprintf(buff, "%02X%u.TMP", sensorID, random(999999));
  String fileName = String(buff);
  logFile = SD.open(fileName, FILE_WRITE);
  if (logFile) {
    logFile.seek(EOF);
    String record = token + "," + variable + "," + timestampISO8601(time) + "," + String(value);
    logFile.println(record);
    logFile.close();
    Serial.print(" saveData ");
    return true;
  } else {
    Serial.print(" Error Data ");
    return false;
  }
}


void logFile(String token, String variable, time_t time, float value, String unit) {
  File logFile;
  String fileName = dateISO8601(Rtc.GetDateTime().Ntp32Time()) + ".LOG";
  logFile = SD.open(fileName, FILE_WRITE);
  if (logFile) {
    logFile.seek(EOF);
    String record = token + "," + variable + "," + timestampISO8601(time) + "," + String(value);
    logFile.println(record);
    logFile.close();
    Serial.print(" LogFile ");
  } else {
    Serial.print(" Error log ");
  }
}

//unsigned long lastConnectionTime = 0;  // last time you connected to the server, in milliseconds

// this method makes a HTTP connection to the server:
bool IOTSendTagoIO(String device_token, String variable, time_t time, float value, String unit) {
  //DATA-JSON
  String json = "{";
  json += "\"variable\":\"" + variable + "\",";
  json += "\"value\":" + String(value) + ",";
  json += "\"unit\":\"" + unit + "\",";
  json += "\"timestamp\":\"" + timestampISO8601(time) + "\"}";
  String Dev_token = String("Device-Token: ") + String(device_token);
  if (client.connected()) {
    Serial.print(" send API ");
    // Make a HTTP request:
    client.println("POST /data? HTTP/1.1");
    client.println("Host: api.tago.io");
    client.println("_ssl: false");  // for non-secured connection, use this option "_ssl: false"
    client.println(Dev_token);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(json.length());
    client.println();
    client.println(json);
    client.println();
    delay(50);
    while (client.available()) {
      client.read();
      delay(10);
    }
    return true;

  } else {
    // if you couldn't make a connection:
    Serial.print(" error API ");
    return false;
  }
}

String dateISO8601(time_t time) {
  sprintf(buff, "%02d%02d%02d", year(time), month(time), day(time));
  return String(buff);
}

String timestampISO8601(time_t time) {
  sprintf(buff, "%02d-%02d-%02dT%02d:%02d:%02d", year(time), month(time), day(time), hour(time), minute(time), second(time));
  return String(buff);
}

byte base[6] = { 0xDE, 0xAD, 0x00, 0x00, 0x00, 0x00 };

void setupMAC(bool force) {

  // Random MAC address stored in EEPROM
  if (EEPROM.read(1) == '#' && force == false) {
    for (int i = 2; i < 6; i++) {
      base[i] = EEPROM.read(i);
    }
  } else {
    randomSeed(analogRead(0));
    for (int i = 2; i < 6; i++) {
      base[i] = random(0, 255);
      EEPROM.write(i, base[i]);
    }
    EEPROM.write(1, '#');
  }
  for (int i = 0; i <= 7; i++) {
    MACAddress[i] = base[i];
  }
}

String MAC2String() {
  char macstr[13];
  String result = "";
  snprintf(macstr, 18, "%02x%02x%02x%02x%02x%02x", MACAddress[0], MACAddress[1], MACAddress[2], MACAddress[3], MACAddress[4], MACAddress[5]);
  for (int i = 0; i < 13; i++) {
    result += String(macstr[i]);
  }
  return result;
}

void reboot() {
  Serial.println("Rebooting...");
  lcd.clear();          // Serve para limpar a tela do display
  lcd.setCursor(0, 0);  // Coloca o cursor do display na coluna 1 e linha 1
  lcd.print("Rebooting...");
  wd_reboot();
}


void logMsg(String msg) {
  lcd.setCursor(STATUS_COL, 0);
  lcd.print(msg.substring(0, 3));
  Serial.println(msg);
}
