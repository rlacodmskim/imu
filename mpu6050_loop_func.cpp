#include <Wire.h>
#include <Arduino.h>

#define LSB_Sensitivity_R4_2 16384.0
#define MPU 0x68
#define cali_sampling_rate 200
#define sample 4000

long sampling_timer;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float elapsedTime, currentTime, previousTime;

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

typedef struct s_angleVal{
  float roll;
  float pitch;
  float yaw;
} t_angleVal;

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

t_errVal calculate_IMU_error(){
  t_errVal ret3;
  ret3.AccErrorX = 0;
  ret3.AccErrorY = 0;
  ret3.GyroErrorX = 0;
  ret3.GyroErrorY = 0;
  ret3.GyroErrorZ = 0;

  int c = 0;

  while (c < cali_sampling_rate) {
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
  while (c < cali_sampling_rate) {
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
  ret3.GyroErrorX = ret3.GyroErrorX / 200.0;
  ret3.GyroErrorY = ret3.GyroErrorY / 200.0;
  ret3.GyroErrorZ = ret3.GyroErrorZ / 200.0;
  
  return ret3;
}

void printIMUError(t_errVal errVal) {
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
  Serial.println("Calibration Processing...");
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
  t_errVal errVal = calculate_IMU_error();
  printIMUError(errVal);
  delay(20);
  Serial.println("Calibration Successed");
}

// get acceleroneer raw and error data
void talkToAccData(){
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
}

void talkToGyroData(){
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
}

// calculate angle
t_angleVal calAngleValue(t_accVal accVal, t_gyroVal gyroVal, t_errVal errVal){
  t_angleVal ret4;
  
  talkToAccData();
  // Calculating Roll and Pitch from the accelerometer data
  accAngleX = (atan(accVal.accY / sqrt(pow(accVal.accX, 2) + pow(accVal.accZ, 2))) * 180 / PI) - errVal.AccErrorX; // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
  accAngleY = (atan(-1 * accVal.accX / sqrt(pow(accVal.accY, 2) + pow(accVal.accZ, 2))) * 180 / PI) - errVal.AccErrorY; // AccErrorY ~(-1.58)
  
  // === read angle with filter  ===
  // === Read gyroscope data === //
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000;  // Divide by 1000 to get seconds

  talkToGyroData();
  // Correct the outputs with the calculated error values
  gyroVal.gyroX = gyroVal.gyroX - errVal.GyroErrorX; // GyroErrorX ~(-0.56)
  gyroVal.gyroY = gyroVal.gyroY - errVal.GyroErrorY; // GyroErrorY ~(2)
  gyroVal.gyroZ = gyroVal.gyroZ - errVal.GyroErrorZ; // GyroErrorZ ~ (-0.8)

  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX = gyroAngleX + gyroVal.gyroX * elapsedTime; // deg/s * s = deg
  gyroAngleY = gyroAngleY + gyroVal.gyroY * elapsedTime; // gyroAngleX = GyroX * elapsedTime; then the movement than the stabilization

  // Complementary filter - combine acceleromter and gyro angle values
  gyroAngleX = 0.996 * gyroAngleX + 0.004 * accAngleX;
  gyroAngleY = 0.996 * gyroAngleY + 0.004 * accAngleY;

  ret4.roll = gyroAngleX;
  ret4.pitch = gyroAngleY;
  ret4.yaw = ret4.yaw + gyroVal.gyroZ * elapsedTime;
  return ret4;
}

// Print the values on the serial monitor
void printAngleVal(t_angleVal angleVal){
  Serial.print(angleVal.roll);
  Serial.print("/");
  Serial.print(angleVal.pitch);
  Serial.print("/");
  Serial.println(angleVal.yaw);
}

// sampling clock
void samplingRate(){
  while(micros() - sampling_timer < sample); //
  sampling_timer = micros(); //Reset the sampling timer  
}

// main function imu

// imu initializing
void setup(){
  Wire.begin();  // Initialize comunication
  Serial.begin(9600);
  initializeIMU();  // wire setup
  delay(100);
}

void loop(){
  // get values
  t_accVal accVal = getAccRawContinue();  // accVal에 함수 거친 것이 저장됨
  t_gyroVal gyroVal = getGyroRawContinue();
  t_errVal errVal = calculate_IMU_error();  // TODO: loop에서 여기까지 계속 가져오니까 문제가 생기는 듯
  Serial.println("Error Value");
  Serial.println(errVal.GyroErrorX);
  
  // calculate roll, pitch, yaw
  t_angleVal angleVal = calAngleValue(accVal, gyroVal, errVal);
  printAngleVal(angleVal);
  samplingRate();
}

//TODO: low speed and inaccurate value
//TODO: error value가 갱신됨
