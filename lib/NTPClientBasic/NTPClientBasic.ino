
#include <TimeLib.h>       //TimeLib library is needed https://github.com/PaulStoffregen/Time
#include <NtpClientLib.h>  //Include NtpClient library header
#include <Ethernet.h>
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

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

  while (!NTP.begin("pool.ntp.org", -3,false, 1)) {
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

  delay(1000);
}
