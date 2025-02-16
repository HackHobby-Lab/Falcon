#include <QMC5883LCompass.h>
#include "AA_MCP2515.h"

// Magnetometer Sensor
QMC5883LCompass compass;
int calibrationData[3][2];
bool changed = false, done = false, calibrated = false;
int t = 0, c = 0;

// CAN Bus Configuration
const CANBitrate::Config CAN_BITRATE = CANBitrate::Config_8MHz_500kbps;
const uint8_t CAN_PIN_CS = 5;
const int8_t CAN_PIN_INT = 25;
CANConfig config(CAN_BITRATE, CAN_PIN_CS, CAN_PIN_INT);
CANController CAN(config);

void setup() {
  Serial.begin(115200);
  compass.init();

  // Initialize CAN Bus
  while (CAN.begin(CANController::Mode::Normal) != CANController::OK) {
    Serial.println("CAN begin FAIL - delaying for 1 second");
    delay(1000);
  }
  Serial.println("CAN begin OK");
}

void loop() {
  if (!calibrated) {
    calibrateMagnetometer();
  } else {
    sendMagnetometerData();
  }
}

void calibrateMagnetometer() {
  int x, y, z;
  compass.read();
  x = compass.getX();
  y = compass.getY();
  z = compass.getZ();

  changed = false;
  if (x < calibrationData[0][0]) { calibrationData[0][0] = x; changed = true; }
  if (x > calibrationData[0][1]) { calibrationData[0][1] = x; changed = true; }
  if (y < calibrationData[1][0]) { calibrationData[1][0] = y; changed = true; }
  if (y > calibrationData[1][1]) { calibrationData[1][1] = y; changed = true; }
  if (z < calibrationData[2][0]) { calibrationData[2][0] = z; changed = true; }
  if (z > calibrationData[2][1]) { calibrationData[2][1] = z; changed = true; }

  if (changed && !done) {
    Serial.println("CALIBRATING... Keep moving your sensor around.");
    c = millis();
  }
  t = millis();

  if ((t - c > 5000) && !done) {
    done = true;
    Serial.println("Calibration Complete!");
    compass.setCalibration(calibrationData[0][0], calibrationData[0][1], calibrationData[1][0], calibrationData[1][1], calibrationData[2][0], calibrationData[2][1]);
    calibrated = true;
  }
}

void sendMagnetometerData() {
  int x, y, z;
  compass.read();
  x = compass.getX();
  y = compass.getY();
  z = compass.getZ();

  int azimuth = compass.getAzimuth();
  float bearing = compass.getBearing(azimuth);

  Serial.print("Azimuth: "); Serial.print(azimuth);
  Serial.print(" Bearing: "); Serial.print(bearing);
  Serial.print(" X: "); Serial.print(x);
  Serial.print(" Y: "); Serial.print(y);
  Serial.print(" Z: "); Serial.println(z);

  // Send Azimuth & Bearing (as two 2-byte values)
  uint8_t data1[4];
  uint16_t az = (uint16_t) azimuth;
  uint16_t br = (uint16_t) bearing;
  memcpy(&data1[0], &az, 2);
  memcpy(&data1[2], &br, 2);
  CANFrame frame1(0x200, data1, sizeof(data1));
  CAN.write(frame1);
  frame1.print("CAN TX: Azimuth/Bearing");

  // Send X, Y, Z values (as three 2-byte values)
  uint8_t data2[6];
  memcpy(&data2[0], &x, 2);
  memcpy(&data2[2], &y, 2);
  memcpy(&data2[4], &z, 2);
  CANFrame frame2(0x201, data2, sizeof(data2));
  CAN.write(frame2);
  frame2.print("CAN TX: X/Y/Z");

  delay(500);
}
