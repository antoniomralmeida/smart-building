
#include <TimeLib.h>  //TimeLib library is needed https://github.com/PaulStoffregen/Time
#include <NTPClient.h>
#include <Ethernet.h>
#include <EthernetClient.h>
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

EthernetClient client;

EthernetUDP ntpUDP;
NTPClient timeClient(ntpUDP);

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


  timeClient.begin();
  timeClient.setTimeOffset(-3 * 3600);
  if (!timeClient.update()) {
    Serial.println("Error NPT");
    while (1)
      ;
  }
  if (!timeClient.isTimeSet()) {
    Serial.println("Error NPT");
    while (1)
      ;
  }
  Serial.println(timeClient.getFormattedTime());



  if (client.connect("api.tago.io", 80)) {
    Serial.println("connected");
  } else {
    Serial.println("error");
  }
}

void loop() {
  time_t rtc = timeClient.getEpochTime();
  char buff[64];
  sprintf(buff, "%02d.%02d.%02d %02d:%02d:%02d", day(rtc), month(rtc), year(rtc), timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());
  Serial.println(buff);

  Serial.println(timeClient.getEpochTime() / 1000);
/*
  float temp = random(10000) / 100.0;
  String json = "{\"variable\":\"Temperatura\",\"value\":" + String(temp) + ",\"unit\":\"ºC\",\"timestamp\":\"1970-01-01T00:00:00\"}";
  if (client.availableForWrite()) {
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

  } else {
    Serial.println("error");
  }
*/
  /*
  Device-Token: 6659e588-d073-4f28-99ff-53726c2f1045
*/
  delay(60000);
}
