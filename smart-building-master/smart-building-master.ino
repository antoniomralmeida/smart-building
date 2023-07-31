#include <Ethernet.h>
#include <EthernetClient.h>
#include <LiquidCrystal_I2C.h>  // Biblioteca utilizada para fazer a comunicação com o display 20x4
#include <EEPROM.h>
#include <avr/wdt.h>  // Include the ATmel library
#include <SPI.h>
#include <DS1302.h>  //RTC
#include <Wire.h>    // Biblioteca utilizada para fazer a comunicação com o I2C
#include <smart-building.h>
#include <SD.h>
#include <NtpClientLib.h>

#define COLS 16       // Serve para definir o numero de colunas do display utilizado
#define ROWS 2        // Serve para definir o numero de linhas do display utilizado
#define LCDADDR 0x27  // Módulo I2C para Display LCD.
#define LED_ERR 13
#define STATUS_COL 13
#define SDCARD_SS_PIN 4
#define MAX_SENSORS 128
#define BOUNDRATE_SERIAL 115200
#define CE 2
#define IO 4
#define SCLK 5

sensorData sensors[MAX_SENSORS];
byte nSensors = 0;

LiquidCrystal_I2C lcd(LCDADDR, COLS, ROWS);  // Chamada da funcação LiquidCrystal para ser usada com o I2C
EthernetClient client;
byte MACAddress[8];

File myFile;

DS1302 Rtc(CE, IO, SCLK);


void setup() {
  String version = "SMART-BUILDING V 1.0.0";
  Serial.begin(BOUNDRATE_SERIAL);
  Serial.println();
  Serial.print(version);
  Serial.print(__DATE__);
  Serial.print(__TIME__);
  Serial.println();

  SetupMAC(false);
  lcd.init();       // Serve para iniciar a comunicação com o display já conectado
  lcd.backlight();  // Serve para ligar a luz do display
  lcd.clear();      // Serve para limpar a tela do display

  lcd.setCursor(0, 1);  // Coloca o cursor do display na coluna 1 e linha 1
  lcd.print(version);

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
  NTP.stop();
  delay(1000);
  if (!NTP.begin(DEFAULT_NTP_SERVER, -3)) {
    logMsg("E04 - NTP Setup error!");
    reboot();
  }
  delay(1000);
  if (NTP.getTime() < 1000) {
    logMsg("E05 - NTP Server not found!");
    reboot();
  }
  Serial.println(NTP.getTimeDateString());

  logMsg("S04 - Setup RTC");
  Rtc.writeProtect(false);
  Rtc.halt(false);
  time_t now = NTP.getTime();
  Time t(year(now), month(now), day(now), hour(now), minute(now), second(now), dayOfWeek(now));
  Rtc.time(t);
  Time rtc = Rtc.time();

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
  String data = "";
  while (myFile.available()) {
    byte column = 0;
    byte pt = 0;
    while (myFile.available()) {
      if (inputChar == '\n' || inputChar == ',') {
        switch (column) {
          case 0:
            {
              sensors[nSensors].channelType = findBusTypeStr(data);
              break;
            }
          case 1:
            {
              sensors[nSensors].channelAddr = data.toInt();
              break;
            }
          case 2:
            {
              sensors[nSensors].sensorType = findSensorTypeStr(data);
              break;
            }
          case 3:
            {
              sensors[nSensors].dataLength = data.toInt();
              break;
            }
        }
        column++;
        data = "";
      } else {
        data += inputChar;
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

  lcd.clear();                    // Serve para limpar a tela do display
  lcd.setCursor(0, 0);            // Coloca o cursor do display na coluna 1 e linha 1
  lcd.print(Ethernet.localIP());  // Comando de saída com a mensagem que deve aparecer na coluna 2 e linha 1.
  Serial.println("OK.");
  Wire.begin();
}

int oneminute = 0;
void loop() {
  char buff[32];
  Time rtc = Rtc.time();
  if (now != 0) {
    if (oneminute == 0) {
      Serial.println();
      sprintf(buff, "%02d.%02d.%02d %02d:%02d:%02d", rtc.date, rtc.mon, rtc.yr, rtc.hr, rtc.min, rtc.sec);
      Serial.print(buff);
      ProcessChannelRead();
    }
    sprintf(buff, "%02d:%02d:%02d", rtc.hr, rtc.min, rtc.sec);
    lcd.setCursor(0, 1);
    lcd.print(buff);
    Serial.print(".");
  }
  oneminute = (oneminute + 1) % 60;
  delay(1000);
}

void ProcessChannelRead() {
  Serial.println(", processChannelRead");
  for (int i = 0; i < nSensors; i++) {
    if (sensors[i].channelType == I2C) {
      Wire.beginTransmission(sensors[i].channelAddr);
      Wire.write(sensors[i].sensorType);
      Wire.endTransmission();
      delay(100);
      Wire.requestFrom((int)sensors[i].channelAddr, sensors[i].dataLength);
      int timeOut = 0;
      while (Wire.available() == 0 && timeOut++ < 100) {
        delay(100);
      }
      if (Wire.available()) {
        Serial.println("Receiving from Slave");
        byte buff[sensors[i].dataLength];
        for (int i = 0; i < sensors[i].dataLength; i++) {
          buff[i] = Wire.read();
        }
        float data = *((float *)&buff[0]);
        Serial.println(data, 10);
      }
    }
  }
}

unsigned long lastConnectionTime = 0;  // last time you connected to the server, in milliseconds

// this method makes a HTTP connection to the server:
void IOTSendTagoIO(String device_token, String variable, String value) {
  char server[] = "api.tago.io";
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  String PostData = String("{\"variable\":\"") + variable + String("\", \"value\":") + String(value) + String(",\"unit\":\"C\"}");
  String Dev_token = String("Device-Token: ") + String(device_token);
  if (client.connect(server, 80)) {  // we will use non-secured connnection (HTTP) for tests
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("POST /data? HTTP/1.1");
    client.println("Host: api.tago.io");
    client.println("_ssl: false");  // for non-secured connection, use this option "_ssl: false"
    client.println(Dev_token);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(PostData.length());
    client.println();
    client.println(PostData);
    // note the time that the connection was made:
    lastConnectionTime = millis();
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}

byte base[6] = { 0xDE, 0xAD, 0x00, 0x00, 0x00, 0x00 };

void SetupMAC(bool force) {

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




String orderDate(unsigned long t) {
  char buff[32];
  sprintf(buff, "%02d.%02d.%02d %02d:%02d:%02d", year(t), month(t), day(t), hour(t), minute(t), second(t));
  return String(buff);
}

String printDate(unsigned long t) {
  char buff[32];
  sprintf(buff, "%02d.%02d.%02d %02d:%02d:%02d", day(t), month(t), year(t), hour(t), minute(t), second(t));
  return String(buff);
}


void reboot() {
  Serial.println("Rebooting...");
  lcd.clear();          // Serve para limpar a tela do display
  lcd.setCursor(0, 0);  // Coloca o cursor do display na coluna 1 e linha 1
  lcd.print("Rebooting...");
  delay(1000);
  wdt_disable();        // Deactivates the function while configuring the time in which it will be reset
  wdt_enable(WDTO_2S);  // We set the timer to restart in 2s
  while (true) {
    delay(1);
  }
}


void logMsg(String msg) {
  lcd.setCursor(STATUS_COL, 0);
  lcd.print(msg.substring(1, 3));
  Serial.println(msg);
}
