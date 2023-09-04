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

#define SLAVE_ADDR  1
#define REGISTER 0x00
#define BAUD 9600
#define LED 13


unsigned long now;
unsigned long lastReqTs = 0;

#define SERIAL_PORT_HARDWARE Serial1

void setup() {
  /**
   * Init serial port used for logging
   */

  SERIAL_PORT_MONITOR.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 0);
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
  ModbusRtuMaster.begin(&SERIAL_PORT_HARDWARE, BAUD,52);

  /**
   * Set callback function to handle responses
   */
  ModbusRtuMaster.setCallback(&onResponse);
}

void loop() {

  //ModbusRtuMaster.readInputRegisters(SLAVE_ADDR, REGISTER, 1);
  Serial.print("readHoldingRegisters(");
  Serial.print(SLAVE_ADDR);
  Serial.print(",");
  Serial.println(REGISTER);
  ModbusRtuMaster.readHoldingRegisters(SLAVE_ADDR, REGISTER, 1);

  ModbusRtuMaster.process();
  //SLAVE_ADDR++;
  delay(5000);
}

byte onResponse(byte unitAddr, byte function, size_t len, byte *data) {
  digitalWrite(LED, 1);
  SERIAL_PORT_MONITOR.print("Response from ");
  SERIAL_PORT_MONITOR.println(unitAddr);
  SERIAL_PORT_MONITOR.println(len);
  SERIAL_PORT_MONITOR.print("Data: ");
  for (int i = 0; i < len; i++) {
    SERIAL_PORT_MONITOR.print(data[i]);
    SERIAL_PORT_MONITOR.print(" ");
  }
  SERIAL_PORT_MONITOR.println("\n");
  digitalWrite(LED, 0);
}
