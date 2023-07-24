/*  
  ModbusMaster.ino - Arduino sketch showing the use of the ModbusRtuMaster library

    Copyright (C) 2020 Sfera Labs S.r.l. - All rights reserved.
  
  This code is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  See file LICENSE.txt for further informations on licensing terms.
*/

#include <ModbusRtuMaster.h>

#define SLAVE_ADDR 1
#define REGISTER 0x0002
#define BAUD 9600
#define PIN_TX_ENABLE 4

unsigned long now;
unsigned long lastReqTs;

#define SERIAL_PORT_HARDWARE Serial1

void setup() {
  /**
   * Init serial port used for logging
   */

  SERIAL_PORT_MONITOR.begin(9600);

  /**
   * Init serial port used for Modbus communication
   * baud rate: BAUD
   * data bits: 8
   * parity: none
   * stop bits: 2
   */
  SERIAL_PORT_HARDWARE.begin(BAUD, SERIAL_8N1);

  /*
   * Init the Modbus master
   */
  ModbusRtuMaster.begin(&SERIAL_PORT_HARDWARE, BAUD, PIN_TX_ENABLE);

  /**
   * Set callback function to handle responses
   */
  ModbusRtuMaster.setCallback(&onResponse);
}

void loop() {
  // Perform a request every 3 seconds
  unsigned long now = millis();
  if (now - lastReqTs >= 3000) {
    //ModbusRtuMaster.readInputRegisters(SLAVE_ADDR, REGISTER, 1);
    //ModbusRtuMaster.readHoldingRegisters(SLAVE_ADDR, REGISTER, 1);
    Serial.println("Send command...");
    byte cmd[] = { 0X01, 0X03, 0X00, 0X00, 0X00, 0X01, 0X84, 0X0A };
    digitalWrite(PIN_TX_ENABLE, 1);
    for (int i = 0; i < sizeof(cmd); i++) {
      Serial1.write(cmd[i]);
    }
    digitalWrite(PIN_TX_ENABLE, 0);
    lastReqTs = now;
  }

  ModbusRtuMaster.process();
}

byte onResponse(byte unitAddr, byte function, size_t len, byte *data) {
  SERIAL_PORT_MONITOR.print("Response from ");
  SERIAL_PORT_MONITOR.println(unitAddr);
  SERIAL_PORT_MONITOR.println(len);
  SERIAL_PORT_MONITOR.print("Data: ");
  for (int i = 0; i < len; i++) {
    SERIAL_PORT_MONITOR.print(data[i]);
    SERIAL_PORT_MONITOR.print(" ");
  }
  SERIAL_PORT_MONITOR.println("\n");
}
