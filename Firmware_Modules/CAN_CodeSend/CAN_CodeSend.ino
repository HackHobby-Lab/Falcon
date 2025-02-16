#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include "AA_MCP2515.h"

// Define GPS module connections
#define RXPin 16  // GPS TX to ESP RX
#define TXPin 17  // GPS RX to ESP TX
#define GPSBaud 9600  

// Use Serial2 for GPS
HardwareSerial mySerial(2);
TinyGPSPlus gps;

// CAN Bus Configuration
const CANBitrate::Config CAN_BITRATE = CANBitrate::Config_8MHz_500kbps;
const uint8_t CAN_PIN_CS = 5;
const int8_t CAN_PIN_INT = 25;
CANConfig config(CAN_BITRATE, CAN_PIN_CS, CAN_PIN_INT);
CANController CAN(config);

void setup() {
  Serial.begin(115200);
  mySerial.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);
  Serial.println("GPS Module Initialized...");

  // Initialize CAN Bus
  while (CAN.begin(CANController::Mode::Normal) != CANController::OK) {
    Serial.println("CAN begin FAIL - delaying for 1 second");
    delay(1000);
  }
  Serial.println("CAN begin OK");
}

void loop() {
  while (mySerial.available() > 0) {
    gps.encode(mySerial.read());

    if (gps.location.isUpdated()) {
      float latitude = gps.location.lat();
      float longitude = gps.location.lng();
      uint16_t altitude = (uint16_t)gps.altitude.meters();
      uint16_t speed = (uint16_t)gps.speed.kmph();

      Serial.print("Latitude: "); Serial.println(latitude, 6);
      Serial.print("Longitude: "); Serial.println(longitude, 6);
      Serial.print("Altitude: "); Serial.println(altitude);
      Serial.print("Speed: "); Serial.println(speed);
      Serial.println("-------------------------");

      sendGPSDataToCAN(latitude, longitude, altitude, speed);
    }
  }
}

void sendGPSDataToCAN(float lat, float lng, uint16_t alt, uint16_t spd) {
  uint8_t data[8];

  // Convert float to 4 bytes (latitude)
  memcpy(&data[0], &lat, 4);
  // Convert float to 4 bytes (longitude)
  memcpy(&data[4], &lng, 4);
  CANFrame frame1(0x100, data, sizeof(data));
  CAN.write(frame1);
  frame1.print("CAN TX: Lat/Lng");

  // Send altitude and speed as 2-byte integers
  uint8_t altSpeedData[4];
  memcpy(&altSpeedData[0], &alt, 2);
  memcpy(&altSpeedData[2], &spd, 2);
  CANFrame frame2(0x101, altSpeedData, sizeof(altSpeedData));
  CAN.write(frame2);
  frame2.print("CAN TX: Alt/Speed");
}
