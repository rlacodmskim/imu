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
  int16_t GyY = GyYH <<8 |GyYL; //raw 값

  static int32_t GyYSum =0;
  static double GyYOff =0.0;
  static int cnt_sample =1000;
  if(cnt_sample >0) {
	GyYSum += GyY;
	cnt_sample --;
	if(cnt_sample ==0) {      
	  GyYOff = GyYSum /1000.0;  //GyY를 1000번 더한 값인 GyYSum을 이용해 GyY의 평균값인 GyYOff 구하기
	}
	delay(1); //1mm delay 후 return
	return;    
  }
  double GyYD = GyY - GyYOff; //보정된 값

  static int cnt_loop;
  cnt_loop ++;
  if(cnt_loop%100 !=0) return;  //100번 마다 세기

 //  Serial.print("GyY = "); Serial.print(GyY);
  Serial.print("GyYD = "); Serial.print(GyYD);
  Serial.println();
}
