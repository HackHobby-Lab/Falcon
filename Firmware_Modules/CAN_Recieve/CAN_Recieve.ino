
#include "AA_MCP2515.h"

// TODO: modify CAN_BITRATE, CAN_PIN_CS(Chip Select) pin, and CAN_PIN_INT(Interrupt) pin as required.
const CANBitrate::Config CAN_BITRATE = CANBitrate::Config_8MHz_500kbps;

const uint8_t CAN_PIN_CS = 5;
const int8_t CAN_PIN_INT = 25;

CANConfig config(CAN_BITRATE, CAN_PIN_CS, CAN_PIN_INT);
CANController CAN(config);

void setup() {
  Serial.begin(115200);

  while(CAN.begin(CANController::Mode::Normal) != CANController::OK) {
    Serial.println("CAN begin FAIL - delaying for 1 second");
    delay(1000);
  }
  Serial.println("CAN begin OK");
}

void loop() {

  CANFrame frame;
  if (CAN.read(frame) == CANController::IOResult::OK) {
    frame.print("RX");
  }
  
  delay(500);
}
