#define MAX485_DE_RE 4

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1);
  pinMode(MAX485_DE_RE, OUTPUT);
  digitalWrite(MAX485_DE_RE, 0);
}

void loop() {
  byte cmd[] = { 0X01, 0X03, 0X00, 0X00, 0X00, 0X01, 0X84, 0X0A };
  digitalWrite(MAX485_DE_RE, 1);
  for (int i = 0; i < sizeof(cmd); i++) {
    Serial2.write(cmd[i]);
  }
  digitalWrite(MAX485_DE_RE, 0);
  delay(100);
  byte ret[255];
  for (int i = 0; i < sizeof(ret); i++) {
    ret[i] = 0;
  }
  for (int i = 0; i < 3; i++) {
    if (Serial2.available()) {
      ret[i] = Serial2.read();
    }
  }
  if (ret[0] == cmd[0] && ret[1] == cmd[1]) {
    for (int i = 0; i < (ret[2] + 2); i++) {
      ret[3 + i] = Serial2.read();
    }
  } else {
    while (Serial2.available()) {
      Serial2.read();
    }
  }
  for (int i = 0; i < (ret[2] + 5); i++) {
    Serial.print(ret[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  delay(2000);
}
