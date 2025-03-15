#include "AA_MCP2515.h"
#include <map>
#include <TFT_eSPI.h>  // TFT display library

// CAN Configuration
const CANBitrate::Config CAN_BITRATE = CANBitrate::Config_8MHz_500kbps;
const uint8_t CAN_PIN_CS = 5;
const int8_t CAN_PIN_INT = 25;

CANConfig config(CAN_BITRATE, CAN_PIN_CS, CAN_PIN_INT);
CANController CAN(config);

// TFT Setup
TFT_eSPI tft = TFT_eSPI();
#define TEXT_SIZE 2
#define TEXT_COLOR TFT_WHITE

// Define expected module IDs
const uint16_t MODULE_IDS[] = {0x100, 0x203, 0x200, 0x101};
const char *MODULE_NAMES[] = {"GPS", "Raspberry Pi", "Magnetometer", "Obstacle"};

std::map<uint16_t, unsigned long> activeModules;
const unsigned long TIMEOUT_MS = 5000;  // Consider module offline if no data in 5 seconds

void loadingScreen(){
  tft.fillRect(0, 0, 480, 320, 0x0);

  tft.setTextColor(0xFFEA);
  tft.setTextSize(2);
  tft.setFreeFont(&FreeSerif24pt7b);
  tft.drawString("FALCON", 55, 109);

}

void UI(){
  tft.fillRect(0, 0, 480, 320, 0x0);

  tft.drawRect(122, -1, 200, 322, 0xFFEA);

  tft.setTextColor(0xFFEA);
  tft.setTextSize(1);
  tft.setFreeFont(&FreeSans9pt7b);
  tft.drawString("Nodes", 5, 10);

  tft.drawString("Status", 66, 10);

  tft.drawString("1", 24, 44);

  tft.drawString("2", 26, 84);

  tft.drawRect(61, -1, 61, 322, 0xFFEA);

  tft.drawRect(2, -1, 61, 322, 0xFFEA);

  tft.fillEllipse(91, 51, 10, 10, TFT_GREEN); //Green Means Online

  tft.fillEllipse(90, 90, 10, 10, TFT_GREEN);

  tft.fillEllipse(90, 129, 10, 10, 0xAD55); // Grey Means Offline

  tft.drawString("3", 26, 122);

  tft.drawString("4", 25, 163);

  tft.fillEllipse(90, 169, 10, 10, TFT_GREEN);

  tft.fillEllipse(90, 208, 10, 10, 0xAD55);

  tft.drawString("5", 26, 201);

  tft.fillRect(1, 290, 480, 30, 0x0);

  tft.drawString("6", 28, 243);

  tft.fillEllipse(90, 249, 10, 10, 0xAD55);

  tft.fillEllipse(29, 306, 10, 10, TFT_GREEN);

  tft.fillEllipse(245, 306, 10, 10, 0xAD55);

  tft.setTextColor(0xFFFF);
  tft.drawString("Online", 74, 299);

  tft.setTextColor(0xFFEA);
  tft.drawString("Description", 176, 10); //Description Column title

  tft.setTextColor(0xFFFF);
  tft.drawString("Offline", 330, 300);

  tft.drawString("Magnetometer", 165, 44); //Description text "Magnetometer"

  tft.drawString("GPS", 199, 83); //Description text "GPS"

  tft.drawString("Obstacle", 184, 122); //Description text "Obstacle"

  tft.drawString("Raspberry PI", 170, 162); //Description text "Raspberry Pi"




}

void nodeMag(bool online) {
    tft.fillEllipse(91, 51, 10, 10, online ? TFT_GREEN : TFT_DARKGREY);
    if (online) tft.drawString("Magnetometer", 165, 44);
}

void nodeGPS(bool online) {
    tft.fillEllipse(90, 90, 10, 10, online ? TFT_GREEN : TFT_DARKGREY);
    if (online) tft.drawString("GPS", 199, 83);
}

void nodeObstacle(bool online) {
    tft.fillEllipse(90, 129, 10, 10, online ? TFT_GREEN : TFT_DARKGREY);
    if (online) tft.drawString("Obstacle", 184, 122);
}

void nodePi(bool online) {
    tft.fillEllipse(90, 169, 10, 10, online ? TFT_GREEN : TFT_DARKGREY);
    if (online) tft.drawString("Raspberry PI", 170, 162);
}

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);  // Adjust rotation as needed
  loadingScreen();
   delay(3000);
  UI();
  nodeMag(false);
  nodeGPS(false);
  nodeObstacle(false);
  nodePi(false);
  // delay(3000);
  // tft.fillScreen(TFT_BLACK);
  // tft.setTextSize(TEXT_SIZE);
  // tft.setTextColor(TEXT_COLOR, TFT_BLACK);

  // // Display static labels
  // for (uint8_t i = 0; i < sizeof(MODULE_IDS) / sizeof(MODULE_IDS[0]); i++) {
  //   tft.setCursor(10, 20 + (i * 30));
  //   tft.print(MODULE_NAMES[i]);
  //   tft.print(": Offline");
  // }

  while (CAN.begin(CANController::Mode::Normal) != CANController::OK) {
    Serial.println("CAN begin FAIL - delaying for 1 second");
    delay(1000);
  }
  Serial.println("CAN begin OK");
}

void updateTFT(uint16_t moduleID, bool online) {
  for (uint8_t i = 0; i < sizeof(MODULE_IDS) / sizeof(MODULE_IDS[0]); i++) {
    if (moduleID == MODULE_IDS[i]) {
      int yPos = 44 + (i * 40);  // Adjust Y position dynamically

      // Clear previous description text
      //tft.fillRect(165, yPos, 150, 20, TFT_BLACK);

      // Call the appropriate node function
      // {"GPS", "Speed", "Magnetometer", "Obstacle"};
      switch (i) {
        case 0: nodeGPS(online); break;
        case 1: nodePi(online); break;
        case 2: nodeMag(online); break;
        case 3: nodeObstacle(online); break;
      }
    }
  }
}



void checkInactiveModules() {
  unsigned long currentTime = millis();

  for (uint8_t i = 0; i < sizeof(MODULE_IDS) / sizeof(MODULE_IDS[0]); i++) {
    uint16_t moduleID = MODULE_IDS[i];

    if (activeModules.count(moduleID) && (currentTime - activeModules[moduleID] > TIMEOUT_MS)) {
      Serial.print(MODULE_NAMES[i]);
      Serial.println(" Module is OFFLINE");
      activeModules.erase(moduleID);  // Remove from tracking
      updateTFT(moduleID, false);
    }
  }
}



void loop() {
  CANFrame frame;

  if (CAN.read(frame) == CANController::IOResult::OK) {
    uint16_t moduleID = frame.getId();

    // Check if the received ID matches any expected module ID
    for (uint8_t i = 0; i < sizeof(MODULE_IDS) / sizeof(MODULE_IDS[0]); i++) {
      if (moduleID == MODULE_IDS[i]) {
        activeModules[moduleID] = millis();  // Update timestamp
        Serial.print(MODULE_NAMES[i]);
        Serial.println(" Module is ONLINE");
        updateTFT(moduleID, true);
      }
    }

    // Print received frame data
    frame.print("RX");
  }

  checkInactiveModules();
  delay(50);
}
