#include <Ethernet.h>
#include <EthernetClient.h>
#include <Wire.h>               // Biblioteca utilizada para fazer a comunicação com o I2C
#include <LiquidCrystal_I2C.h>  // Biblioteca utilizada para fazer a comunicação com o display 20x4
#include <EEPROM.h>
#include <EthernetUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include <avr/wdt.h>  // Include the ATmel library
#include <SPI.h>
#include <ThreeWire.h>  //RTC
#include <RtcDS1302.h>  //RTC

#define col 16        // Serve para definir o numero de colunas do display utilizado
#define lin 2         // Serve para definir o numero de linhas do display utilizado
#define LCDADDR 0x27  // Módulo I2C para Display LCD.
#define LED_ERR 13
#define STATUS_COL 13
#define ZERODATE 1367256704

#define I2C 0x01
#define RS485 0x02

#define DHT22T 0x01
#define DHT22H 0x02

typedef struct
{
  byte channelType;
  byte channelAddr;
  byte SensorType;
  byte dataLength;
} SensorData;

SensorData sensors[2];

LiquidCrystal_I2C lcd(LCDADDR, col, lin);  // Chamada da funcação LiquidCrystal para ser usada com o I2C
ThreeWire myWire(4, 5, 2);              //OBJETO DO TIPO ThreeWire
RtcDS1302<ThreeWire> Rtc(myWire);       //OBJETO DO TIPO RtcDS1302

EthernetClient client;
byte MACAddress[8];

// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;
NTPClient timeClient(Udp, "0.pool.ntp.org");

