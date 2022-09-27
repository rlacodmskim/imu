#include <Wire.h>

void setup() {
  Serial.begin(115200); // serial 통신 속도를 115200으로 설정한다.

  Wire.begin();
  Wire.setClock(400000);  // I2C 통신 속도를 400KHZ으로 설정한다.
  
  Wire.beginTransmission(0x68); // 0*68번지 값을 갖는 MPU6050과 I2C통신을 시작한다. 이때 0*68번지는 I2C 슬레이브 모듈이 된다.
  Wire.write(0x6b);
  Wire.write(0x0);
  Wire.endTransmission(true); //**  
}
// 9-12: mpu에 0*6b, 0을 보냄. 0*6b를 0으로 설정 후 mpu 깨움

void loop() {
  Wire.beginTransmission(0x68);
  Wire.write(0x45);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68,2,true);
  int8_t GyXH = Wire.read();  
  int8_t GyXL = Wire.read();
  int8_t GyX = GyXH <<8 |GyXL;
  
  int8_t GyYH = Wire.read();  
  int8_t GyYL = Wire.read();
  int8_t GyY = GyYH <<8 |GyYL;  //8비트 만큼 shift 해서 GyYH,GyYL 순으로 저장

  int8_t GyZH = Wire.read();  
  int8_t GyZL = Wire.read();
  int8_t GyZ = GyZH <<8 |GyZL;

  static int cnt_loop;
  cnt_loop ++;
  if(cnt_loop%100 !=0) return;  //loop 함수를 100번 수행할 때마다 한 번 출력한다

  Serial.print("GyX = "); Serial.print(GyX);
  Serial.println(); 
  Serial.print("GyY = "); Serial.print(GyY);
  Serial.println(); 
  Serial.print("GyZ = "); Serial.print(GyZ);
  Serial.println();
}