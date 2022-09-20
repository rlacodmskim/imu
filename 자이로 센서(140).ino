#include <Wire.h>

void setup() {
  Serial.begin(115200);  

  Wire.begin();
  Wire.setClock(400000);
  
  Wire.beginTransmission(0x68);
  Wire.write(0x6b);
  Wire.write(0x0);
  Wire.endTransmission(true);
}

void loop() {
  Wire.beginTransmission(0x68);
  Wire.write(0x45);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68,2,true);
  int16_t GyYH = Wire.read();  
  int16_t GyYL = Wire.read();
  int16_t GyY = GyYH <<8 |GyYL;

  static int cnt_loop;
  cnt_loop ++;
  if(cnt_loop%100 !=0) return;

  Serial.print("GyY = "); Serial.print(GyY);
  Serial.println();
}