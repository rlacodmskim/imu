#include <Wire.h>

void setup() {
  Wire.begin();
  Wire.setClock(400000);
  
  Serial.begin(115200);  

  Wire.beginTransmission(0x68);
  Wire.write(0x6b);
  Wire.write(0x0);
  Wire.endTransmission(true);

  int32_t GyXSum=0, GyYSum=0, GyZSum=0;
  int32_t GyXOff, GyYOff, GyZOff;
  
  for(int cnt_sample=0; cnt_sample<1000; cnt_sample++){
  // 값을 받아옴
  Wire.beginTransmission(0x68);
  Wire.write(0x45);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68,6,true);
  int16_t GyXH = Wire.read();  
  int16_t GyXL = Wire.read();
  int16_t GyYH = Wire.read();  
  int16_t GyYL = Wire.read();
  int16_t GyZH = Wire.read();  
  int16_t GyZL = Wire.read();
  int16_t GyX = GyXH <<8 |GyXL;
  int16_t GyY = GyYH <<8 |GyYL; 
  int16_t GyZ = GyZH <<8 |GyZL;

  // sum
  GyXSum += GyX, GyYSum += GyY, GyZSum += GyZ;
  }

  GyXOff = GyXSum /1000.0;
  GyYOff = GyYSum /1000.0;
  GyZOff = GyZSum /1000.0;
}

void loop() {
  // 값을 다시 받아옴
  Wire.beginTransmission(0x68);
  Wire.write(0x45);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68,6,true);
  int16_t GyXH = Wire.read();  
  int16_t GyXL = Wire.read();
  int16_t GyYH = Wire.read();  
  int16_t GyYL = Wire.read();
  int16_t GyZH = Wire.read();  
  int16_t GyZL = Wire.read();
  int16_t GyX = GyXH <<8 |GyXL;
  int16_t GyY = GyYH <<8 |GyYL; 
  int16_t GyZ = GyZH <<8 |GyZL;
	
  //뒤틀린 축에 대한 보정값
  double GyXD = GyX - GyXOff;
  double GyYD = GyY - GyYOff;
  double GyZD = GyZ - GyZOff;

  //100번 마다 세기
  static int cnt_loop;
  cnt_loop ++;
  if(cnt_loop%100 !=0) return;

 /*  Serial.print("GyX = "); Serial.print(GyX), 
     Serial.print("GyY = "); Serial.print(GyY), 
     Serial.print("GyZ = "); Serial.print(GyZ);   */
  Serial.print("GyXD = "); Serial.print(GyXD);
  Serial.print("GyYD = "); Serial.print(GyYD);
  Serial.print("GyZD = "); Serial.print(GyZD);
  Serial.println();
}
