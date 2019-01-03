#include <SoftwareSerial.h>
SoftwareSerial mySerial(3, 2);

int ACK = 0x04; // Acknowledge byte
int NACK = 0x02; // Not acknowledge byte

int SensorDisconnected = 0;
int lastRefresh = millis();
int SerialValue [] = {0,0,0,0};
int refreshTime = 300;
long lastupdate = millis();

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  mySerial.begin(2400);
  SensorSetup();
  
 
}

void loop() {
    if (millis() - lastRefresh > refreshTime){
      SensorMode(2);
      lastRefresh = millis();
    }
    SerialValue[2] = mySerial.read();
    if(SerialValue[2]==-1 && SerialValue[2]==255){
      SensorDisconnected ++;
      mySerial.read();
    }
    if(SerialValue[2]!=-1 && SerialValue[2] <= 100 && SerialValue[3]==0){
      SensorDisconnected = 0;
      SerialValue[3] = 1;
      Serial.print(SerialValue[2]);
      Serial.print("          ");
      Serial.print((millis()-lastupdate));
      Serial.println("ms");
      lastupdate = millis();
    }
    else{
      SerialValue[3] = 0; 
    }
    if(SensorDisconnected > 10){
      Serial.println("Sensor Disconnected");
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

