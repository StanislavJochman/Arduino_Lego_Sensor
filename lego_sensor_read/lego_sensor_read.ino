#include <Wire.h>
void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.setTimeout(10);
}
/*
modes

0 - red light
1 - blue light
2 - color mode
3 - red light (not stable,not used)
4 - color mode (not stable,not used)
5 - off
*/

long prev = millis();
String serialInput;
int data;
void loop() {
  if (Serial.available() > 0) {
    serialInput = Serial.readString();
    data = serialInput.toInt();
  }
  Serial.println(ReadData());
  
  if(data==1){
    setMode("Off");
    delay(200);
  }
  else if(data==2){
    setMode("LightR");
    delay(200);
  }
  else if(data==3){
    setMode("LightB");
    delay(200);
  }
  else if(data==4){
    setMode("Color");
    delay(200);
  }
}

int ReadData(){
  Wire.requestFrom(0x10,2);
  int data = (Wire.read() | Wire.read() << 8);
  return data;
}
void setMode(String mode){
  Wire.beginTransmission(0x10);
  //Sensor off
  if(mode == "Off"){
    Wire.write(5);
  }
  //Light Blue
  else if(mode == "LightB"){
    Wire.write(1);
  }
  //Light red
  else if(mode == "LightR"){
    Wire.write(0);
  }
  //Color
  else{
    Wire.write(2);
  }
  Wire.endTransmission();
}
