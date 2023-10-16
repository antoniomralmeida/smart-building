#include <PubSubClient.h>
#include <ethernet.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
EthernetClient client;


PubSubClient mqttClient(client);

const char* server = "83734c7eda514823bc0bfaa4a35fa596.s2.eu.hivemq.cloud";

void init_mqtt() {
  mqttClient.setServer(server, 1883);

  if (mqttClient.connect("Arduino", "xxx", "xxx")) {
    Serial.println("Connected");
  } else {
    Serial.print("Connection failed: ");
    Serial.println(mqttClient.state());
  }
}

void setup() {
  Ethernet.begin(mac);
  init_mqtt();

}

void loop() {
  // put your main code here, to run repeatedly:

}
