
#include <TimeLib.h>       //TimeLib library is needed https://github.com/PaulStoffregen/Time
#include <NtpClientLib.h>  //Include NtpClient library header
#include <Ethernet.h>
#include <EthernetClient.h>
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

EthernetClient client;
void setup() {
  Serial.begin(115200);

  Serial.println("Setup Ethernet");

  while (Ethernet.begin(mac) == 0) {
    Serial.print(".");
  }

  // NTP begin with default parameters:
  //   NTP server: pool.ntp.org
  //   TimeZone: UTC
  //   Daylight saving: off
  Serial.println("Setup NTP");

  while (!NTP.begin("pool.ntp.org", -3, false, 1)) {
    Serial.print(".");
  }
  if (NTP.getTime() == 0) {
    Serial.print("Error NTP server");
    while (1) {}
  }
}

void loop() {
  //To keep time updated you need to call now() from time to time inside loop
  //in this case getTimeDateString() implies a call to now()
  Serial.println(NTP.getTimeDateString());

  String json = "{\"variable\":\"Temperatura\",\"value\":27.70,\"unit\":\"ºC\",\"timestamp\":\"1970-01-01T00:00:00\"}";
  if (client.connect("api.tago.io", 80)) {
    Serial.println("connected");
    client.println("POST /data? HTTP/1.1");
    client.println("Host: api.tago.io");
    client.println("_ssl: false");  // for non-secured connection, use this option "_ssl: false"
    client.println("Device-Token: 6659e588-d073-4f28-99ff-53726c2f1045");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(json.length());
    client.println();
    client.println(json);
    client.println();
    delay(500);
    while (client.available()) {
      char c = client.read();
      Serial.print(c);
    }
  } else {
    Serial.println("error");
  }
  client.stop();
  /*
  Device-Token: 6659e588-d073-4f28-99ff-53726c2f1045
*/
  delay(2000);
}
