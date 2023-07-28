
#include <NTPClient.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <TimeLib.h>
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

// A UDP instance to let us send and receive packets over UDP
EthernetUDP ntpUDP;
NTPClient timeClient(ntpUDP,"pool.ntp.org", -3 * 3600, 60000);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Get IP");
  // start Ethernet and UDP
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
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
  Serial.println("Setup NTP");

  timeClient.begin();
  //timeClient.setTimeOffset(-3 * 60 * 60);
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  
  unsigned long t = timeClient.getEpochTime();
  Serial.println(timeClient.getFormattedTime());

  char buff[32];
  sprintf(buff, "%02d.%02d.%02d %02d:%02d:%02d", year(t), month(t), day(t), hour(t), minute(t), second(t));
  Serial.println(String(buff));
  
}

void loop() {}
 