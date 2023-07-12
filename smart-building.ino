

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <Wire.h> // Biblioteca utilizada para fazer a comunicação com o I2C
#include <LiquidCrystal_I2C.h> // Biblioteca utilizada para fazer a comunicação com o display 20x4 
#include <EEPROM.h>
#include <NTPClient.h>
#include <EthernetUdp.h>


#define col 16 // Serve para definir o numero de colunas do display utilizado
#define lin  2 // Serve para definir o numero de linhas do display utilizado
#define ende  0x27 // Módulo I2C para Display LCD.

LiquidCrystal_I2C lcd(ende,col,lin); // Chamada da funcação LiquidCrystal para ser usada com o I2C


#define LED_ERR    13
#define STATUS_COL 13

bool Error = false;

EthernetClient client;
byte MACAddress[8];

// Define NTP Client to get time

EthernetUDP ntpUDP;
NTPClient timeClient(ntpUDP, "0.pool.ntp.org");

void setup() {
  Serial.begin(9600);

  SetupMAC(MACAddress, false);
  lcd.init(); // Serve para iniciar a comunicação com o display já conectado
  lcd.backlight(); // Serve para ligar a luz do display
  lcd.clear(); // Serve para limpar a tela do display

  lcd.setCursor(0,0); // Coloca o cursor do display na coluna 1 e linha 1
  lcd.print("Booting..."); // Comando de saída com a mensagem que deve aparecer na coluna 2 e linha 1.
  lcd.setCursor(0,1); // Coloca o cursor do display na coluna 1 e linha 1
  lcd.print(MAC2String(MACAddress));


  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  lcd.setCursor(STATUS_COL,0);
  lcd.print("S01"); //Step 1
  Serial.println("S01 - Initialize Ethernet with DHCP:");

  // start the Ethernet connection:
  if (Ethernet.begin(MACAddress) == 0) {
    lcd.setCursor(STATUS_COL,0);
    lcd.print("E01"); //Error 1

    Serial.println("E01 - Failed to configure Ethernet using DHCP");

    if (Ethernet.hardwareStatus()  == EthernetNoHardware) {
      lcd.setCursor(0,11);
      lcd.print("E02"); //Error 2
      Serial.println("E02 - Ethernet shield was not found.  Sorry, can't run without hardware. :(");

    } else if (Ethernet.linkStatus() == LinkOFF) {
      lcd.setCursor(STATUS_COL,0);
      lcd.print("E03"); //Error 3
      Serial.println("E03 - Ethernet cable is not connected.");

    }

    // no point in carrying on, so do nothing forevermore:
    while (true) {
      delay(1);
    }

  }
  lcd.setCursor(STATUS_COL,0);
  lcd.print("S02"); //Step 2
  Serial.println("S02 - Get IP");

  // print your local IP address:
  Serial.print("My IP address(SETUP): ");
  Serial.print("client is at ");
  Serial.println(Ethernet.localIP());
  Serial.print("subnetmask is ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("gateway is ");
  Serial.println(Ethernet.gatewayIP());
  
  lcd.setCursor(STATUS_COL,0);
  lcd.print("S03"); //Step 3
  Serial.println("S03 - Setup NTP");
  setupNTP();


  lcd.clear(); // Serve para limpar a tela do display
  lcd.setCursor(0,0); // Coloca o cursor do display na coluna 1 e linha 1
  lcd.print( Ethernet.localIP()); // Comando de saída com a mensagem que deve aparecer na coluna 2 e linha 1.
  
}

void loop() {
 timeClient.update();

  Serial.print("Day: ");
  Serial.println(timeClient.getDay());  


  delay(1000);
}



unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds

// this method makes a HTTP connection to the server:
void IOTSendTagoIO(String device_token, String variable, String value) {
    char server[] = "api.tago.io";
    // close any connection before send a new request.
    // This will free the socket on the WiFi shield
    client.stop();

    Serial.println("\nStarting connection to server...");
    // if you get a connection, report back via serial:
    String PostData = String("{\"variable\":\"") + variable + String("\", \"value\":") + String(value)+ String(",\"unit\":\"C\"}");
    String Dev_token = String("Device-Token: ")+ String(device_token);
    if (client.connect(server,80)) {                      // we will use non-secured connnection (HTTP) for tests
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("POST /data? HTTP/1.1");
    client.println("Host: api.tago.io");
    client.println("_ssl: false");                        // for non-secured connection, use this option "_ssl: false"
    client.println(Dev_token);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(PostData.length());
    client.println();
    client.println(PostData);
    // note the time that the connection was made:
    lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}

  byte base[6] = { 0xDE, 0xAD, 0x00, 0x00, 0x00, 0x00 };

void SetupMAC(byte newmac[], bool force) {

  // Random MAC address stored in EEPROM
  if (EEPROM.read(1) == '#' && force == false) {
    for (int i = 2; i < 6; i++) {
      base[i] = EEPROM.read(i);
    }
  } else
   {
    randomSeed(analogRead(0));
    for (int i = 2; i < 6; i++) {
      base[i] = random(0, 255);
      EEPROM.write(i, base[i]);
    }
    EEPROM.write(1, '#');
  }
  for (int i=8;i<8;i++) {
    newmac[i] = base[i];
  }
}

String  MAC2String(byte mac[]) {
  char macstr[13];
  String result = "";
  snprintf(macstr, 18, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  for (int i=0;i<13;i++) {
    result += String(macstr[i]);
  }
  return result;
}

void setupNTP()
{
    unsigned int localPort = 8888;
    //Inicializa o client NTP
    timeClient.begin(localPort);    
    //Espera pelo primeiro update online
    while(!timeClient.update())
    {
        Serial.print(".");
        delay(500);
    }
}
