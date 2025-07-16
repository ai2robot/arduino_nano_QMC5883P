 /*
 * GY-273 Compass Module  ->  Arduino nano
 * VCC  -> 3.3V
 * GND  -> GND
 * SCL  -> A5
 * SDA  -> A4
 * DRDY -> Not Connected
 */
#include <Wire.h>

// QMC5883P I2C address
const int QMC5883P_ADDR = 0x2C;

// Control register addresses
const int MODE_REG = 0x0A;
const int CONFIG_REG = 0x0B;

// Data output register addresses
const int X_LSB_REG = 0x01;
const int X_MSB_REG = 0x02;
const int Y_LSB_REG = 0x03;
const int Y_MSB_REG = 0x04;
const int Z_LSB_REG = 0x05;
const int Z_MSB_REG = 0x06;

// Status register address
const int STATUS_REG = 0x09;

// Initialize QMC5883P
void initQMC5883P() {
  Wire.begin();
  // Set to continuous measurement mode, 200Hz output data rate, ±8G range
  Wire.beginTransmission(QMC5883P_ADDR);
  Wire.write(MODE_REG);
  Wire.write(0xCF); // 0xCF: Continuous mode, 200Hz output data rate
  Wire.endTransmission();

  Wire.beginTransmission(QMC5883P_ADDR);
  Wire.write(CONFIG_REG);
  Wire.write(0x08); // 0x08: Enable Set/Reset mode, ±8G range
  Wire.endTransmission();
}

// Read raw data from QMC5883P
void readQMC5883PData(int16_t& x, int16_t& y, int16_t& z) {
  Wire.beginTransmission(QMC5883P_ADDR);
  Wire.write(X_LSB_REG);
  Wire.endTransmission(false);
  Wire.requestFrom(QMC5883P_ADDR, 6);

  if (Wire.available() == 6) {
    byte x_lsb = Wire.read();
    byte x_msb = Wire.read();
    byte y_lsb = Wire.read();
    byte y_msb = Wire.read();
    byte z_lsb = Wire.read();
    byte z_msb = Wire.read();

    // Combine MSB and LSB to form 16-bit values
    x = (x_msb << 8) | x_lsb;
    y = (y_msb << 8) | y_lsb;
    z = (z_msb << 8) | z_lsb;
  }
}

void setup() {
  Serial.begin(9600);
  initQMC5883P();
}

void loop() {
  int16_t x, y, z;
  readQMC5883PData(x, y, z);

    // Calculate heading in radians (using X and Y axes)
  // atan2(y,x) returns angle between positive x-axis and point (x,y)
  float heading = atan2(y, x);
  
  // Convert radians to degrees
  heading = heading * 180 / PI;
  
  // Adjust heading to 0-360° range
  if (heading < 0) {
    heading += 360;
  }

  Serial.print("X: ");
  Serial.print(x);
  Serial.print("\tY: ");
  Serial.print(y);
  Serial.print("\tZ: ");
  Serial.print(z);
  Serial.print(" Heading: ");
  Serial.print(heading);
  Serial.println("°");

  delay(100);
}