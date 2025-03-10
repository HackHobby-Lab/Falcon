#include <SPI.h>
#include <mcp_can.h>

#define CAN_CS 5  // MCP2515 SPI CS pin

// Define Ultrasonic Sensor Pins
#define TRIG_LEFT 14
#define ECHO_LEFT 27
#define TRIG_CENTER 26
#define ECHO_CENTER 25
#define TRIG_RIGHT 33
#define ECHO_RIGHT 32
#define TRIG_BACK 4
#define ECHO_BACK 2

// Threshold for obstacle detection (in cm)
#define THRESHOLD 10  

MCP_CAN CAN0(CAN_CS); // Create CAN object

// Function to measure distance
long getDistance(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    long duration = pulseIn(echoPin, HIGH, 30000);  // Timeout at 30ms (max ~5m)
    return duration * 0.034 / 2;  // Convert to cm
}

void setup() {
    Serial.begin(115200);

    // Initialize Ultrasonic Sensors
    pinMode(TRIG_LEFT, OUTPUT); pinMode(ECHO_LEFT, INPUT);
    pinMode(TRIG_CENTER, OUTPUT); pinMode(ECHO_CENTER, INPUT);
    pinMode(TRIG_RIGHT, OUTPUT); pinMode(ECHO_RIGHT, INPUT);
    pinMode(TRIG_BACK, OUTPUT); pinMode(ECHO_BACK, INPUT);

    // Initialize MCP2515 CAN Module
    if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
        Serial.println("MCP2515 Initialized Successfully");
    } else {
        Serial.println("MCP2515 Initialization Failed!");
        while (1);
    }
    CAN0.setMode(MCP_NORMAL);
    Serial.println("CAN BUS Ready");
}

void loop() {
    long distLeft = getDistance(TRIG_LEFT, ECHO_LEFT);
    long distCenter = getDistance(TRIG_CENTER, ECHO_CENTER);
    long distRight = getDistance(TRIG_RIGHT, ECHO_RIGHT);
    long distBack = getDistance(TRIG_BACK, ECHO_BACK);

    String message = "";

    // Check for obstacles
    if (distLeft < THRESHOLD) message = "ObsLeft";
    if (distCenter < THRESHOLD) message = "ObsCntr";
    if (distRight < THRESHOLD) message = "ObsRight";
    if (distBack < THRESHOLD) message = "ObsBack";

    // Send message if obstacle is detected
    if (message != "") {
        Serial.println("Sending CAN Message: " + message);

        unsigned char msgBuf[8] = {0};  // Ensure full 8-byte frame
        memcpy(msgBuf, message.c_str(), message.length());

        if (CAN0.sendMsgBuf(0x101, 0, 8, msgBuf) == CAN_OK) {
            Serial.println("Message Sent Successfully!");
        } else {
            Serial.println("Error Sending Message!");
        }
    }

    delay(500);
}
