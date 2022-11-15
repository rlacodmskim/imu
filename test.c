#define LSB_Sensitivity_R4_2 16384

// get raw data
struct s_accVal{
	float accX;
	float accY;
	float accZ;
}

struct s_gyroVal{
	float gyroX;
	float gyroY;
	float gyroZ;
}

void getAccRaw(s_accVal){
  
  accX = (Wire.read() << 8 | Wire.read()) / LSB_Sensitivity_R4_2;
  accY = (Wire.read() << 8 | Wire.read()) / LSB_Sensitivity_R4_2;
  accZ = (Wire.read() << 8 | Wire.read()) / LSB_Sensitivity_R4_2;
}

void getGyroRaw(s_gyroVal){
  gyroX = Wire.read() << 8 | Wire.read();
  gyroY = Wire.read() << 8 | Wire.read();
  gyroZ = Wire.read() << 8 | Wire.read();
}

//calculate error values
struct s_accErrVal{
	float accErrX;
	float accErrY;
}

struct s_gyroErrVal{
	float gyroErrX;
	float gyroErrY;
	float gyroErrZ;
}

struct s_configVal{
	struct s_accErrVal;
	struct s_gyroErrVal;
}
//s_calibVal.s_accErrVal.accErX;

void calculateIMUError(){	//return할 값이 없을 때 void로

	// 1
	s_accErrVal temp_acc = calcAccErrVal();	//temp_acc의 변수를 가지는데 calc을 넣겠다
	s_gyroErrVal temp_gyro = calcGyroErrVal();
	DBG_printErrVal(temp_acc, temp_gyro);

	// 2
	// DBG_printErrVal(calAccErrVal, calcGyroVal);
}

void DBG_printErrVal(s_accErrVal _accErrVal, s_gyro_ErrVal){

	Serial.print("AccError X: ");
	Serial.print(_accErrVal.accErrX);
}

s_accErrVal calcAccErrVal(){
	int	idx = 0;

	while(idx < 200){
		Wire.beginTransmission(0x68);
		Wire.write(0x3B);
		Wire.endTransmission(false);
		Wire.requestFrom(MPU, 6, true);
		getAccRaw();

		accErrorX = accErrorX + ((atan((accY) / sqrt(pow((accX), 2) + pow((accZ), 2))) * 180 / PI));
    	accErrorY = accErrorY + ((atan(-1 * (accX) / sqrt(pow((accY), 2) + pow((accZ), 2))) * 180 / PI));
        idx++;
	}

	accErrX = accErrX / 200;
	accErrY = accErrY / 200;
	idx = 0;

	s_accErrVal ret;

	ret.accErrX = accErrX;
	ret.accErrY = accErrY;

	return ret;
}

s_gyroErrVal calcGyroErrVal(){
  ret
  while(idx < 200){
	Wire.beginTransmission(0x68);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    getGyroRaw();
    // Sum all readings
    gyroErrX = gyroErrX + (GyroX / 131.0);
    gyroErrY = gyroErrY + (GyroY / 131.0);
    gyroErrZ = gyroErrZ + (GyroZ / 131.0);
    idx++;
  }

  //Divide the sum by 200 to get the error value
  gyroErrX = gyroErrX / 200;
  gyroErrY = gyroErrY / 200;
  gyroErrZ = gyroErrZ / 200;

void DBG_printErrVal(s_accErrVal, s_gyro_ErrVal){
  Serial.print("AccErrorX: ");
  Serial.println(s_accErrVal.accErrX);
  Serial.print("AccErrorY: ");
  Serial.println(s_accErrVal.accErrY);
  Serial.print("GyroErrorX: ");
  Serial.println(s_gyroErrVal.gyroErrX);
  Serial.print("GyroErrorY: ");
  Serial.println(s_gyroErrVal.gyroErrY);
  Serial.print("GyroErrorZ: ");
  Serial.println(s_gyroErrVal.gyroErrZ);
}