void setup() {
  Serial.begin(9600);

  SetupMAC(false);
  lcd.init();       // Serve para iniciar a comunicação com o display já conectado
  lcd.backlight();  // Serve para ligar a luz do display
  lcd.clear();      // Serve para limpar a tela do display

  lcd.setCursor(0, 0);      // Coloca o cursor do display na coluna 1 e linha 1
  lcd.print("Booting...");  // Comando de saída com a mensagem que deve aparecer na coluna 2 e linha 1.


  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  lcd.setCursor(STATUS_COL, 0);
  lcd.print("S01");  //Step 1
  Serial.println("S01 - Initialize Ethernet with DHCP");

  lcd.setCursor(0, 1);  // Coloca o cursor do display na coluna 1 e linha 1
  lcd.print(MAC2String());


  // start the Ethernet connection:
  if (Ethernet.begin(MACAddress) == 0) {
    lcd.setCursor(STATUS_COL, 0);
    lcd.print("E01");  //Error 1
    Serial.println("E01 - Failed to configure Ethernet using DHCP");
    delay(1000);

    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      lcd.setCursor(0, 11);
      lcd.print("E02");  //Error 2
      Serial.println("E02 - Ethernet shield was not found.");
    } else if (Ethernet.linkStatus() == LinkOFF) {
      lcd.setCursor(STATUS_COL, 0);
      lcd.print("E03");  //Error 3
      Serial.println("E03 - Ethernet cable is not connected.");
    }
    reboot();
  }

  // print your local IP address:
  Serial.print("My IP address(SETUP): ");
  Serial.print("client is at ");
  Serial.println(Ethernet.localIP());
  Serial.print("subnetmask is ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("gateway is ");
  Serial.println(Ethernet.gatewayIP());

  lcd.setCursor(STATUS_COL, 0);
  lcd.print("S03");  //Step 3
  Serial.println("S03 - Setup NTP");

  unsigned long ntp = getNTP();
  if (ntp == "") {
    lcd.print("E04");  //Error 3
    Serial.println("E04 - NTP Server not found.");
    reboot();
  }
  Serial.println("NTP: " + printDate(ntp));

  lcd.setCursor(STATUS_COL, 0);
  lcd.print("S04");  //Step 4
  Serial.println("S04 - Setup RTC");

  if (Rtc.GetIsWriteProtected()) {                                                          //SE O RTC ESTIVER PROTEGIDO CONTRA GRAVAÇÃO, FAZ
    Serial.println("RTC está protegido contra gravação. Habilitando a gravação agora...");  //IMPRIME O TEXTO NO MONITOR SERIAL
    Rtc.SetIsWriteProtected(false);                                                         //HABILITA GRAVAÇÃO NO RTC
    Serial.println();                                                                       //QUEBRA DE LINHA NA SERIAL
  }

  if (!Rtc.GetIsRunning()) {                                                           //SE RTC NÃO ESTIVER SENDO EXECUTADO, FAZ
    Serial.println("RTC não está funcionando de forma contínua. Iniciando agora...");  //IMPRIME O TEXTO NO MONITOR SERIAL
    Rtc.SetIsRunning(true);                                                            //INICIALIZA O RTC
    Serial.println();                                                                  //QUEBRA DE LINHA NA SERIAL
  }

  unsigned long rtc = Rtc.GetDateTime().Epoch32Time();
  Serial.println("RTC: " + printDate(rtc));

  if (!orderDate(rtc).equals( orderDate(ntp))) {                                                             //SE A INFORMAÇÃO REGISTRADA FOR MENOR QUE A INFORMAÇÃO COMPILADA, FAZ
    Serial.println("As informações atuais do RTC estão desatualizadas. Atualizando informações...");  //IMPRIME O TEXTO NO MONITOR SERIAL
    //RtcDateTime new = RtcDateTime(ntp);
    Rtc.SetDateTime(RtcDateTime (ntp));  //INFORMAÇÕES COMPILADAS SUBSTITUEM AS INFORMAÇÕES ANTERIORES
    Serial.println();      //QUEBRA DE LINHA NA SERIAL
  } else {                 //SENÃO, SE A INFORMAÇÃO REGISTRADA FOR MAIOR QUE A INFORMAÇÃO COMPILADA, FAZ
    Serial.println("As informações atuais do RTC estão atualizadas.");
    Serial.println();  //QUEBRA DE LINHA NA SERIAL
  }

  lcd.setCursor(STATUS_COL, 0);
  lcd.print("S05");  //Step 5
  Serial.println("S05 - Read Data Setup");

  sensors[0].channelType = I2C;
  sensors[0].channelAddr = 0X08;
  sensors[0].SensorType = DHT22T;
  sensors[0].dataLength = 4;

  sensors[1].channelType = I2C;
  sensors[1].channelAddr = 0X08;
  sensors[1].SensorType = DHT22H;
  sensors[1].dataLength = 4;


  lcd.clear();                    // Serve para limpar a tela do display
  lcd.setCursor(0, 0);            // Coloca o cursor do display na coluna 1 e linha 1
  lcd.print(Ethernet.localIP());  // Comando de saída com a mensagem que deve aparecer na coluna 2 e linha 1.
  Serial.println("OK.");
}

int oneminute = 0;
void loop() {

  unsigned long rtc = Rtc.GetDateTime().Epoch32Time();
  if (rtc != ZERODATE) {
    char buff2[32];
    sprintf(buff2, "%02d:%02d:%02d", hour(rtc), minute(rtc), second(rtc));
    lcd.setCursor(0, 1);
    lcd.print(buff2);

    if (oneminute == 0) {
      char buff[32];
      sprintf(buff, "%02d.%02d.%02d %02d:%02d:%02d", day(rtc), month(rtc), year(rtc), hour(rtc), minute(rtc), second(rtc));
      Serial.println(buff);
      ProcessChannelRead();
    }
  }
  oneminute = (oneminute + 1) % 60;
  Serial.println(oneminute);
  delay(1000);
}

void ProcessChannelRead() {
  for (int i = 0; i < sizeof(sensors); i++) {
    if (sensors[i].channelType == I2C) {
        Wire.begin(sensors[i].channelAddr);
        Wire.write(sensors[i].SensorType);
        Wire.endTransmission();
        delay(100);
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

unsigned long getNTP() {
  timeClient.begin();
  timeClient.setTimeOffset(-3 * 3600);
  if (timeClient.update()) {
    unsigned long t = timeClient.getEpochTime();
    char buff[32];
    //sprintf(buff, "%02d.%02d.%02d %02d:%02d:%02d", month(t), day(t), year(t), hour(t), minute(t), second(t));
    return t;
  }
  return 0;
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
