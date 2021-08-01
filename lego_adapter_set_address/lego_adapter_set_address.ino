#include <Wire.h>
#include <i2cdetect.h>
void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.setTimeout(10);
  Serial.println("To change address of module connect jumper Addr and press switch SW on module.");
  Serial.println("Wait for led Status to start blinking once a second.");
  Serial.println("Write new adress from 0x04 to 0x0F. 0X00 - 0X03 are reserved!!");
}
String serialInput;
int address = 0;
void loop() {
  while (address == 0){
    if (Serial.available() > 0) {
      serialInput = Serial.readString();
      serialInput = serialInput.substring(2);
      address = hstoi(serialInput);
    }
  }
  Wire.beginTransmission(0x03);
  Wire.write(address);
  Wire.write((address>>8));
  Wire.endTransmission();
  Serial.print("Address sucessfuly set to 0X");
  Serial.print(address,HEX);
  Serial.println(".");
  Serial.println("You can now press disconnect jumper Addr and press switch SW.");
  while (true) {}
}
int hstoi(String recv){
  char c[recv.length() + 1];
  recv.toCharArray(c, recv.length() + 1);
  return strtol(c, NULL, 16);
}
