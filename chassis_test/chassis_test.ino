#include <ESP32Servo.h>  // Use ESP32Servo instead of the standard Servo library

// Define pins for motor and servo
#define MOTOR_PIN 13     // Pin for DC motor control (ESC)
#define SERVO_PIN 14     // Pin for steering servo

// Motor PWM settings
#define REVERSE_DUTY_CYCLE 10  // Reverse duty cycle (10%)
#define NEUTRAL_DUTY_CYCLE 15  // Neutral duty cycle (15%)
#define FORWARD_DUTY_CYCLE 20  // Forward duty cycle (20%)

// Servo steering settings
#define LEFT_STEER 10    // Left steer duty cycle (10%)
#define STRAIGHT_STEER 15 // Straight steer duty cycle (15%)
#define RIGHT_STEER 20   // Right steer duty cycle (20%)

Servo steeringServo; // Servo object for steering control

void setup() {
  // Initialize motor and servo pins
  pinMode(MOTOR_PIN, OUTPUT);
  steeringServo.attach(SERVO_PIN); // Attach servo to specified pin

  // Start with neutral position for both motor and servo
  analogWrite(MOTOR_PIN, NEUTRAL_DUTY_CYCLE);
  steeringServo.write(STRAIGHT_STEER); // Steering straight
}

void loop() {
  // Motor control: Forward
  motorControl(FORWARD_DUTY_CYCLE);
  delay(2000); // Wait for 2 seconds

  // Motor control: Neutral (stopping the car)
  motorControl(NEUTRAL_DUTY_CYCLE);
  delay(2000); // Wait for 2 seconds

  // Motor control: Reverse
  motorControl(REVERSE_DUTY_CYCLE);
  delay(2000); // Wait for 2 seconds

  // Servo control: Left steer
  steerControl(LEFT_STEER);
  delay(2000); // Wait for 2 seconds

  // Servo control: Right steer
  steerControl(RIGHT_STEER);
  delay(2000); // Wait for 2 seconds

  // Servo control: Straight steer
  steerControl(STRAIGHT_STEER);
  delay(2000); // Wait for 2 seconds
}

// Function to control the DC motor (ESC)
void motorControl(int dutyCycle) {
  analogWrite(MOTOR_PIN, dutyCycle);
}

// Function to control the steering servo motor
void steerControl(int steerPosition) {
  steeringServo.write(steerPosition);
}
