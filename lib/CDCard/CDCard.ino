/*
  SD card read/write

  This example shows how to read and write data to and from an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  created   Nov 2010
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/
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



ThreeWire myWire(IO, SCLK, CE);  // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

//SoftwareSerial mySerial(RXPIN, TXPIN);

ModbusRTUMaster modbus(Serial1);  // serial port, driver enable pin for rs-485 (optional)

EthernetUDP ntpUDP;
NTPClient timeClient(ntpUDP);

File tmpFile;

File myFile;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1)
      ;
  }
  Serial.println("initialization done.");
}

void loop() {
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.seek(EOF);
    myFile.println("testing 1, 2, 3.");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  // re-open the file for reading:
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}
