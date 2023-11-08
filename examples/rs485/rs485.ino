
void setup() {
  Serial.begin(9600, SERIAL_8N1);
  pinMode(13, OUTPUT);
  digitalWrite(13,LOW);
}

void loop() {
  byte cmd[] = { 0X01, 0X03, 0X00, 0X00, 0X00, 0X01, 0X84, 0X0A };
  for (int i = 0; i < sizeof(cmd); i++) {
    Serial.write(cmd[i]);
  }
  delay(100);
  byte ret[255];
  for (int i = 0; i < sizeof(ret); i++) {
    ret[i] = 0;
  }
  for (int i = 0; i < 3; i++) {
    if (Serial.available()) {
      ret[i] = Serial.read();
    }
  }
  if (ret[0] == cmd[0] && ret[1] == cmd[1]) {
    digitalWrite(13,HIGH); 
  
    for (int i = 0; i < (ret[2] + 2); i++) {
      ret[3 + i] = Serial.read();
    }
  } else {
    while (Serial.available()) {
      Serial.read();
    }
  }
  delay(2000);
}
