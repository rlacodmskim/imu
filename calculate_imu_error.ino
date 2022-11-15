#include <Wire.h>

#define LSB_Sensitivity_R4_2 16384.0

// get raw data
typedef struct s_accVal{
  float accX;
  float accY;
  float accZ;
} t_accVal;

typedef struct s_gyroVal{a
  float gyroX;
  float gyroY;
  float gyroZ;
} t_gyroVal;

void getAccRaw(t_accVal){
  
  accX = (Wire.read() << 8 | Wire.read()) / LSB_Sensitivity_R4_2;
  accY = (Wire.read() << 8 | Wire.read()) / LSB_Sensitivity_R4_2;
  accZ = (Wire.read() << 8 | Wire.read()) / LSB_Sensitivity_R4_2;
}

void getGyroRaw(t_gyroVal){
  gyroX = Wire.read() << 8 | Wire.read();
  gyroY = Wire.read() << 8 | Wire.read();
  gyroZ = Wire.read() << 8 | Wire.read();
}

//calculate error values
typedef struct s_accErrVal{
  float accErrX;
  float accErrY;
} t_accErrVal;

typedef struct s_gyroErrVal{
  float gyroErrX;
  float gyroErrY;
  float gyroErrZ;
} t_gyroErrVal;

typedef struct s_configVal{
  struct s_accErrVal;
  struct s_gyroErrVal;
} t_configVal;

void setup() {
  Serial.begin(19200);
  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(0x68);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission
  
  // Configure Accelerometer Sensitivity - Full Scale Range (default +/- 2g)
  Wire.beginTransmission(0x68);
  Wire.write(0x1C);                  //Talk to the ACCEL_CONFIG register (1C hex)
  Wire.write(0x10);                  //Set the register bits as 00010000 (+/- 8g full scale range)
  Wire.endTransmission(true);
  // Configure Gyro Sensitivity - Full Scale Range (default +/- 250deg/s)
  Wire.beginTransmission(0x68);
  Wire.write(0x1B);                   // Talk to the GYRO_CONFIG register (1B hex)
  Wire.write(0x10);                   // Set the register bits as 00010000 (1000deg/s full scale)
  Wire.endTransmission(true);
  delay(20);
  
  // Call this function if you need to get the IMU error values for your module
  calculate_IMU_error();
  delay(20);
}


// main function
void calculateIMUError(){ //return할 값이 없을 때 void로

  s_accErrVal temp_acc = calcAccErrVal();
  s_gyroErrVal temp_gyro = calcGyroErrVal();
  DBG_printErrVal(temp_acc, temp_gyro);
}

// sub function
s_accErrVal.calcAccErrVal(t_accErrVal, t_accVal){
  int idx = 0;

  while(idx < 200){
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(0x68, 6, true);
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

s_gyroErrVal.calcGyroErrVal(t_gyroErrVal, t_gyroVal){

  while(idx < 200){
  Wire.beginTransmission(0x68);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(0x68, 6, true);
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

  s_gyroErrVal ret;

  ret.gyroErrX = gyroErrX;
  ret.gyroErrY = gyroErrY;
  ret.gyroErrZ = gyroErrZ;

  return ret;
}

void DBG_printErrVal(t_configVal){
  Serial.print("AccErrorX: ");
  Serial.println(t_accErrVal.accErrX);
  Serial.print("AccErrorY: ");
  Serial.println(t_accErrVal.accErrY);
  Serial.print("GyroErrorX: ");
  Serial.println(t_gyroVal.gyroErrX);
  Serial.print("GyroErrorY: ");
  Serial.println(t_gyroVal.gyroErrY);
  Serial.print("GyroErrorZ: ");
  Serial.println(t_gyroVal.gyroErrZ);
}

void loop(){
}