#include <Arduino.h>

// Define the pins
const int pin1 = 8;  // Set pin 1
const int pin2 = 9;  // Set pin 2

void setup() {
  Serial.begin(9600); // Initialize serial communication at a baud rate of 9600
  // Initialize pins as outputs
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);

  // Set initial states to LOW
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
}

void controlActuator(int command) {
  if (command == 1.0) {
    // Extend actuator
    // Make pin1 HIGH for 5 seconds, then LOW
  digitalWrite(pin1, HIGH);
  delay(5000);
  digitalWrite(pin1, LOW);
  } else if (command == 2.0) {
    // Retract actuator
    // Make pin2 HIGH for 5 seconds, then LOW
    digitalWrite(pin2, HIGH);
  delay(5000);
  digitalWrite(pin2, LOW);
  }
}

void loop() {
  if (Serial.available() > 0) {
    int command = Serial.parseInt(); // Read the command from ROS
    controlActuator(command);
  }
  
}

