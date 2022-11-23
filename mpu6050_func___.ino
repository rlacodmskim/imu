#include <Wire.h>

#define LSB_Sensitivity_R4_2 16384.0
#define MPU 0x68

float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float elapsedTime, currentTime, previousTime;
int c = 0;

// get raw data
typedef struct s_accVal{
  float accX;
  float accY;
  float accZ;
} t_accVal;

typedef struct s_gyroVal{
  float gyroX;
  float gyroY;
  float gyroZ;
} t_gyroVal;

typedef struct s_errVal{
  float AccErrorX;
  float AccErrorY;
  float GyroErrorX;
  float GyroErrorY;
  float GyroErrorZ;
} t_errVal;

// getting raw data function
// get accelerator raw data
t_accVal getAccRawContinue(){
  t_accVal ret1;  
  ret1.accX = (Wire.read() << 8 | Wire.read()) / LSB_Sensitivity_R4_2;
  ret1.accY = (Wire.read() << 8 | Wire.read()) / LSB_Sensitivity_R4_2;
  ret1.accZ = (Wire.read() << 8 | Wire.read()) / LSB_Sensitivity_R4_2;

  return ret1;
} // ret에 채워 넣은 걸 반환하겠다

// get raw gyroscope data
t_gyroVal getGyroRawContinue(){
  t_gyroVal ret2;
  ret2.gyroX = (Wire.read() << 8 | Wire.read()) / 131.0;
  ret2.gyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  ret2.gyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
  
  return ret2;
} // t_gyroVal는 반환 형태임

//caculate IMU error

t_errVal getErrVal(){
  t_errVal ret3;
    while (c < 200) {
      Wire.beginTransmission(MPU);
      Wire.write(0x3B);
      Wire.endTransmission(false);
      Wire.requestFrom(MPU, 6, true);
      t_accVal accVal = getAccRawContinue();

      // Sum all readings
      ret3.AccErrorX = ret3.AccErrorX + ((atan((accVal.accY) / sqrt(pow((accVal.accX), 2) + pow((accVal.accZ), 2))) * 180 / PI));
      ret3.AccErrorY = ret3.AccErrorY + ((atan(-1 * (accVal.accX) / sqrt(pow((accVal.accY), 2) + pow((accVal.accZ), 2))) * 180 / PI));
      c++;
    }

    //Divide the sum by 200 to get the error value
    ret3.AccErrorX = ret3.AccErrorX / 200;
    ret3.AccErrorY = ret3.AccErrorY / 200;
    c = 0;

    // Read gyro values 200 times
    while (c < 200) {
      Wire.beginTransmission(MPU);
      Wire.write(0x43);
      Wire.endTransmission(false);
      Wire.requestFrom(MPU, 6, true);
    
      t_gyroVal gyroVal = getGyroRawContinue();

      // Sum all readings
      ret3.GyroErrorX = ret3.GyroErrorX + gyroVal.gyroX;
      ret3.GyroErrorY = ret3.GyroErrorY + gyroVal.gyroY;
      ret3.GyroErrorZ = ret3.GyroErrorZ + gyroVal.gyroZ;
      c++;
    }

    //Divide the sum by 200 to get the error value
    ret3.GyroErrorX = ret3.GyroErrorX / 200;
    ret3.GyroErrorY = ret3.GyroErrorY / 200;
    ret3.GyroErrorZ = ret3.GyroErrorZ / 200;
  
  return ret3;
}

void calculate_IMU_error() {
  // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. 
  // From here we will get the error values used in the above equations printed on the Serial Monitor.
  // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
  // Read accelerometer values 200 times
  t_errVal errVal = getErrVal();

  // Print the error values on the Serial Monitor
  Serial.print("AccErrorX: ");
  Serial.println(errVal.AccErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(errVal.AccErrorY);
  Serial.print("GyroErrorX: ");
  Serial.println(errVal.GyroErrorX);
  Serial.print("GyroErrorY: ");
  Serial.println(errVal.GyroErrorY);
  Serial.print("GyroErrorZ: ");
  Serial.println(errVal.GyroErrorZ);
  }

// initializing IMU wire
void initializeIMU(){
  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission
  
  // Configure Accelerometer Sensitivity - Full Scale Range (default +/- 2g)
  Wire.beginTransmission(MPU);
  Wire.write(0x1C);                  //Talk to the ACCEL_CONFIG register (1C hex)
  Wire.write(0x10);                  //Set the register bits as 00010000 (+/- 8g full scale range)
  Wire.endTransmission(true);
  // Configure Gyro Sensitivity - Full Scale Range (default +/- 250deg/s)
  Wire.beginTransmission(MPU);
  Wire.write(0x1B);                   // Talk to the GYRO_CONFIG register (1B hex)
  Wire.write(0x10);                   // Set the register bits as 00010000 (1000deg/s full scale)
  Wire.endTransmission(true);
  delay(20);
  
  // Call this function if you need to get the IMU error values for your module
  calculate_IMU_error();
  delay(20);
}

// main function imu

// imu initializing
void setup(){
  Serial.begin(19200);
  Serial.print("Calibration Processing...")
  ;initializeIMU();  // wire setup
  Serial.print("Calibration Successed")
  Serial.println("initializing Successed");
  delay(10000)
}

void loop(){
  // === Read acceleromter data === //
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers

  t_accVal accVal = getAccRawContinue();  // accVal에 함수 거친 것이 저장됨
  
  // Calculating Roll and Pitch from the accelerometer data
  accAngleX = fabs((atan(accVal.accY / sqrt(pow(accVal.accX, 2) + pow(accVal.accZ, 2))) * 180 / PI) + errVal.AccErrorX); // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
  accAngleY = fabs((atan(-1 * accVal.accX / sqrt(pow(accVal.accY, 2) + pow(accVal.accZ, 2))) * 180 / PI) + errVal.AccErrorY); // AccErrorY ~(-1.58)

  // === Read gyroscope data === //
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000;  // Divide by 1000 to get seconds

  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers

  t_gyroVal gyroVal = getGyroRawContinue();

  // Correct the outputs with the calculated error values
  gyroVal.gyroX = gyroVal.gyroX + errVal.GyroErrorX; // GyroErrorX ~(-0.56)
  gyroVal.gyroY = gyroVal.gyroY + errVal.GyroErrorY; // GyroErrorY ~(2)
  gyroVal.gyroZ = gyroVal.gyroZ + errVal.GyroErrorZ; // GyroErrorZ ~ (-0.8)

  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX = gyroAngleX + gyroVal.gyroX * elapsedTime; // deg/s * s = deg
  gyroAngleY = gyroAngleY + gyroVal.gyroY * elapsedTime; // gyroAngleX = GyroX * elapsedTime; then the movement than the stabilisation
  yaw = yaw + gyroVal.gyroZ * elapsedTime;

  // Complementary filter - combine acceleromter and gyro angle values
  gyroAngleX = 0.96 * gyroAngleX + 0.04 * accAngleX;
  gyroAngleY = 0.96 * gyroAngleY + 0.04 * accAngleY;

  roll = gyroAngleX;
  pitch = gyroAngleY;
  
  // Print the values on the serial monitor
  Serial.print(roll);
  Serial.print("/");
  Serial.print(pitch);
  Serial.print("/");
  Serial.println(yaw);
}
