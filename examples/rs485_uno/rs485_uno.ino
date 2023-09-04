#include <SoftwareSerial.h>

#define rxPin 10
#define txPin 11

// Set up a new SoftwareSerial object
SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);

RS485 RS485(&mySerial, 52, 1);

void setup()  {
    // Define pin modes for TX and RX
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    Serial.begin(9600, SERIAL_8N1);
    
    // Set the baud rate for the SoftwareSerial object

    mySerial.begin(9600);
    
    Serial.println("SoftwareSerial.h");
    
}

void loop() {
    while (mySerial.available() > 0) {
        char c = mySerial.read();
        Serial.print(c, HEX);
        Serial.print(" ");
        //mySerial.print(c);
    }
    Serial.println();
    delay(500);
}