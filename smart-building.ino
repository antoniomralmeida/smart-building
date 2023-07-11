

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <Wire.h> // Biblioteca utilizada para fazer a comunicação com o I2C
#include <LiquidCrystal_I2C.h> // Biblioteca utilizada para fazer a comunicação com o display 20x4 

#define col 16 // Serve para definir o numero de colunas do display utilizado
#define lin  2 // Serve para definir o numero de linhas do display utilizado
#define ende  0x27 // Módulo I2C para Display LCD.

LiquidCrystal_I2C lcd(ende,col,lin); // Chamada da funcação LiquidCrystal para ser usada com o I2C

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};

#define LED_ERR = 13;

bool Error = false;

EthernetClient client;

void setup() {
  Serial.begin(9600);

  lcd.init(); // Serve para iniciar a comunicação com o display já conectado
  lcd.backlight(); // Serve para ligar a luz do display
  lcd.clear(); // Serve para limpar a tela do display

  lcd.setCursor(0,0); // Coloca o cursor do display na coluna 1 e linha 1
  lcd.print("Booting ..."); // Comando de saída com a mensagem que deve aparecer na coluna 2 e linha 1.
  lcd.setCursor(0,1); // Coloca o cursor do display na coluna 1 e linha 1
  lcd.print(ByetArray2HexString(mac, 6));

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection:

  Serial.println("Initialize Ethernet with DHCP:");

  if (Ethernet.begin(mac) == 0) {

    Serial.println("Failed to configure Ethernet using DHCP");

    if (Ethernet.hardwareStatus() == EthernetNoHardware) {

      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");

    } else if (Ethernet.linkStatus() == LinkOFF) {

      Serial.println("Ethernet cable is not connected.");

    }

    // no point in carrying on, so do nothing forevermore:

    while (true) {

      delay(1);

    }

  }


  // print your local IP address:
  Serial.print("My IP address(SETUP): ");
  Serial.print("client is at ");
  Serial.println(Ethernet.localIP());
  Serial.print("subnetmask is ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("gateway is ");
  Serial.println(Ethernet.gatewayIP());

  lcd.clear(); // Serve para limpar a tela do display
  lcd.setCursor(0,0); // Coloca o cursor do display na coluna 1 e linha 1
  lcd.print( Ethernet.localIP()); // Comando de saída com a mensagem que deve aparecer na coluna 2 e linha 1.

}

void loop() {

  delay(500);
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


String ByetArray2HexString(byte barrya[], int length) {
  String hexstring = "";

  for(int i = 0; i < length; i++) {
   
    hexstring += String(barrya[i], HEX);
  }
  return hexstring;
}
