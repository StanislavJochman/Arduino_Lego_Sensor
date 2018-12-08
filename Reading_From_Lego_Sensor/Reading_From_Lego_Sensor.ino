#include <SoftwareSerial.h>
SoftwareSerial mySerial(3, 2); // RX, TX

int SYNC = 0x00; // Synchronization byte
int ACK = 0x04; // Acknowledge byte
int NACK = 0x02; // Not acknowledge byte
int payloadLookup []= {1, 2, 4, 8, 16, 32, 0, 0};
int messageLength = 0;
int lastRefresh = millis();
int message [34]= {};
int SerialValue [] = {0,0};
int color = 0;
int refreshTime = 1;
bool checkSumError = false;


long lastupdate = millis();
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  mySerial.begin(2400);
  for (int x=0;x<34;x++){
    message[x] = x;
  }
  delay(50);
  SensorSetup();
  
 
}

void loop() {
    if (millis() - lastRefresh > refreshTime){
      SensorMode(2);
      lastRefresh = millis();
    }
    color = mySerial.read();
    if(color==-1 && color==0 && color==255){
      mySerial.read();
    }
    if(color!=-1 && color < 8){
      Serial.print((millis()-lastupdate));
      Serial.print("ms          ");
      Serial.println(color);
      lastupdate = millis();
    }
}
void SensorSetup(){
  while (true){
  int value = mySerial.read();
  if(value==-1 && value==0 && value==255){
    mySerial.read();
  }
  if(value!=-1){
    SerialValue[1] = SerialValue[0];
    SerialValue[0] = value;
  }
  
  if(SerialValue[0]==128 && SerialValue[1]==144){
    while (true){
      if(mySerial.read()==4){
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
void SensorMode(int newMode){
  if (newMode <= 5 && newMode >= 0){
    sendMessage(0x44, 0x11);
    for (int n;n<3;n++){
      sendMessage(0x43, newMode & 0x7);
      mySerial.write(NACK);
    }
  }
}
void sendMessage(int cmd,int data){
  int cSum = 0xff ^ cmd ^ data;
  mySerial.write(cmd);
  mySerial.write(data);
  mySerial.write(cSum);
}

