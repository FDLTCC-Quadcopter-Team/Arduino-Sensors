#include <Wire.h>

#define ACC_DEV 0x53
#define GYRO_DEV 0x68
#define COMPASS_DEV 0x1E
#define LIDAR_DEV 0x62
#define THERM_DEV 0x5A

void sendByte(byte dev, byte b){
  Wire.beginTransmission(dev);
  Wire.write(b);
  Wire.endTransmission();
}

void sendByte(byte dev, byte b, boolean n){
  Wire.beginTransmission(dev);
  Wire.write(b);
  Wire.endTransmission(n);
}
void setByte(byte dev, byte reg, byte b){
  Wire.beginTransmission(dev);
  Wire.write(reg);
  Wire.write(b);
  Wire.endTransmission();
}

void readBuff(byte dev, byte reg, int length, byte* output){
  sendByte(dev, reg);
  Wire.requestFrom((int)dev, length);
  int i=0;
  while(Wire.available()){
    output[i] = Wire.read();
    i++;
  }
}

void initAcc(){
  setByte(ACC_DEV, 0x2D, 0x09);
  delay(20);
  setByte(ACC_DEV, 0x31, 0x08);
}

void initGyro(){
  setByte(GYRO_DEV,0x3E,0x00);
  delay(20);
  setByte(GYRO_DEV,0x02,0xFF);
  delay(20);
  setByte(GYRO_DEV,0x16,0x1E);
  delay(20);
  setByte(GYRO_DEV,0x17,0x00);
}

void initCompass(){
  setByte(COMPASS_DEV,0x02,0x00);
}

void readAcc(short* output){
  byte data[6];
  readBuff(ACC_DEV, 0x32, 6, data);
  for(int i=0;i<3;i++){
    output[i]=(data[(i*2)+1]<<8)|data[i*2];
  }
}

void readGyro(short* output){
  byte data[6];
  readBuff(GYRO_DEV, 0x1D, 6, data);
  for(int i=0;i<3;i++){
    output[i]=(data[i*2]<<8)|data[(i*2)+1];
  }
}

void readCompass(short* output){
  byte data[6];
  readBuff(COMPASS_DEV, 0x03, 6, data);
  for(int i=0;i<3;i++){
    output[i]=(data[i*2]<<8)|data[(i*2)+1];
  }
}

double readTherm(){
  sendByte(THERM_DEV, 0x07, false);
  Wire.requestFrom(THERM_DEV, 3, true);
  if(Wire.available()>=3){
    byte low = Wire.read();
    byte high = Wire.read();
    Wire.read();//PEC
    return (((double)(low|(high<<8))*0.02)- 273.15);
  }
}

short readLidar(){
  setByte(LIDAR_DEV,0x00,0x04);
  delay(20);
  byte data[2];
  readBuff(LIDAR_DEV, 0x8F, 2, data);
  return (data[1]|(data[0]<<8));
}

void setup()
{
  Wire.begin();        // join i2c bus (address optional for master)
  initAcc();
  delay(20);
  initGyro();
  delay(20);
  initCompass();
  delay(20);
  Serial.begin(9600);  // start serial for output
  Serial.println("Acc X,Acc Y,Acc Z,Gyro X,Gyro Y,Gyro Z,Compass X,Compass Y,Compass Z,Lidar,Temperature,Time");
}

void loop()
{
  short acc[3];
  short gyro[3];
  short compass[3];
  readAcc(acc);
  delay(20);
  readGyro(gyro);
  delay(20);
  readCompass(compass);
  delay(20);
  short lidar = readLidar();
  double temp = readTherm();
  for(int i=0;i<3;i++){
    Serial.print(acc[i]);
    Serial.print(", ");
  }
  for(int i=0;i<3;i++){
    Serial.print(gyro[i]);
    Serial.print(", ");
  }
  for(int i=0;i<3;i++){
    Serial.print(compass[i]);
    Serial.print(", ");
  }
  Serial.print(lidar);
  Serial.print(", ");
  Serial.print(temp);
  Serial.print(", ");
  Serial.println(millis());
}
