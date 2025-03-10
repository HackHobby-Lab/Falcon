#include <TinyGPS++.h>
#include <HardwareSerial.h>

// Use Serial2 for GPS (or change RX/TX pins as needed)
HardwareSerial mySerial(2);  
TinyGPSPlus gps;

// Define GPS module connections
#define RXPin 16  // Connect to GPS TX
#define TXPin 17  // Connect to GPS RX
#define GPSBaud 9600  // GPS default baud rate

void setup() {
  Serial.begin(115200);  // Serial monitor
  mySerial.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin); // GPS Serial
  Serial.println("GPS Module Initialized...");
}

void loop() {
  while (mySerial.available() > 0) {  // Read GPS data
    gps.encode(mySerial.read());

    if (gps.location.isUpdated()) {
      Serial.print("Latitude: ");
      Serial.println(gps.location.lat(), 6);
      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(), 6);
      Serial.print("Altitude: ");
      Serial.print(gps.altitude.meters());
      Serial.println(" meters");
      Serial.print("Speed: ");
      Serial.print(gps.speed.kmph());
      Serial.println(" km/h");
      Serial.println("-------------------------");
    }
  }
}
