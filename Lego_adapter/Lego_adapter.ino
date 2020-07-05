#include <SoftwareSerial.h>
#include <Wire.h>
#include <TimerOne.h>
SoftwareSerial mySerial(3, 2);

int ACK = 0x04;
int NACK = 0x02;

int SensorDisconnected = 0;
int lastRefresh = millis();
int SerialValue [] = {0, 0, 0, 0};
int refreshTime = 30;
long lastupdate = millis();
int value = 0;
uint8_t mode = 2;
void setup() {
  Serial.begin(9600);
  mySerial.begin(2400);
  SensorSetup();
  Wire.begin(0x10);
  Timer1.initialize(30000);
  Timer1.attachInterrupt(ackSend);
  Wire.onRequest(request);
  Wire.onReceive(receive);

}

void loop() {
  value = ReadData();

}
int ReadData() {
  SerialValue[2] = mySerial.read();
  if (SerialValue[2] == -1 && SerialValue[2] == 255) {
    SensorDisconnected ++;
    mySerial.read();
  }
  if (SerialValue[2] != -1 && SerialValue[3] == 0) {
    if((mode == 2 || mode == 4) && SerialValue[2] <= 7){
      SensorDisconnected = 0;
      SerialValue[3] = 1;
      lastupdate = millis();
      return SerialValue[2];
    }
    else if((mode == 0 || mode == 1 || mode == 3) && SerialValue[2] <= 100){
      SensorDisconnected = 0;
      SerialValue[3] = 1;
      lastupdate = millis();
      return SerialValue[2];
    }
    else if(mode == 5){
      SensorDisconnected = 0;
      SerialValue[3] = 1;
      lastupdate = millis();
      return 0;
    }

  }
  else {
    SerialValue[3] = 0;
  }
}
void SensorSetup() {
  while (true) {
    int value = mySerial.read();
    if (value == -1 && value == 0 && value == 255) {
      mySerial.read();
    }
    if (value != -1) {
      SerialValue[1] = SerialValue[0];
      SerialValue[0] = value;
    }

    if (SerialValue[0] == 128 && SerialValue[1] == 144) {
      while (true) {
        if (mySerial.read() == 4) {
          break;
        }
      }
      mySerial.write(ACK);
      delay(60);
      mySerial.end();
      mySerial.begin(57600);
      break;
    }
  }
}
void SensorMode(int newMode) {
  if (newMode <= 5 && newMode >= 0) {
    sendMessage(0x44, 0x11);
    for (int n = 0; n < 3; n++) {
      sendMessage(0x43, newMode & 0x7);
      mySerial.write(NACK);
    }
  }
}
void sendMessage(int cmd, int data) {
  int cSum = 0xff ^ cmd ^ data;
  mySerial.write(cmd);
  mySerial.write(data);
  mySerial.write(cSum);
}
void ackSend() {
  SensorMode(mode);
}
void request() {
  Wire.write(value);
  Wire.write((value >> 8));
}
void receive() {
  mode = Wire.read();
}
