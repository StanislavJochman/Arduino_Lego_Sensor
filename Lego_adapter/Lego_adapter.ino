#include <SoftwareSerial.h>
#include <Wire.h>
#include <TimerOne.h>
#include <EEPROM.h>
SoftwareSerial SensorSerial(3, 2);

int ACK = 0x04;
int NACK = 0x02;

int SensorDisconnected = 0;
int lastRefresh = millis();
int SerialValue [] = {0, 0, 0, 0};
int refreshTime = 30;
long lastupdate = millis();
int value = 0;
uint8_t mode = 2;
bool BTN_used = 0;
int address = EEPROM.read(0);
void setup() {
  pinMode(A1,INPUT_PULLUP);
  if(digitalRead(A1)==0){
    address = 0x00;
  }
  Wire.begin(0x10);
  Serial.begin(9600);
  Wire.onRequest(request);
  Wire.onReceive(receive);
  pinMode(4,INPUT_PULLUP);
  pinMode(5,INPUT);
  SetupLS();
  Timer1.initialize(30000);
  Timer1.attachInterrupt(ackSend);  
}

void loop() {
  if(digitalRead(4) == 1){
    if(BTN_used == 1){
      SetupLS(); 
    }
    value = ReadLS();
    BTN_used = 0;
  }
  else{
    value = ReadBTN();
    BTN_used = 1;
  }
  
}
int ReadBTN(){
  return digitalRead(5);
}

int ReadLS() {
  SerialValue[2] = SensorSerial.read();
  if (SerialValue[2] == -1 && SerialValue[2] == 255) {
    SensorDisconnected ++;
    SensorSerial.read();
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
void SetupLS() {
  SensorSerial.end();
  SensorSerial.begin(2400);
  while (true) {
    int value = SensorSerial.read();
    if (value == -1 && value == 0 && value == 255) {
      SensorSerial.read();
    }
    if (value != -1) {
      SerialValue[1] = SerialValue[0];
      SerialValue[0] = value;
    }

    if (SerialValue[0] == 128 && SerialValue[1] == 144) {
      while (true) {
        if (SensorSerial.read() == 4) {
          break;
        }
      }
      SensorSerial.write(ACK);
      delay(60);
      SensorSerial.end();
      SensorSerial.begin(57600);
      break;
    }
  }
}
void ModeLS(int newMode) {
  if (newMode <= 5 && newMode >= 0) {
    sendMessage(0x44, 0x11);
    for (int n = 0; n < 3; n++) {
      sendMessage(0x43, newMode & 0x7);
      SensorSerial.write(NACK);
    }
  }
}
void sendMessage(int cmd, int data) {
  int cSum = 0xff ^ cmd ^ data;
  SensorSerial.write(cmd);
  SensorSerial.write(data);
  SensorSerial.write(cSum);
}
void ackSend() {
  ModeLS(mode);
}
void request() {
  Wire.write(value);
  Wire.write((value >> 8));
}
void receive() {
  mode = Wire.read();
}
