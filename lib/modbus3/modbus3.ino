#include "SimpleModbusMaster.h"

/*
   The example will use packet1 to read a register from address 0 (the adc ch0 value)
   from the arduino slave (id=1). It will then use this value to adjust the brightness
   of an led on pin 9 using PWM.
   It will then use packet2 to write a register (its own adc ch0 value) to address 1 
   on the arduino slave (id=1) adjusting the brightness of an led on pin 9 using PWM.
*/

//////////////////// Port information ///////////////////
#define baud 9600
#define timeout 1000
#define polling 200 // the scan rate
#define retry_count 10

// used to toggle the receive/transmit pin on the driver
#define TxEnablePin 4 

#define LED 9

// The total amount of available memory on the master to store data
#define TOTAL_NO_OF_REGISTERS 2

// This is the easiest way to create new packets
// Add as many as you want. TOTAL_NO_OF_PACKETS
// is automatically updated.
enum
{
  PACKET1,
  PACKET2,
  TOTAL_NO_OF_PACKETS // leave this last entry
};

// Create an array of Packets to be configured
Packet packets[TOTAL_NO_OF_PACKETS];

// Masters register array
unsigned int regs[TOTAL_NO_OF_REGISTERS];

void setup()
{
  // Initialize each packet
  modbus_construct(&packets[PACKET1], 1, READ_HOLDING_REGISTERS, 0, 1, 0);
  
  // Initialize the Modbus Finite State Machine
  modbus_configure(&Serial2, baud, SERIAL_8N1, timeout, polling, retry_count, TxEnablePin, packets, TOTAL_NO_OF_PACKETS, regs);
  
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  modbus_update();
  Serial.print("REG[0]: ");
  Serial.println(regs[0]);
  delay(1000);

}
