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
  Wire.requestFrom(0x68,6,true);
  int16_t GyXH = Wire.read();  
  int16_t GyXL = Wire.read();
  int16_t GyYH = Wire.read();  
  int16_t GyYL = Wire.read();
  int16_t GyZH = Wire.read();  
  int16_t GyZL = Wire.read();
  int16_t GyX = GyXH <<8 |GyXL;
  int16_t GyY = GyYH <<8 |GyYL; 
  int16_t GyZ = GyZH <<8 |GyZL; //raw 값

  static int32_t GyXSum =0, GyYSum =0, GyZSum =0;
  static double GyXOff =0.0, GyYOff =0.0, GyZOff =0.0;
  static int cnt_sample =1000;
  if(cnt_sample >0) {
	GyXSum += GyX, GyYSum += GyY, GyZSum += GyZ;
	cnt_sample --;
	if(cnt_sample ==0) {      
	  GyXOff = GyXSum /1000.0;
    GyYOff = GyYSum /1000.0;
    GyZOff = GyZSum /1000.0;  //GyY를 1000번 더한 값인 GyYSum을 이용해 GyY의 평균값인 GyYOff 구하기
	}
	delay(1); //1mm delay 후 return
	return;    
  }
  double GyXD = GyX - GyXOff;
  double GyYD = GyY - GyYOff;
  double GyZD = GyZ - GyZOff; //보정된 값

  static int cnt_loop;
  cnt_loop ++;
  if(cnt_loop%100 !=0) return;  //100번 마다 세기

 /*  Serial.print("GyX = "); Serial.print(GyX), 
     Serial.print("GyY = "); Serial.print(GyY), 
     Serial.print("GyZ = "); Serial.print(GyZ); */
  Serial.print("GyXD = "); Serial.print(GyXD);
  Serial.print("GyYD = "); Serial.print(GyYD);
  Serial.print("GyZD = "); Serial.print(GyZD);
  Serial.println();
}
